#include "animator.h"
#include "config.h"
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <clocale>

Animator::Animator(const Config& config) {
    std::setlocale(LC_ALL, "");

    if (config.frame_paths.empty()) {
        throw std::runtime_error("Animator requires at least one frame path.");
    }

    for (const auto& path : config.frame_paths) {
        frames.emplace_back(path);
    }

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

Frame Animator::generate_interpolated_frame(const Frame& start_frame, const Frame& end_frame, int step, int total_steps) const {
    float progress = (total_steps == 0) ? 1.0f : static_cast<float>(step) / total_steps;

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

        mblen(NULL, 0);

        while (p_start < limit_start || p_end < limit_end) {
            std::string char_start = " ";
            if (p_start < limit_start) {
                int len = mblen(p_start, limit_start - p_start);
                if (len > 0) {
                    char_start = std::string(p_start, len);
                    p_start += len;
                } else { p_start++; }
            }

            std::string char_end = " ";
            if (p_end < limit_end) {
                int len = mblen(p_end, limit_end - p_end);
                if (len > 0) {
                    char_end = std::string(p_end, len);
                    p_end += len;
                } else { p_end++; }
            }

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