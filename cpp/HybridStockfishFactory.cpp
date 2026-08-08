#include "HybridStockfishFactory.hpp"

#include "HybridStockfishEngine.hpp"

#include <exception>
#include <stdexcept>
#include <string>

namespace margelo::nitro::stockfish {

std::shared_ptr<Promise<std::shared_ptr<HybridStockfishEngineSpec>>>
HybridStockfishFactory::createEngine(const std::optional<EngineOptions>& options) {
  return Promise<std::shared_ptr<HybridStockfishEngineSpec>>::async(
      [options]() -> std::shared_ptr<HybridStockfishEngineSpec> {
        try {
          auto engine = std::make_shared<HybridStockfishEngine>(options.value_or(EngineOptions{}));
          return std::static_pointer_cast<HybridStockfishEngineSpec>(std::move(engine));
        } catch (const std::exception& error) {
          throw std::runtime_error(
              std::string("Stockfish engine creation failed: ") + error.what());
        }
      });
}

} // namespace margelo::nitro::stockfish
