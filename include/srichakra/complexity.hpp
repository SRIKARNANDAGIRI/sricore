#pragma once

#include "srichakra/constants.hpp"
#include "srichakra/types.hpp"

namespace srichakra {

class ComplexityCalculator {
public:
    [[nodiscard]] double calculate(const ComplexityBudget& budget) const noexcept;
};

[[nodiscard]] bool is_approaching_ceiling(const ComplexityBudget& budget) noexcept;
[[nodiscard]] bool should_reject(const ComplexityBudget& budget) noexcept;

}  // namespace srichakra
