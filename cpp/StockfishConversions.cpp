#include "StockfishConversions.hpp"

#include <sstream>
#include <type_traits>

namespace margelo::nitro::stockfish {

namespace {

std::vector<std::string> splitPrincipalVariation(std::string_view pv) {
  std::istringstream stream{std::string(pv)};
  std::vector<std::string> moves;
  std::string move;
  while (stream >> move) {
    moves.emplace_back(std::move(move));
  }
  return moves;
}

Evaluation zeroEvaluation() {
  return Evaluation(EvaluationKind::CENTIPAWNS, 0.0);
}

} // namespace

Evaluation toEvaluation(const Stockfish::Score& score) {
  return score.visit([](const auto& value) -> Evaluation {
    using ValueType = std::decay_t<decltype(value)>;
    if constexpr (std::is_same_v<ValueType, Stockfish::Score::Mate>) {
      return Evaluation(EvaluationKind::MATE, static_cast<double>(value.plies));
    } else if constexpr (std::is_same_v<ValueType, Stockfish::Score::Tablebase>) {
      return Evaluation(EvaluationKind::TABLEBASE, static_cast<double>(value.plies));
    } else {
      return Evaluation(EvaluationKind::CENTIPAWNS, static_cast<double>(value.value));
    }
  });
}

AnalysisInfo toAnalysisInfo(const Stockfish::Engine::InfoFull& info) {
  return AnalysisInfo(
      static_cast<double>(info.depth),
      static_cast<double>(info.selDepth),
      static_cast<double>(info.multiPV),
      toEvaluation(info.score),
      static_cast<double>(info.timeMs),
      static_cast<int64_t>(info.nodes),
      static_cast<double>(info.nps),
      splitPrincipalVariation(info.pv),
      static_cast<double>(info.hashfull));
}

AnalysisInfo toAnalysisInfo(const Stockfish::Engine::InfoShort& info) {
  return AnalysisInfo(
      static_cast<double>(info.depth),
      static_cast<double>(info.depth),
      1.0,
      toEvaluation(info.score),
      0.0,
      0,
      0.0,
      {},
      0.0);
}

} // namespace margelo::nitro::stockfish
