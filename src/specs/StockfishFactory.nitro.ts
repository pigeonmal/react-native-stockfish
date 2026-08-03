import type { HybridObject } from 'react-native-nitro-modules'
import type { EngineOptions } from '../types/Options'
import type { StockfishEngine } from './StockfishEngine.nitro'

/**
 * Creates ready-to-use native Stockfish engines.
 *
 * @see {@linkcode StockfishEngine}
 */
export interface StockfishFactory extends HybridObject<{ ios: 'c++'; android: 'c++' }> {
  /**
   * Creates an engine and loads its embedded NNUE networks off the JS thread.
   *
   * @param options Optional mobile engine configuration.
   * @returns A fully initialized engine that can accept a position immediately.
   */
  createEngine(options?: EngineOptions): Promise<StockfishEngine>
}
