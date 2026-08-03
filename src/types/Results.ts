import type { Int64 } from 'react-native-nitro-modules'
import type { StockfishEngine } from '../specs/StockfishEngine.nitro'
import type { Evaluation } from './Evaluation'

/**
 * A low-allocation principal-variation update from
 * {@linkcode StockfishEngine.addOnAnalysisInfoListener}.
 */
export interface AnalysisInfo {
  /**
   * Completed iterative-deepening depth.
   */
  depth: number

  /**
   * Selective search depth reached for this line.
   */
  selDepth: number

  /**
   * One-based MultiPV line number.
   */
  multiPv: number

  /**
   * Score for the current principal variation.
   */
  score: Evaluation

  /**
   * Elapsed search time in milliseconds.
   */
  timeMs: number

  /**
   * Number of searched nodes.
   */
  nodes: Int64

  /**
   * Nodes searched per second.
   */
  nps: number

  /**
   * Principal variation in UCI coordinate notation.
   */
  pv: string[]

  /**
   * Transposition-table occupancy in per mille.
   */
  hashfull: number
}

/**
 * The terminal result resolved by {@linkcode StockfishEngine.startSearch}.
 */
export interface BestMoveResult {
  /**
   * Best move in UCI coordinate notation, or `0000` when no legal move exists.
   */
  bestMove: string

  /**
   * Optional ponder move in UCI coordinate notation.
   */
  ponderMove?: string

  /**
   * Final score reported by Stockfish.
   */
  score: Evaluation

  /**
   * Deepest completed search depth.
   */
  depth: number

  /**
   * Number of searched nodes.
   */
  nodes: Int64

  /**
   * Total search time in milliseconds.
   */
  timeMs: number
}
