#ifndef FRAME_ANIMATOR_H
#define FRAME_ANIMATOR_H

#include "frame.h"
#include <vector>
#include <string>

// Forward-declare Config to avoid circular dependencies if Config needs the Animator later.
struct Config;

class Animator {
public:
    explicit Animator(const Config& config);

    // Returns a frame for a given step in the animation.
    // For "sequence" mode, step is the frame index.
    // For "interpolate" mode, step is the interpolation step.
    Frame get_frame_at(int step) const;

    // Gets the total number of frames or steps in the animation.
    int get_total_steps() const;

private:
    std::string mode;
    std::vector<Frame> frames;
    int interpolation_steps;

    // Generates an interpolated frame based on the step.
    Frame generate_interpolated_frame(int step) const;

    // Helper function to interpolate a single character/glyph.
    char32_t interpolate_char(char32_t start, char32_t end, float progress) const;
};

#endif //FRAME_ANIMATOR_H
