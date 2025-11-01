#include "animator.h"
#include "config.h"
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <codecvt>
#include <locale>

namespace {

std::u32string utf8_to_u32(const std::string& input) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    try {
        return converter.from_bytes(input);
    } catch (const std::range_error&) {
        return std::u32string{};
    }
}

std::string u32_to_utf8(const std::u32string& input) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    try {
        return converter.to_bytes(input);
    } catch (const std::range_error&) {
        return std::string{};
    }
}

}

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
        // Include both the starting and ending frames in the timeline.
        return interpolation_steps + 1;
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

    int start_height = start_frame.get_height();
    int end_height = end_frame.get_height();
    int start_width = start_frame.get_width();
    int end_width = end_frame.get_width();

    // Determine dimensions of the new frame (use the larger of the two)
    int height = std::max(start_height, end_height);
    int width = std::max(start_width, end_width);

    int start_vertical_offset = (height - start_height) / 2;
    int end_vertical_offset = (height - end_height) / 2;

    std::vector<std::string> new_pixels;
    new_pixels.reserve(height);

    for (int y = 0; y < height; ++y) {
        int start_y = y - start_vertical_offset;
        int end_y = y - end_vertical_offset;

        const std::string& start_line = (start_y >= 0 && start_y < static_cast<int>(start_pixels.size()))
            ? start_pixels[start_y]
            : "";
        const std::string& end_line = (end_y >= 0 && end_y < static_cast<int>(end_pixels.size()))
            ? end_pixels[end_y]
            : "";

        // Convert UTF-8 strings into vectors of Unicode code points so we can
        // reason about glyph positions instead of raw bytes.
        auto start_glyphs = utf8_to_u32(start_line);
        auto end_glyphs = utf8_to_u32(end_line);

        // Fallback in case of conversion errors – operate directly on bytes.
        if (start_glyphs.empty() && !start_line.empty()) {
            start_glyphs.assign(start_line.begin(), start_line.end());
        }
        if (end_glyphs.empty() && !end_line.empty()) {
            end_glyphs.assign(end_line.begin(), end_line.end());
        }

        size_t glyph_width = static_cast<size_t>(
            std::max(width, static_cast<int>(std::max(start_glyphs.size(), end_glyphs.size()))));

        size_t start_horizontal_offset = 0;
        if (glyph_width > start_glyphs.size()) {
            start_horizontal_offset = (glyph_width - start_glyphs.size()) / 2;
        }

        size_t end_horizontal_offset = 0;
        if (glyph_width > end_glyphs.size()) {
            end_horizontal_offset = (glyph_width - end_glyphs.size()) / 2;
        }

        std::u32string new_line32;
        new_line32.reserve(glyph_width);

        for (size_t x = 0; x < glyph_width; ++x) {
            char32_t start_cp = U' ';
            if (x >= start_horizontal_offset &&
                x - start_horizontal_offset < start_glyphs.size()) {
                start_cp = start_glyphs[x - start_horizontal_offset];
            }

            char32_t end_cp = U' ';
            if (x >= end_horizontal_offset &&
                x - end_horizontal_offset < end_glyphs.size()) {
                end_cp = end_glyphs[x - end_horizontal_offset];
            }
            new_line32.push_back(interpolate_char(start_cp, end_cp, progress));
        }

        new_pixels.push_back(u32_to_utf8(new_line32));
    }

    return Frame(new_pixels);
}

char32_t Animator::interpolate_char(char32_t start, char32_t end, float progress) const {
    if (start == end) {
        return start;
    }

    auto is_basic_printable = [](char32_t codepoint) {
        return codepoint >= 0x20 && codepoint <= 0x7E;
    };

    if (is_basic_printable(start) && is_basic_printable(end)) {
        float interpolated = static_cast<float>(start) +
                             (static_cast<float>(end) - static_cast<float>(start)) * progress;
        return static_cast<char32_t>(std::round(interpolated));
    }

    return (progress < 0.5f) ? start : end;
}
