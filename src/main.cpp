#include "config.h"
#include "frame.h"
#include "renderer.h"
#include <iostream>

int main(int argc, char** argv) {
    Config config = parse_config(argc, argv);

    if (config.mode == "static") {
        if (config.frame_paths.empty()) {
            std::cerr << "Error: Static mode requires a frame file path." << std::endl;
            std::cerr << "Please provide it in frame.toml or via the --frames flag." << std::endl;
            return 1;
        }

        Frame frame(config.frame_paths[0]);
        if (frame.get_height() == 0) {
            // The Frame constructor already prints an error if loading fails.
            return 1;
        }

        Renderer renderer;
        renderer.clear_screen();
        renderer.draw_frame(frame);
        renderer.wait_for_quit();

    } else {
        std::cout << "Mode '" << config.mode << "' is not yet implemented." << std::endl;
    }

    return 0;
}