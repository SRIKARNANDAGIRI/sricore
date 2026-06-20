#include "srichakra/constants.hpp"
#include "srichakra/engine.hpp"

#include <cstdint>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    srichakra::Engine engine;

    std::string goal_input = "analyze this task";
    if (argc > 1 && argv[1] != nullptr) {
        goal_input = argv[1];
    }

    const srichakra::BinduOutput output = engine.process(goal_input);

    std::uint8_t layers_passed = 0;
    for (const srichakra::LayerResult& layer : output.layer_results) {
        if (layer.passed) {
            ++layers_passed;
        }
    }

    const std::uint64_t total_us = output.total_ns / 1000;

    std::cout << "success: " << (output.success ? "true" : "false") << '\n';
    std::cout << "layers passed: " << static_cast<int>(layers_passed) << '/'
              << static_cast<int>(srichakra::kLayerCount) << '\n';
    std::cout << "total time (us): " << total_us << '\n';
    std::cout << "result: " << output.result << '\n';

    return output.success ? 0 : 1;
}
