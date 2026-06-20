#include "srichakra/engine.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>

namespace srichakra {

namespace {

std::atomic<std::uint64_t> g_next_goal_id{1};

[[nodiscard]] std::uint64_t now_ns() noexcept {
    return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
                                          std::chrono::steady_clock::now().time_since_epoch())
                                          .count());
}

[[nodiscard]] std::uint64_t elapsed_ns(
    const std::chrono::steady_clock::time_point& started) noexcept {
    const auto ended = std::chrono::steady_clock::now();
    return static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(ended - started).count());
}

}  // namespace

ComplexityBudget Engine::budget_from_length(const std::size_t length) noexcept {
    ComplexityBudget budget{};
    budget.nesting_depth =
        static_cast<std::uint8_t>(std::min(length / 512, static_cast<std::size_t>(255)));
    budget.dependency_count =
        static_cast<std::uint16_t>(std::min(length / 64, static_cast<std::size_t>(65535)));
    budget.output_schema_size =
        static_cast<std::uint16_t>(std::min(length, static_cast<std::size_t>(65535)));
    budget.token_estimate =
        static_cast<std::uint32_t>(std::min(length, static_cast<std::size_t>(4294967295U)));
    return budget;
}

BinduOutput Engine::process(const std::string& goal_input) noexcept {
    const auto started = std::chrono::steady_clock::now();

    const std::uint64_t goal_id = g_next_goal_id.fetch_add(1, std::memory_order_relaxed);
    const Goal goal(goal_input, goal_id, now_ns());

    state_machine_.start(goal);

    std::uint8_t layer = 0;
    while (layer < kLayerCount) {
        const LayerResult result = state_machine_.process_layer(layer, goal);

        if (result.passed) {
            ++layer;
            continue;
        }

        if (state_machine_.state().status == AvaranaStatus::Error) {
            break;
        }

        if (state_machine_.is_rejected()) {
            continue;
        }

        break;
    }

    last_output_.goal_id = goal.id();
    last_output_.success = state_machine_.is_complete();

    for (std::uint8_t i = 0; i < kLayerCount; ++i) {
        last_output_.layer_results[i] = state_machine_.result_at(i);
    }

    last_output_.total_ns = elapsed_ns(started);

    if (last_output_.success) {
        last_output_.result = goal.input();
    } else {
        last_output_.result.clear();
    }

    return last_output_;
}

AvaranaStatus Engine::layer_status(const std::uint8_t layer_index) const noexcept {
    if (layer_index >= kLayerCount) {
        return AvaranaStatus::Error;
    }

    const AvaranaState& machine_state = state_machine_.state();
    const LayerResult& layer_result = state_machine_.result_at(layer_index);
    const std::uint8_t current_layer =
        machine_state.current_layer.load(std::memory_order_acquire);

    if (layer_result.passed) {
        return AvaranaStatus::Passed;
    }

    if (layer_result.rejected) {
        if (machine_state.status == AvaranaStatus::Error && layer_index == current_layer) {
            return AvaranaStatus::Error;
        }
        return AvaranaStatus::Rejected;
    }

    if (layer_index == current_layer && machine_state.status == AvaranaStatus::Running) {
        return AvaranaStatus::Running;
    }

    if (layer_index > current_layer) {
        return AvaranaStatus::Pending;
    }

    return AvaranaStatus::Pending;
}

double Engine::complexity(const std::string& input) const noexcept {
    const ComplexityBudget budget = budget_from_length(input.size());
    return calculator_.calculate(budget);
}

bool Engine::validate(const std::uint8_t layer_index, const LayerResult& result) const noexcept {
    return result.passed && result.layer_index == layer_index;
}

const BinduOutput& Engine::last_output() const noexcept {
    return last_output_;
}

}  // namespace srichakra
