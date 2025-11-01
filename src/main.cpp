#include "config.h"
#include "frame.h"
#include "renderer.h"
#include "animator.h"
#include <iostream>
#include <thread>
#include <chrono>

void run_static_mode(const Config& config) {
    if (config.frame_paths.empty()) {
        std::cerr << "Error: Static mode requires a frame file path." << std::endl;
        exit(1);
    }
    Frame frame(config.frame_paths[0]);
    if (frame.get_height() == 0) {
        exit(1);
    }
    Renderer renderer;
    renderer.clear_screen();
    renderer.draw_frame(frame);
    renderer.wait_for_quit();
}

void run_animation_mode(const Config& config) {
    try {
        Animator animator(config);
        if (animator.get_frame_count() < 2) {
            std::cerr << "Error: Animation modes require at least two frames." << std::endl;
            exit(1);
        }

        Renderer renderer;
        renderer.clear_screen();

        auto frame_duration = std::chrono::milliseconds(1000 / config.frame_rate);
        auto pause_duration = std::chrono::milliseconds(config.pause_ms);
        bool quit_requested = false;

        do { // Main loop for the whole animation, handles the 'loop' config
            // Loop through pairs of frames (F1->F2, F2->F3, etc.)
            for (int i = 0; i < animator.get_frame_count() - 1; ++i) {
                const Frame& start_frame = animator.get_frame(i);
                const Frame& end_frame = animator.get_frame(i + 1);
                int total_interp_steps = config.steps;

                // The interpolation loop for a single segment
                for (int step = 0; step <= total_interp_steps; ++step) {
                    auto start_time = std::chrono::steady_clock::now();

                    Frame current_frame = animator.generate_interpolated_frame(start_frame, end_frame, step, total_interp_steps);
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
                } // End interpolation loop

                if (quit_requested) break;

                // Handle pause between interpolations, but only if not on the last frame and pause is configured
                if (i < animator.get_frame_count() - 2 && pause_duration > std::chrono::milliseconds(0)) {
                    std::this_thread::sleep_for(pause_duration);
                }

            } // End frame-pair loop

            if (quit_requested) break;

        } while (config.loop);

        // If the animation completes without being quit, wait for a final quit command.
        if (!quit_requested) {
            renderer.wait_for_quit();
        }

    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        exit(1);
    }
}

int main(int argc, char** argv) {
    Config config = parse_config(argc, argv);

    if (config.mode == "static") {
        run_static_mode(config);
    } else if (config.mode == "sequence" || config.mode == "interpolate") {
        run_animation_mode(config);
    } else {
        std::cerr << "Error: Unknown mode '" << config.mode << "'" << std::endl;
        return 1;
    }

    return 0;
}