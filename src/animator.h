#ifndef FRAME_ANIMATOR_H
#define FRAME_ANIMATOR_H

#include "frame.h"
#include <vector>
#include <string>

struct Config;

class Animator {
public:
    explicit Animator(const Config& config);

    // Stateless method to generate an interpolated frame.
    Frame generate_interpolated_frame(const Frame& start, const Frame& end, int step, int total_steps) const;

    // Accessors for the loaded frames.
    const Frame& get_frame(int index) const;
    int get_frame_count() const;

private:
    std::vector<Frame> frames;
};

#endif //FRAME_ANIMATOR_H
