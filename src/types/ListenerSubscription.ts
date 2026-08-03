/**
 * Owns one native listener registration created by a Stockfish engine.
 *
 * @see {@linkcode import('../specs/StockfishEngine.nitro').StockfishEngine.addOnAnalysisInfoListener}
 */
export interface ListenerSubscription {
  /**
   * Removes this listener. Calling `remove` more than once is safe.
   */
  remove: () => void
}
