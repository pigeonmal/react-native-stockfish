# @pigeonmal/react-native-stockfish

Source repository: https://github.com/pigeonmal/react-native-stockfish

Mobile-first Stockfish 18 for React Native, exposed through Nitro Modules and built with React Native Builder Bob.

The package embeds Stockfish 18's compact NNUE network only, keeping the download and native memory footprint small for mobile. This Stockfish Lite profile trades some playing strength for a much smaller package and keeps search on native worker threads. It defaults to one thread, a 16 MiB hash, and a 250 ms move time so a game UI stays responsive and battery use is predictable.

## Install

```sh
npm install @pigeonmal/react-native-stockfish react-native-nitro-modules
```

For GitHub Packages, authenticate with a classic personal access token that can read packages and add this to the consuming app's `.npmrc`:

```ini
@pigeonmal:registry=https://npm.pkg.github.com
```

## Usage

```ts
import { Stockfish } from '@pigeonmal/react-native-stockfish'

const engine = await Stockfish.createEngine({
  threads: 2,
  hashSizeMb: 32,
})

const subscription = engine.addOnAnalysisInfoListener((info) => {
  console.log(info.depth, info.score.kind, info.score.value, info.pv)
})

try {
  await engine.setPosition(
    'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1',
    [],
  )
  const result = await engine.startSearch({ moveTimeMs: 300 })
  console.log(result.bestMove)
} finally {
  subscription.remove()
  engine.stop()
}
```

`startSearch({ infinite: true })` is useful for analysis screens; call `stop()` to resolve it with the current best move. Calling `setPosition()` or `clearHash()` while a search is active stops and waits for that search on a native worker.

## Development

```sh
bun install
bun run specs
bun run typecheck
bun run build
bun run pack:check
```

The C++ implementation is linked against the official Stockfish 18 source under `third-party/stockfish`, with the evaluator configured to use its compact network only. Stockfish is GPLv3-licensed; see [`third-party/stockfish/Copying.txt`](third-party/stockfish/Copying.txt) and [`third-party/stockfish/AUTHORS`](third-party/stockfish/AUTHORS).

## Publishing

The package is configured for GitHub Packages at `https://npm.pkg.github.com`. After building and checking the tarball, publish with:

```sh
npm publish
```

The publisher must already be authenticated to GitHub Packages; no token is stored in this repository.
