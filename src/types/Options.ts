import type { Int64 } from 'react-native-nitro-modules'
import type { StockfishEngine } from '../specs/StockfishEngine.nitro'
import type { StockfishFactory } from '../specs/StockfishFactory.nitro'

/**
 * Options used when creating a ready {@linkcode StockfishEngine}.
 *
 * @see {@linkcode StockfishFactory.createEngine}
 */
export interface EngineOptions {
  /**
   * Number of search threads. Defaults to `1`, which is the best mobile
   * default for predictable battery and thermal usage.
   */
  threads?: number

  /**
   * Transposition-table size in mebibytes. Defaults to `16`.
   */
  hashSizeMb?: number

  /**
   * Stockfish skill level from `0` to `20`. Defaults to `20`.
   */
  skillLevel?: number

  /**
   * Enables Chess960 move generation and castling rules. Defaults to `false`.
   */
  chess960?: boolean
}

/**
 * Limits for one {@linkcode StockfishEngine.startSearch} call.
 *
 * Omitted limits use a mobile-friendly 250 ms move time. Multiple concrete
 * limits may be combined; `infinite` is mutually exclusive with the other
 * limits and requires {@linkcode StockfishEngine.stop} to finish the search.
 */
export interface SearchOptions {
  /**
   * Maximum search time in milliseconds.
   */
  moveTimeMs?: number

  /**
   * Maximum iterative-deepening depth.
   */
  depth?: number

  /**
   * Maximum number of searched nodes.
   */
  nodes?: Int64

  /**
   * Searches until stopped explicitly.
   */
  infinite?: boolean
}
