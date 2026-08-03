#pragma once

#include "AnalysisInfo.hpp"
#include "BestMoveResult.hpp"
#include "Evaluation.hpp"
#include "../third-party/stockfish/src/engine.h"

namespace margelo::nitro::stockfish {

Evaluation toEvaluation(const Stockfish::Score& score);
AnalysisInfo toAnalysisInfo(const Stockfish::Engine::InfoFull& info);
AnalysisInfo toAnalysisInfo(const Stockfish::Engine::InfoShort& info);

} // namespace margelo::nitro::stockfish
