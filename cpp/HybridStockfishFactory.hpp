#pragma once

#include "HybridStockfishFactorySpec.hpp"

namespace margelo::nitro::stockfish {

class HybridStockfishFactory final : public HybridStockfishFactorySpec {
public:
  HybridStockfishFactory() : HybridObject(TAG) {}

  std::shared_ptr<Promise<std::shared_ptr<HybridStockfishEngineSpec>>>
  createEngine(const std::optional<EngineOptions>& options) override;
};

} // namespace margelo::nitro::stockfish
