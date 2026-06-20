#pragma once

#include <array>
#include <cstddef>
#include <string_view>

namespace srichakra {

inline constexpr std::size_t kLayerCount = 9;

inline constexpr std::array<std::string_view, kLayerCount> kLayerNames = {
    "Bhupura",
    "16-petal",
    "8-petal",
    "14-triangle",
    "Outer10",
    "Inner10",
    "8-triangle",
    "Upward",
    "Downward",
};

inline constexpr std::string_view kBinduStageName = "Bindu";

inline constexpr double kComplexityCeilingWarn = 0.7;
inline constexpr double kComplexityCeilingReject = 0.9;

inline constexpr std::size_t kMaxRetriesPerLayer = 3;

inline constexpr std::size_t kMemoryPoolSizeBytes = 1024 * 1024;

inline constexpr std::size_t kMaxGoalStringLength = 4096;

static_assert(kLayerNames.size() == kLayerCount);
static_assert(kComplexityCeilingWarn < kComplexityCeilingReject);
static_assert(kComplexityCeilingReject <= 1.0);

}  // namespace srichakra
