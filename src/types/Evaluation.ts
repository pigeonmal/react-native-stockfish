import type { StockfishEngine } from '../specs/StockfishEngine.nitro'

/**
 * Describes which scale is used by an {@linkcode Evaluation} value.
 *
 * @see {@linkcode Evaluation.kind}
 */
export type EvaluationKind = 'centipawns' | 'mate' | 'tablebase'

/**
 * A search score reported by {@linkcode StockfishEngine}.
 *
 * `centipawns` values are from the side to move's perspective. `mate` values
 * contain the signed number of plies to mate, and `tablebase` values contain
 * the signed tablebase distance-to-zeroing move when available.
 *
 * @see {@linkcode StockfishEngine.startSearch}
 */
export interface Evaluation {
  /**
   * The score scale used by `value`.
   */
  kind: EvaluationKind

  /**
   * The score in the units described by {@linkcode Evaluation.kind}.
   */
  value: number
}
