#include <catch2/catch_test_macros.hpp>

#include "srichakra/constants.hpp"
#include "srichakra/engine.hpp"
#include "srichakra/state_machine.hpp"

#include <cstdint>
#include <string>

namespace {

std::string make_nested_goal(const int depth, const int width) {
    std::string goal;
    goal.reserve(static_cast<std::size_t>(depth * 2 + width + 8));
    for (int i = 0; i < depth; ++i) {
        goal += '{';
    }
    goal += "payload";
    for (int i = 0; i < depth; ++i) {
        goal += '}';
    }
    goal.append(static_cast<std::size_t>(width), ',');
    return goal;
}

std::string make_extreme_goal() {
    std::string goal;
    goal.reserve(5000);
    for (int i = 0; i < 200; ++i) {
        goal += '(';
    }
    goal += "deeply nested goal payload";
    for (int i = 0; i < 200; ++i) {
        goal += ')';
    }
    goal.append(3000, ';');
    goal.append(1500, 'x');
    return goal;
}

}  // namespace

TEST_CASE("Goal stores input, id, and timestamp", "[goal]") {
    const srichakra::Goal goal("analyze this task", 42, 1'700'000'000'000ULL);

    REQUIRE(goal.input() == "analyze this task");
    REQUIRE(goal.id() == 42);
    REQUIRE(goal.timestamp_ns() == 1'700'000'000'000ULL);
}

TEST_CASE("StateMachine starts at layer 0", "[state_machine]") {
    srichakra::StateMachine machine;
    const srichakra::Goal goal("start here", 1, 100);

    machine.start(goal);

    REQUIRE(machine.state().current_layer.load() == 0);
    REQUIRE(machine.state().status == srichakra::AvaranaStatus::Running);
    REQUIRE(machine.state().retry_count == 0);
}

TEST_CASE("StateMachine passes all 9 layers for a simple goal", "[state_machine]") {
    srichakra::StateMachine machine;
    const srichakra::Goal goal("simple goal", 2, 200);

    machine.start(goal);

    for (std::uint8_t layer = 0; layer < srichakra::kLayerCount; ++layer) {
        const srichakra::LayerResult result = machine.process_layer(layer, goal);
        REQUIRE(result.passed);
        REQUIRE_FALSE(result.rejected);
        REQUIRE(result.layer_index == layer);
    }

    REQUIRE(machine.is_complete());
    REQUIRE(machine.state().current_layer.load() == srichakra::kLayerCount);

    for (std::uint8_t layer = 0; layer < srichakra::kLayerCount; ++layer) {
        REQUIRE(machine.result_at(layer).passed);
    }
}

TEST_CASE("StateMachine rejects a complex nested goal", "[state_machine]") {
    srichakra::StateMachine machine;
    const std::string nested_input = make_nested_goal(30, 500);
    const srichakra::Goal goal(nested_input, 3, 300);

    machine.start(goal);

    const srichakra::LayerResult result = machine.process_layer(0, goal);

    REQUIRE_FALSE(result.passed);
    REQUIRE(result.rejected);
    REQUIRE(result.layer_index == 0);
    REQUIRE(machine.is_rejected());
    REQUIRE_FALSE(machine.is_complete());
}

TEST_CASE("Engine::process succeeds on a simple goal", "[engine]") {
    srichakra::Engine engine;

    const srichakra::BinduOutput output = engine.process("analyze this task");

    REQUIRE(output.success);
    REQUIRE(output.result == "analyze this task");

    std::uint8_t passed_layers = 0;
    for (const srichakra::LayerResult& layer : output.layer_results) {
        if (layer.passed) {
            ++passed_layers;
        }
    }

    REQUIRE(passed_layers == srichakra::kLayerCount);
    REQUIRE(engine.last_output().success);
}

TEST_CASE("Engine::process fails on an extremely long deeply nested goal", "[engine]") {
    srichakra::Engine engine;

    const srichakra::BinduOutput output = engine.process(make_extreme_goal());

    REQUIRE_FALSE(output.success);
    REQUIRE(output.result.empty());

    std::uint8_t passed_layers = 0;
    for (const srichakra::LayerResult& layer : output.layer_results) {
        if (layer.passed) {
            ++passed_layers;
        }
    }

    REQUIRE(passed_layers < srichakra::kLayerCount);
    REQUIRE(engine.last_output().success == false);
}
