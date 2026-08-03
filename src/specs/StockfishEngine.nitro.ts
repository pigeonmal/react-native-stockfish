import type { HybridObject } from 'react-native-nitro-modules'
import type { ListenerSubscription } from '../types/ListenerSubscription'
import type { SearchOptions } from '../types/Options'
import type { AnalysisInfo, BestMoveResult } from '../types/Results'
import type { StockfishFactory } from './StockfishFactory.nitro'

/**
 * A ready, stateful Stockfish search engine created by
 * {@linkcode StockfishFactory.createEngine}.
 */
export interface StockfishEngine extends HybridObject<{ ios: 'c++'; android: 'c++' }> {
  /**
   * Replaces the current position and move history. The operation stops and
   * waits for an active search before applying the new position.
   *
   * @param fen Position in Forsyth-Edwards Notation.
   * @param moves Moves after `fen`, in UCI coordinate notation.
   */
  setPosition(fen: string, moves: string[]): Promise<void>

  /**
   * Starts a search. The returned promise resolves when Stockfish emits its
   * best move, including after an explicit {@linkcode StockfishEngine.stop}.
   *
   * @throws If another search or position operation is already in progress.
   */
  startSearch(options?: SearchOptions): Promise<BestMoveResult>

  /**
   * Stops the active search without blocking the JS thread.
   */
  stop(): void

  /**
   * Clears the transposition table off the JS thread.
   */
  clearHash(): Promise<void>

  /**
   * Adds a listener for incremental principal-variation updates.
   *
   * Listener callbacks are delivered from native search workers. Remove the
   * returned subscription when the owning screen or game is disposed.
   */
  addOnAnalysisInfoListener(listener: (info: AnalysisInfo) => void): ListenerSubscription
}
