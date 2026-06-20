#pragma once

#include "srichakra/constants.hpp"

#include <array>
#include <atomic>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>

namespace srichakra {

class Goal {
public:
    Goal(std::string input, std::uint64_t id, std::uint64_t timestamp_ns)
        : input_(clamp_input(std::move(input))), id_(id), timestamp_ns_(timestamp_ns) {}

    [[nodiscard]] const std::string& input() const noexcept { return input_; }
    [[nodiscard]] std::uint64_t id() const noexcept { return id_; }
    [[nodiscard]] std::uint64_t timestamp_ns() const noexcept { return timestamp_ns_; }

private:
    static std::string clamp_input(std::string input) {
        if (input.size() > kMaxGoalStringLength) {
            input.resize(kMaxGoalStringLength);
        }
        return input;
    }

    const std::string input_;
    const std::uint64_t id_;
    const std::uint64_t timestamp_ns_;
};

struct LayerResult {
    std::uint8_t layer_index{};
    bool passed{};
    bool rejected{};
    std::string_view rejection_reason{};
    std::uint64_t processing_ns{};
};

enum class AvaranaStatus : std::uint8_t {
    Pending,
    Running,
    Passed,
    Rejected,
    Error,
};

struct alignas(64) AvaranaState {
    std::atomic<std::uint8_t> current_layer{0};
    std::uint8_t retry_count{0};
    AvaranaStatus status{AvaranaStatus::Pending};
};

static_assert(sizeof(AvaranaState) <= 64);

struct ComplexityBudget {
    std::uint8_t nesting_depth{};
    std::uint16_t dependency_count{};
    std::uint16_t output_schema_size{};
    std::uint32_t token_estimate{};
    double score{};
};

struct SkillSignature {
    std::string_view name{};
    std::uint64_t hash{};
    std::uint32_t version{};
};

struct BinduOutput {
    std::uint64_t goal_id{};
    std::string result;
    std::array<LayerResult, kLayerCount> layer_results{};
    std::uint64_t total_ns{};
    bool success{};
};

}  // namespace srichakra
