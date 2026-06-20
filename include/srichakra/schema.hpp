#pragma once

#include <nlohmann/json.hpp>

#include "srichakra/types.hpp"

namespace srichakra {

nlohmann::json graph_to_json(const Graph& graph);
Graph graph_from_json(const nlohmann::json& j);

}  // namespace srichakra
