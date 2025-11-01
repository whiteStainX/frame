#include "animator.h"
#include "config.h"
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <cstdlib> // For mblen, srand, rand
#include <ctime>   // For time
#include <clocale> // For setlocale

Animator::Animator(const Config& config) {
    // Set the global C locale from the user's environment.
    // This is necessary for mblen to correctly interpret multi-byte UTF-8 characters.
    std::setlocale(LC_ALL, "");

    if (config.frame_paths.empty()) {
        throw std::runtime_error("Animator requires at least one frame path.");
    }

    for (const auto& path : config.frame_paths) {
        frames.emplace_back(path);
    }

    // Seed the random number generator once for the entire animation session.
    srand(time(NULL));
}

const Frame& Animator::get_frame(int index) const {
    if (index < 0 || index >= frames.size()) {
        throw std::out_of_range("Frame index out of range.");
    }
    return frames[index];
}

int Animator::get_frame_count() const {
    return frames.size();
}

// Generates a new frame by interpolating between two source frames.
// This function is stateless; it calculates the result based only on the inputs.
Frame Animator::generate_interpolated_frame(const Frame& start_frame, const Frame& end_frame, int step, int total_steps) const {
    // Progress is a value from 0.0 (fully start_frame) to 1.0 (fully end_frame).
    float progress = (total_steps == 0) ? 1.0f : static_cast<float>(step) / total_steps;

    const auto& start_pixels = start_frame.get_pixels();
    const auto& end_pixels = end_frame.get_pixels();

    // The new frame will have the maximum height of the two source frames.
    int height = std::max(start_frame.get_height(), end_frame.get_height());

    std::vector<std::string> new_pixels;
    new_pixels.reserve(height);

    for (int y = 0; y < height; ++y) {
        // Get the corresponding line from each frame. If one frame is shorter, use an empty string.
        const std::string& start_line = (y < start_pixels.size()) ? start_pixels[y] : "";
        const std::string& end_line = (y < end_pixels.size()) ? end_pixels[y] : "";

        std::string new_line;
        
        const char* p_start = start_line.c_str();
        const char* p_end = end_line.c_str();
        const char* const limit_start = p_start + start_line.length();
        const char* const limit_end = p_end + end_line.length();

        // Reset the multi-byte character state for each new line.
        mblen(NULL, 0);

        // This loop iterates through both lines simultaneously, character by character,
        // correctly handling multi-byte UTF-8 characters.
        while (p_start < limit_start || p_end < limit_end) {
            // Get the next full character (grapheme) from the start line.
            std::string char_start = " ";
            if (p_start < limit_start) {
                int len = mblen(p_start, limit_start - p_start);
                if (len > 0) {
                    char_start = std::string(p_start, len);
                    p_start += len;
                } else { // Invalid char or error, advance by 1 byte to avoid an infinite loop.
                    p_start++;
                }
            }

            // Get the next full character (grapheme) from the end line.
            std::string char_end = " ";
            if (p_end < limit_end) {
                int len = mblen(p_end, limit_end - p_end);
                if (len > 0) {
                    char_end = std::string(p_end, len);
                    p_end += len;
                } else { // Invalid char or error, advance by 1 byte.
                    p_end++;
                }
            }

            // Implements a "dissolve" effect. The probability of choosing the
            // end character increases with the animation's progress.
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