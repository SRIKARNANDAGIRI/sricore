#include "srichakra/complexity.hpp"

namespace srichakra {

namespace {

[[nodiscard]] double clamp_score(double value) noexcept {
    if (value < 0.0) {
        return 0.0;
    }
    if (value > 1.0) {
        return 1.0;
    }
    return value;
}

}  // namespace

double ComplexityCalculator::calculate(const ComplexityBudget& budget) const noexcept {
    const double score = (static_cast<double>(budget.nesting_depth) * 0.3) +
                         (static_cast<double>(budget.dependency_count) * 0.2) +
                         (static_cast<double>(budget.output_schema_size) / 1000.0 * 0.2) +
                         (static_cast<double>(budget.token_estimate) / 10000.0 * 0.3);
    return clamp_score(score);
}

bool is_approaching_ceiling(const ComplexityBudget& budget) noexcept {
    static const ComplexityCalculator calculator;
    return calculator.calculate(budget) > kComplexityCeilingWarn;
}

bool should_reject(const ComplexityBudget& budget) noexcept {
    static const ComplexityCalculator calculator;
    return calculator.calculate(budget) > kComplexityCeilingReject;
}

}  // namespace srichakra
