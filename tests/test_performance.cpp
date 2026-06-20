#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include "srichakra/complexity.hpp"

namespace {

srichakra::ComplexityBudget make_budget(const std::uint32_t token_estimate) {
    srichakra::ComplexityBudget budget{};
    budget.nesting_depth = 4;
    budget.dependency_count = 12;
    budget.output_schema_size = 256;
    budget.token_estimate = token_estimate;
    return budget;
}

}  // namespace

TEST_CASE("ComplexityCalculator performance", "[performance]") {
    const srichakra::ComplexityCalculator calculator;
    const srichakra::ComplexityBudget budget = make_budget(10'000);

    BENCHMARK("calculate complexity score") { return calculator.calculate(budget); };

    const double score = calculator.calculate(budget);
    REQUIRE(score >= 0.0);
    REQUIRE(score <= 1.0);
}
