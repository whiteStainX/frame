#include <iostream>
#include "config.h"

int main(int argc, char** argv) {
    Config config = parse_config(argc, argv);

    std::cout << "--- Configuration ---" << std::endl;
    std::cout << "Mode: " << config.mode << std::endl;
    std::cout << "Steps: " << config.steps << std::endl;
    std::cout << "Frame Rate: " << config.frame_rate << std::endl;
    std::cout << "Loop: " << (config.loop ? "true" : "false") << std::endl;
    std::cout << "Config File: " << config.config_file << std::endl;
    std::cout << "Frame Paths: " << std::endl;
    if (config.frame_paths.empty()) {
        std::cout << "  (none)" << std::endl;
    } else {
        for (const auto& path : config.frame_paths) {
            std::cout << "  - " << path << std::endl;
        }
    }
    std::cout << "---------------------" << std::endl;

    return 0;
}
