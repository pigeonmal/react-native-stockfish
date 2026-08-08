import { NitroModules } from 'react-native-nitro-modules'
import type { StockfishFactory } from './specs/StockfishFactory.nitro'

export const Stockfish =
  NitroModules.createHybridObject<StockfishFactory>('StockfishFactory')

export type { StockfishEngine } from './specs/StockfishEngine.nitro'
export type { StockfishFactory } from './specs/StockfishFactory.nitro'
export type { Evaluation, EvaluationKind } from './types/Evaluation'
export type { EngineOptions, SearchOptions } from './types/Options'
export type { AnalysisInfo, BestMoveResult } from './types/Results'
export type { ListenerSubscription } from './types/ListenerSubscription'
