#pragma once

#include "HybridStockfishEngineSpec.hpp"
#include "EngineOptions.hpp"
#include "../third-party/stockfish/src/engine.h"

#include <cstdint>
#include <memory>
#include <mutex>
#include <optional>
#include <unordered_map>

namespace margelo::nitro::stockfish {

class HybridStockfishEngine final : public HybridStockfishEngineSpec {
public:
  explicit HybridStockfishEngine(const EngineOptions& options);
  ~HybridStockfishEngine() override;

  std::shared_ptr<Promise<void>> setPosition(const std::string& fen,
                                              const std::vector<std::string>& moves) override;
  std::shared_ptr<Promise<BestMoveResult>> startSearch(
      const std::optional<SearchOptions>& options) override;
  void stop() override;
  std::shared_ptr<Promise<void>> clearHash() override;
  ListenerSubscription addOnAnalysisInfoListener(
      const std::function<void(const AnalysisInfo&)>& listener) override;

protected:
  size_t getExternalMemorySize() noexcept override;

private:
  void applyEngineOptions(const EngineOptions& options);
  void beginSearch(const std::optional<SearchOptions>& options,
                   const std::shared_ptr<Promise<BestMoveResult>>& promise);
  void stopAndWait();
  void removeListener(uint64_t listenerId);
  void emitAnalysisInfo(const AnalysisInfo& info);
  void emitNoMoves(const Stockfish::Engine::InfoShort& info);
  void finishSearch(std::string_view bestMove, std::string_view ponderMove);

  std::unique_ptr<Stockfish::Engine> engine_;
  std::mutex stateMutex_;
  bool searching_ = false;
  bool operationInProgress_ = false;
  bool stopRequested_ = false;
  uint64_t nextListenerId_ = 1;
  std::shared_ptr<Promise<BestMoveResult>> pendingSearch_;
  std::shared_ptr<HybridStockfishEngine> activeSearchSelf_;
  std::optional<AnalysisInfo> latestInfo_;
  std::unordered_map<uint64_t, std::function<void(const AnalysisInfo&)>> listeners_;
  size_t estimatedMemorySizeBytes_ = 0;

public:
  static constexpr auto TAG = "StockfishEngine";
};

} // namespace margelo::nitro::stockfish
