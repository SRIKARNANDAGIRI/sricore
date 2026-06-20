#pragma once

#include "srichakra/complexity.hpp"
#include "srichakra/constants.hpp"
#include "srichakra/types.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <string_view>

namespace srichakra {

class StateMachine {
public:
    void start(const Goal& goal) noexcept {
        (void)goal;
        results_ = {};
        state_.current_layer.store(0, std::memory_order_release);
        state_.retry_count = 0;
        state_.status = AvaranaStatus::Running;
    }

    LayerResult process_layer(std::uint8_t layer_index, const Goal& goal) noexcept {
        const auto started = std::chrono::steady_clock::now();

        LayerResult result{};
        result.layer_index = layer_index;

        if (state_.status == AvaranaStatus::Error) {
            result.rejected = true;
            result.rejection_reason = kReasonRetryLimit;
            result.processing_ns = elapsed_ns(started);
            return result;
        }

        if (layer_index >= kLayerCount) {
            result.rejected = true;
            result.rejection_reason = kReasonInvalidLayer;
            result.processing_ns = elapsed_ns(started);
            return result;
        }

        if (layer_index != state_.current_layer.load(std::memory_order_acquire)) {
            result.rejected = true;
            result.rejection_reason = kReasonOutOfSequence;
            result.processing_ns = elapsed_ns(started);
            return result;
        }

        if (state_.retry_count > kMaxRetriesPerLayer) {
            state_.status = AvaranaStatus::Error;
            result.rejected = true;
            result.rejection_reason = kReasonRetryLimit;
            result.processing_ns = elapsed_ns(started);
            results_[layer_index] = result;
            return result;
        }

        const ComplexityBudget budget = derive_budget(goal);
        if (should_reject(budget)) {
            ++state_.retry_count;
            result.rejected = true;
            result.rejection_reason = kReasonComplexity;
            result.processing_ns = elapsed_ns(started);
            results_[layer_index] = result;

            if (state_.retry_count > kMaxRetriesPerLayer) {
                state_.status = AvaranaStatus::Error;
                result.rejection_reason = kReasonRetryLimit;
                results_[layer_index] = result;
            } else {
                state_.status = AvaranaStatus::Rejected;
            }
            return result;
        }

        result.passed = true;
        result.rejected = false;
        result.rejection_reason = {};
        result.processing_ns = elapsed_ns(started);
        results_[layer_index] = result;

        state_.retry_count = 0;
        state_.status = AvaranaStatus::Running;

        const std::uint8_t next_layer = static_cast<std::uint8_t>(layer_index + 1);
        state_.current_layer.store(next_layer, std::memory_order_release);

        if (next_layer >= kLayerCount) {
            state_.status = AvaranaStatus::Passed;
        }

        return result;
    }

    [[nodiscard]] bool can_advance() const noexcept {
        if (state_.status != AvaranaStatus::Running && state_.status != AvaranaStatus::Rejected) {
            return false;
        }

        const std::uint8_t layer = state_.current_layer.load(std::memory_order_acquire);
        if (layer >= kLayerCount) {
            return false;
        }

        if (layer == 0) {
            return true;
        }

        return results_[layer - 1].passed;
    }

    [[nodiscard]] bool is_complete() const noexcept {
        return state_.status == AvaranaStatus::Passed;
    }

    [[nodiscard]] bool is_rejected() const noexcept {
        return state_.status == AvaranaStatus::Rejected;
    }

    [[nodiscard]] const AvaranaState& state() const noexcept { return state_; }

    [[nodiscard]] const LayerResult& result_at(std::uint8_t index) const noexcept {
        return index < kLayerCount ? results_[index] : empty_result_;
    }

private:
    static constexpr std::string_view kReasonComplexity = "complexity ceiling exceeded";
    static constexpr std::string_view kReasonRetryLimit = "retry limit exceeded";
    static constexpr std::string_view kReasonOutOfSequence = "out of sequence";
    static constexpr std::string_view kReasonInvalidLayer = "invalid layer index";

    [[nodiscard]] static std::uint64_t elapsed_ns(
        const std::chrono::steady_clock::time_point& started) noexcept {
        const auto ended = std::chrono::steady_clock::now();
        return static_cast<std::uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(ended - started).count());
    }

    [[nodiscard]] ComplexityBudget derive_budget(const Goal& goal) const noexcept {
        ComplexityBudget budget{};
        const std::string_view input{goal.input()};

        std::uint8_t depth = 0;
        std::uint8_t max_depth = 0;
        std::uint16_t dependencies = 0;

        for (const char ch : input) {
            if (ch == '{' || ch == '[' || ch == '(') {
                ++depth;
                max_depth = std::max(max_depth, depth);
            } else if (ch == '}' || ch == ']' || ch == ')') {
                if (depth > 0) {
                    --depth;
                }
            } else if (ch == ',' || ch == ';') {
                ++dependencies;
            }
        }

        const std::size_t length = input.size();
        budget.nesting_depth = max_depth;
        budget.dependency_count = dependencies;
        budget.output_schema_size =
            static_cast<std::uint16_t>(std::min(length, static_cast<std::size_t>(65535)));
        budget.token_estimate =
            static_cast<std::uint32_t>(std::min(length, static_cast<std::size_t>(4294967295U)));
        budget.score = calculator_.calculate(budget);
        return budget;
    }

    ComplexityCalculator calculator_{};
    AvaranaState state_{};
    std::array<LayerResult, kLayerCount> results_{};
    static inline const LayerResult empty_result_{};
};

}  // namespace srichakra
