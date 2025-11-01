#include "frame.h"
#include <fstream>
#include <iostream>

Frame::Frame() : width(0), height(0) {}

Frame::Frame(const std::string& path) : width(0), height(0) {
    if (!load_from_file(path)) {
        std::cerr << "Error: Could not load frame from file: " << path << std::endl;
    }
}

bool Frame::load_from_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    pixels.clear();
    std::string line;
    width = 0;
    while (std::getline(file, line)) {
        pixels.push_back(line);
        if (line.length() > static_cast<size_t>(width)) {
            width = line.length();
        }
    }

    height = pixels.size();

    // Pad shorter lines with spaces to make the frame rectangular
    for (auto& p_line : pixels) {
        p_line.resize(width, ' ');
    }

    file.close();
    return true;
}

int Frame::get_width() const {
    return width;
}

int Frame::get_height() const {
    return height;
}

const std::vector<std::string>& Frame::get_pixels() const {
    return pixels;
}
