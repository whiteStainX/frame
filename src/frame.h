#ifndef FRAME_FRAME_H
#define FRAME_FRAME_H

#include <string>
#include <vector>

class Frame {
public:
    // Default constructor
    Frame();

    // Constructor that loads a frame from a file path
    explicit Frame(const std::string& path);

    // Constructor for creating a frame from raw pixel data in memory
    explicit Frame(std::vector<std::string> new_pixels);

    // Load a frame from a file path. Returns false if loading fails.
    bool load_from_file(const std::string& path);

    // Getters
    int get_width() const;
    int get_height() const;
    const std::vector<std::string>& get_pixels() const;

private:
    std::vector<std::string> pixels;
    int width;
    int height;
};

#endif //FRAME_FRAME_H
