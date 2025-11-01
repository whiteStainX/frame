#include "animator.h"
#include "config.h"
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <cstdlib> // For mblen, srand, rand
#include <ctime>   // For time
#include <clocale> // For setlocale

Animator::Animator(const Config& config) {
    mode = config.mode;
    interpolation_steps = config.steps;

    // Set the locale to allow mblen to process UTF-8 characters correctly
    std::setlocale(LC_ALL, "");

    if (config.frame_paths.empty()) {
        throw std::runtime_error("Animator requires at least one frame path.");
    }

    for (const auto& path : config.frame_paths) {
        frames.emplace_back(path);
    }

    if (mode == "interpolate" && frames.size() < 2) {
        throw std::runtime_error("Interpolation mode requires at least two frames.");
    }

    // Seed the random number generator for the dissolve effect
    srand(time(NULL));
}

int Animator::get_total_steps() const {
    if (mode == "sequence") {
        return frames.size();
    }
    if (mode == "interpolate") {
        return interpolation_steps;
    }
    return 0;
}

Frame Animator::get_frame_at(int step) const {
    if (mode == "sequence") {
        if (step >= 0 && step < frames.size()) {
            return frames[step];
        }
    } else if (mode == "interpolate") {
        return generate_interpolated_frame(step);
    }
    return Frame(); // Return empty frame on error or for other modes
}

Frame Animator::generate_interpolated_frame(int step) const {
    if (frames.size() < 2 || step < 0 || step > interpolation_steps) {
        return Frame(); // Return empty frame
    }

    const Frame& start_frame = frames[0];
    const Frame& end_frame = frames[1];

    float progress = (interpolation_steps == 0) ? 1.0f : static_cast<float>(step) / interpolation_steps;

    const auto& start_pixels = start_frame.get_pixels();
    const auto& end_pixels = end_frame.get_pixels();

    int height = std::max(start_frame.get_height(), end_frame.get_height());

    std::vector<std::string> new_pixels;
    new_pixels.reserve(height);

    for (int y = 0; y < height; ++y) {
        const std::string& start_line = (y < start_pixels.size()) ? start_pixels[y] : "";
        const std::string& end_line = (y < end_pixels.size()) ? end_pixels[y] : "";

        std::string new_line;
        
        const char* p_start = start_line.c_str();
        const char* p_end = end_line.c_str();
        const char* const limit_start = p_start + start_line.length();
        const char* const limit_end = p_end + end_line.length();

        // Reset mblen state for each line
        mblen(NULL, 0);

        while (p_start < limit_start || p_end < limit_end) {
            std::string char_start = " ";
            if (p_start < limit_start) {
                int len = mblen(p_start, limit_start - p_start);
                if (len > 0) {
                    char_start = std::string(p_start, len);
                    p_start += len;
                } else { // Invalid char, advance by 1
                    p_start++;
                }
            }

            std::string char_end = " ";
            if (p_end < limit_end) {
                int len = mblen(p_end, limit_end - p_end);
                if (len > 0) {
                    char_end = std::string(p_end, len);
                    p_end += len;
                } else { // Invalid char, advance by 1
                    p_end++;
                }
            }

            // Dissolve effect
            float random_threshold = static_cast<float>(rand()) / RAND_MAX;
            if (random_threshold < progress) {
                new_line += char_end;
            } else {
                new_line += char_start;
            }
        }
        new_pixels.push_back(new_line);
    }

    return Frame(new_pixels);
}
