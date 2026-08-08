import type { Int64 } from 'react-native-nitro-modules'

/**
 * Options used when creating a ready
 * {@linkcode import('../specs/StockfishEngine.nitro').StockfishEngine}.
 *
 * @see {@linkcode import('../specs/StockfishFactory.nitro').StockfishFactory.createEngine}
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
   * Number of principal variations calculated for each completed depth.
   * Defaults to `1`. Analysis screens commonly use `3`; keeping the default
   * at one avoids extra CPU work for play and hint use cases.
   */
  multiPv?: number

  /**
   * Enables Chess960 move generation and castling rules. Defaults to `false`.
   */
  chess960?: boolean
}

/**
 * Limits for one
 * {@linkcode import('../specs/StockfishEngine.nitro').StockfishEngine.startSearch}
 * call.
 *
 * Omitted limits use a mobile-friendly 250 ms move time. Multiple concrete
 * limits may be combined; `infinite` is mutually exclusive with the other
 * limits and requires
 * {@linkcode import('../specs/StockfishEngine.nitro').StockfishEngine.stop} to
 * finish the search.
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
