#include "frame.h"
#include <fstream>
#include <iostream>
#include <notcurses/notcurses.h>

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
        int valid_bytes = 0;
        int valid_width = 0;
        int line_width = ncstrwidth(line.c_str(), &valid_bytes, &valid_width);
        if (line_width < 0) {
            line_width = valid_width;
        }
        if (line_width > width) {
            width = line_width;
        }
    }

    height = pixels.size();

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
