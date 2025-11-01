#include "config.h"
#include "frame.h"
#include "renderer.h"
#include "animator.h"
#include <iostream>
#include <thread>
#include <chrono>

int main(int argc, char** argv) {
    Config config = parse_config(argc, argv);

    if (config.mode == "static") {
        if (config.frame_paths.empty()) {
            std::cerr << "Error: Static mode requires a frame file path." << std::endl;
            return 1;
        }
        Frame frame(config.frame_paths[0]);
        if (frame.get_height() == 0) {
            return 1;
        }
        Renderer renderer;
        renderer.clear_screen();
        renderer.draw_frame(frame);
        renderer.wait_for_quit();
    } else if (config.mode == "sequence" || config.mode == "interpolate") {
        try {
            Animator animator(config);
            Renderer renderer;
            renderer.clear_screen();

            auto frame_duration = std::chrono::milliseconds(1000 / config.frame_rate);
            bool quit_requested = false;

            do { // This loop handles the 'loop' config option
                int total_steps = animator.get_total_steps();
                for (int i = 0; i < total_steps; ++i) {
                    auto start_time = std::chrono::steady_clock::now();

                    Frame current_frame = animator.get_frame_at(i);
                    renderer.draw_frame(current_frame);

                    if (renderer.check_for_quit()) {
                        quit_requested = true;
                        break;
                    }

                    auto end_time = std::chrono::steady_clock::now();
                    auto render_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
                    auto sleep_time = frame_duration - render_time;

                    if (sleep_time > std::chrono::milliseconds(0)) {
                        std::this_thread::sleep_for(sleep_time);
                    }
                }
                if (quit_requested) {
                    break;
                }
            } while (config.loop);

        } catch (const std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Error: Unknown mode '" << config.mode << "'" << std::endl;
        return 1;
    }

    return 0;
}
