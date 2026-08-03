#include "HybridStockfishFactory.hpp"

#include "HybridStockfishEngine.hpp"

namespace margelo::nitro::stockfish {

std::shared_ptr<Promise<std::shared_ptr<HybridStockfishEngineSpec>>>
HybridStockfishFactory::createEngine(const std::optional<EngineOptions>& options) {
  return Promise<std::shared_ptr<HybridStockfishEngineSpec>>::async(
      [options]() -> std::shared_ptr<HybridStockfishEngineSpec> {
        auto engine = std::make_shared<HybridStockfishEngine>(options.value_or(EngineOptions{}));
        return std::static_pointer_cast<HybridStockfishEngineSpec>(std::move(engine));
      });
}

} // namespace margelo::nitro::stockfish
