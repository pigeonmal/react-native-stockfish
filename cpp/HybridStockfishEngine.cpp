#include "HybridStockfishEngine.hpp"

#include "StockfishConversions.hpp"
#include "../third-party/stockfish/src/bitboard.h"

#include <cmath>
#include <exception>
#include <limits>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace margelo::nitro::stockfish {

namespace {

constexpr double kDefaultMoveTimeMs = 250.0;
constexpr int kDefaultThreads = 1;
constexpr int kDefaultHashSizeMb = 16;
constexpr int kDefaultSkillLevel = 20;
constexpr int kDefaultMultiPv = 1;
constexpr int kMaxMobileThreads = 64;
constexpr int kMaxMobileHashSizeMb = 1024;
constexpr int kMaxMobileMultiPv = 5;

int checkedIntegerOption(const char* name, double value, int minimum, int maximum) {
  if (!std::isfinite(value) || std::floor(value) != value || value < minimum || value > maximum) {
    throw std::invalid_argument(std::string(name) + " must be an integer in [" +
                                std::to_string(minimum) + ", " + std::to_string(maximum) + "]");
  }
  return static_cast<int>(value);
}

void setUciOption(Stockfish::Engine& engine, const std::string& name, const std::string& value) {
  std::istringstream stream("name " + name + " value " + value);
  engine.get_options().setoption(stream);
}

std::unique_ptr<Stockfish::Engine> createStockfishEngine() {
  static std::once_flag initializationFlag;
  std::call_once(initializationFlag, [] {
    // Stockfish's executable initializes these tables from main(). Nitro
    // embeds Engine directly, so the library must perform the same startup
    // sequence before Position::set() computes its state.
    Stockfish::Bitboards::init();
    Stockfish::Position::init();
  });

  return std::make_unique<Stockfish::Engine>();
}

BestMoveResult resultFromInfo(const AnalysisInfo& info, std::string_view bestMove,
                              std::string_view ponderMove) {
  std::optional<std::string> ponder;
  if (!ponderMove.empty()) {
    ponder = std::string(ponderMove);
  }
  return BestMoveResult(
      std::string(bestMove),
      std::move(ponder),
      info.score,
      info.depth,
      info.nodes,
      info.timeMs);
}

AnalysisInfo defaultInfo() {
  return AnalysisInfo(
      0.0,
      0.0,
      1.0,
      Evaluation(EvaluationKind::CENTIPAWNS, 0.0),
      0.0,
      0,
      0.0,
      {},
      0.0);
}

} // namespace

HybridStockfishEngine::HybridStockfishEngine(const EngineOptions& options)
    : HybridObject(TAG),
      engine_(createStockfishEngine()),
      estimatedMemorySizeBytes_(static_cast<size_t>(
          checkedIntegerOption(
              "hashSizeMb",
              options.hashSizeMb.value_or(static_cast<double>(kDefaultHashSizeMb)),
              1,
              kMaxMobileHashSizeMb)) * 1024u * 1024u +
          16u * 1024u * 1024u) {
  engine_->set_on_update_full([this](const auto& info) {
    emitAnalysisInfo(toAnalysisInfo(info));
  });
  engine_->set_on_update_no_moves([this](const auto& info) {
    emitNoMoves(info);
  });
  engine_->set_on_bestmove([this](std::string_view bestMove, std::string_view ponderMove) {
    finishSearch(bestMove, ponderMove);
  });
  applyEngineOptions(options);
}

HybridStockfishEngine::~HybridStockfishEngine() {
  if (engine_ != nullptr) {
    engine_->stop();
    engine_->wait_for_search_finished();
  }
}

void HybridStockfishEngine::applyEngineOptions(const EngineOptions& options) {
  const int threads = checkedIntegerOption(
      "threads", options.threads.value_or(static_cast<double>(kDefaultThreads)), 1,
      kMaxMobileThreads);
  const int hashSizeMb = checkedIntegerOption(
      "hashSizeMb", options.hashSizeMb.value_or(static_cast<double>(kDefaultHashSizeMb)), 1,
      kMaxMobileHashSizeMb);
  const int skillLevel = checkedIntegerOption(
      "skillLevel", options.skillLevel.value_or(static_cast<double>(kDefaultSkillLevel)), 0, 20);
  const int multiPv = checkedIntegerOption(
      "multiPv", options.multiPv.value_or(static_cast<double>(kDefaultMultiPv)), 1,
      kMaxMobileMultiPv);

  setUciOption(*engine_, "Threads", std::to_string(threads));
  setUciOption(*engine_, "Hash", std::to_string(hashSizeMb));
  setUciOption(*engine_, "Skill Level", std::to_string(skillLevel));
  setUciOption(*engine_, "MultiPV", std::to_string(multiPv));
  setUciOption(*engine_, "UCI_Chess960", options.chess960.value_or(false) ? "true" : "false");
}

std::shared_ptr<Promise<void>> HybridStockfishEngine::setPosition(
    const std::string& fen, const std::vector<std::string>& moves) {
  if (fen.empty()) {
    return Promise<void>::rejected(
        std::make_exception_ptr(std::invalid_argument("fen must not be empty")));
  }

  auto self = shared_cast<HybridStockfishEngine>();
  return Promise<void>::async([self, fen, moves]() {
    {
      std::unique_lock lock(self->stateMutex_);
      if (self->operationInProgress_) {
        throw std::runtime_error("Another engine operation is already in progress");
      }
      self->operationInProgress_ = true;
      if (self->searching_) {
        self->engine_->stop();
      }
    }

    try {
      self->stopAndWait();
      self->engine_->set_position(fen, moves);
      std::lock_guard lock(self->stateMutex_);
      self->latestInfo_.reset();
      self->operationInProgress_ = false;
    } catch (...) {
      std::lock_guard lock(self->stateMutex_);
      self->operationInProgress_ = false;
      throw;
    }
  });
}

std::shared_ptr<Promise<BestMoveResult>> HybridStockfishEngine::startSearch(
    const std::optional<SearchOptions>& options) {
  auto promise = Promise<BestMoveResult>::create();
  auto self = shared_cast<HybridStockfishEngine>();
  Promise<void>::async([self, options, promise]() {
    try {
      self->beginSearch(options, promise);
    } catch (...) {
      promise->reject(std::current_exception());
    }
  });
  return promise;
}

void HybridStockfishEngine::beginSearch(
    const std::optional<SearchOptions>& options,
    const std::shared_ptr<Promise<BestMoveResult>>& promise) {
  Stockfish::Search::LimitsType limits;
  const SearchOptions resolved = options.value_or(SearchOptions{});

  if (resolved.infinite.value_or(false) &&
      (resolved.moveTimeMs.has_value() || resolved.depth.has_value() || resolved.nodes.has_value())) {
    throw std::invalid_argument("infinite cannot be combined with moveTimeMs, depth, or nodes");
  }
  if (resolved.moveTimeMs.has_value()) {
    const double moveTimeMs = *resolved.moveTimeMs;
    if (!std::isfinite(moveTimeMs) || moveTimeMs < 1.0 || moveTimeMs > 86400000.0) {
      throw std::invalid_argument("moveTimeMs must be between 1 and 86400000");
    }
    limits.movetime = static_cast<Stockfish::TimePoint>(moveTimeMs);
  } else if (!resolved.depth.has_value() && !resolved.nodes.has_value() &&
             !resolved.infinite.value_or(false)) {
    limits.movetime = static_cast<Stockfish::TimePoint>(kDefaultMoveTimeMs);
  }
  if (resolved.depth.has_value()) {
    limits.depth = checkedIntegerOption("depth", *resolved.depth, 1, 200);
  }
  if (resolved.nodes.has_value()) {
    if (*resolved.nodes <= 0) {
      throw std::invalid_argument("nodes must be greater than zero");
    }
    limits.nodes = static_cast<uint64_t>(*resolved.nodes);
  }
  if (resolved.infinite.value_or(false)) {
    limits.infinite = 1;
  }

  {
    std::lock_guard lock(stateMutex_);
    if (operationInProgress_ || searching_) {
      throw std::runtime_error("Another search or engine operation is already in progress");
    }
    operationInProgress_ = true;
    stopRequested_ = false;
    latestInfo_.reset();
    pendingSearch_ = promise;
    activeSearchSelf_ = shared_cast<HybridStockfishEngine>();
    searching_ = true;
  }

  try {
    engine_->go(limits);
    bool stopRequested;
    {
      std::lock_guard lock(stateMutex_);
      operationInProgress_ = false;
      stopRequested = stopRequested_;
    }
    if (stopRequested) {
      engine_->stop();
    }
  } catch (...) {
    std::lock_guard lock(stateMutex_);
    searching_ = false;
    pendingSearch_.reset();
    activeSearchSelf_.reset();
    operationInProgress_ = false;
    stopRequested_ = false;
    throw;
  }
}

void HybridStockfishEngine::stop() {
  std::lock_guard lock(stateMutex_);
  if (operationInProgress_) {
    stopRequested_ = true;
  } else if (searching_) {
    engine_->stop();
  }
}

std::shared_ptr<Promise<void>> HybridStockfishEngine::clearHash() {
  auto self = shared_cast<HybridStockfishEngine>();
  return Promise<void>::async([self]() {
    {
      std::lock_guard lock(self->stateMutex_);
      if (self->operationInProgress_) {
        throw std::runtime_error("Another engine operation is already in progress");
      }
      self->operationInProgress_ = true;
      if (self->searching_) {
        self->engine_->stop();
      }
    }
    try {
      self->stopAndWait();
      self->engine_->search_clear();
      std::lock_guard lock(self->stateMutex_);
      self->latestInfo_.reset();
      self->operationInProgress_ = false;
    } catch (...) {
      std::lock_guard lock(self->stateMutex_);
      self->operationInProgress_ = false;
      throw;
    }
  });
}

ListenerSubscription HybridStockfishEngine::addOnAnalysisInfoListener(
    const std::function<void(const AnalysisInfo&)>& listener) {
  auto self = shared_cast<HybridStockfishEngine>();
  uint64_t listenerId;
  {
    std::lock_guard lock(stateMutex_);
    listenerId = nextListenerId_++;
    listeners_.emplace(listenerId, listener);
  }
  std::weak_ptr<HybridStockfishEngine> weakSelf = self;
  return ListenerSubscription([weakSelf, listenerId]() {
    if (auto owner = weakSelf.lock()) {
      owner->removeListener(listenerId);
    }
  });
}

void HybridStockfishEngine::removeListener(uint64_t listenerId) {
  std::lock_guard lock(stateMutex_);
  listeners_.erase(listenerId);
}

void HybridStockfishEngine::emitAnalysisInfo(const AnalysisInfo& info) {
  std::vector<std::function<void(const AnalysisInfo&)>> listeners;
  {
    std::lock_guard lock(stateMutex_);
    latestInfo_ = info;
    listeners.reserve(listeners_.size());
    for (const auto& [id, listener] : listeners_) {
      (void)id;
      listeners.push_back(listener);
    }
  }
  for (const auto& listener : listeners) {
    listener(info);
  }
}

void HybridStockfishEngine::emitNoMoves(const Stockfish::Engine::InfoShort& info) {
  emitAnalysisInfo(toAnalysisInfo(info));
}

void HybridStockfishEngine::finishSearch(std::string_view bestMove, std::string_view ponderMove) {
  std::shared_ptr<Promise<BestMoveResult>> promise;
  std::shared_ptr<HybridStockfishEngine> keepAlive;
  BestMoveResult result;
  {
    std::lock_guard lock(stateMutex_);
    if (!searching_ || pendingSearch_ == nullptr) {
      return;
    }
    const AnalysisInfo info = latestInfo_.value_or(defaultInfo());
    result = resultFromInfo(info, bestMove, ponderMove);
    promise = std::move(pendingSearch_);
    keepAlive = std::move(activeSearchSelf_);
    searching_ = false;
  }
  promise->resolve(std::move(result));

  // The best-move callback runs on Stockfish's search thread. Releasing the
  // self-retain inline can invoke this object's destructor on that same thread,
  // where waiting for search completion would deadlock. Hand the final release
  // to Nitro's worker pool; the destructor can then safely stop/wait even when
  // JavaScript dropped its engine reference while the search was finishing.
  if (keepAlive != nullptr) {
    (void)Promise<void>::async(
        [keepAlive = std::move(keepAlive)]() { (void)keepAlive; });
  }
}

void HybridStockfishEngine::stopAndWait() {
  {
    std::lock_guard lock(stateMutex_);
    if (searching_) {
      engine_->stop();
    }
  }
  engine_->wait_for_search_finished();
}

size_t HybridStockfishEngine::getExternalMemorySize() noexcept {
  return estimatedMemorySizeBytes_;
}

} // namespace margelo::nitro::stockfish
