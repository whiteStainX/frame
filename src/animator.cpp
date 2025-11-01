#include "animator.h"
#include "config.h"
#include <stdexcept>
#include <algorithm>
#include <cmath>

Animator::Animator(const Config& config) {
    mode = config.mode;
    interpolation_steps = config.steps;

    if (config.frame_paths.empty()) {
        throw std::runtime_error("Animator requires at least one frame path.");
    }

    for (const auto& path : config.frame_paths) {
        frames.emplace_back(path);
    }

    if (mode == "interpolate" && frames.size() < 2) {
        throw std::runtime_error("Interpolation mode requires at least two frames.");
    }
}

int Animator::get_total_steps() const {
    if (mode == "sequence") {
        return frames.size();
    }
    if (mode == "interpolate") {
        // The number of steps is the number of intermediate frames.
        // Total frames in animation will be steps + 1.
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

    // Determine dimensions of the new frame (use the larger of the two)
    int height = std::max(start_frame.get_height(), end_frame.get_height());
    int width = std::max(start_frame.get_width(), end_frame.get_width());

    std::vector<std::string> new_pixels;
    new_pixels.reserve(height);

    for (int y = 0; y < height; ++y) {
        // Get corresponding lines, or an empty string if one frame is shorter
        const std::string& start_line = (y < start_pixels.size()) ? start_pixels[y] : "";
        const std::string& end_line = (y < end_pixels.size()) ? end_pixels[y] : "";

        // Pad the lines to the max width before interpolating to ensure safe access
        std::string padded_start = start_line;
        padded_start.resize(width, ' ');
        std::string padded_end = end_line;
        padded_end.resize(width, ' ');

        std::string new_line;
        new_line.reserve(width);
        for (int x = 0; x < width; ++x) {
            new_line += interpolate_char(padded_start[x], padded_end[x], progress);
        }
        new_pixels.push_back(new_line);
    }

    return Frame(new_pixels);
}

char Animator::interpolate_char(char start, char end, float progress) const {
    // Only interpolate printable ASCII characters. For anything else (like box-drawing chars or spaces),
    // just switch from start to end when progress passes the halfway mark. This prevents
    // generating weird non-printable characters during the transition.
    if (isprint(start) && isprint(end)) {
        return static_cast<char>(start + (end - start) * progress);
    } else {
        return (progress < 0.5f) ? start : end;
    }
}
