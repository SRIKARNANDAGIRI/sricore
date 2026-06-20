#pragma once

#include "srichakra/complexity.hpp"
#include "srichakra/constants.hpp"
#include "srichakra/state_machine.hpp"
#include "srichakra/types.hpp"

#include <cstdint>
#include <string>

namespace srichakra {

class Engine {
public:
    [[nodiscard]] BinduOutput process(const std::string& goal_input) noexcept;
    [[nodiscard]] AvaranaStatus layer_status(std::uint8_t layer_index) const noexcept;
    [[nodiscard]] double complexity(const std::string& input) const noexcept;
    [[nodiscard]] bool validate(std::uint8_t layer_index, const LayerResult& result) const noexcept;
    [[nodiscard]] const BinduOutput& last_output() const noexcept;

private:
    [[nodiscard]] static ComplexityBudget budget_from_length(std::size_t length) noexcept;

    ComplexityCalculator calculator_{};
    StateMachine state_machine_{};
    BinduOutput last_output_{};
};

}  // namespace srichakra
