#include "renderer.h"
#include "frame.h" // The full definition of Frame is needed here
#include <cstdint>
#include <iostream>
#include <notcurses/notcurses.h>

Renderer::Renderer() : frame_plane(nullptr) {
    notcurses_options opts = {};
    // NCOPTION_SUPPRESS_BANNERS: Don't show Notcurses startup/shutdown messages.
    opts.flags = NCOPTION_SUPPRESS_BANNERS;

    nc = notcurses_init(&opts, NULL);
    if (nc == NULL) {
        std::cerr << "Error: Could not initialize Notcurses." << std::endl;
        exit(1);
    }
    stdplane = notcurses_stdplane(nc);
}

Renderer::~Renderer() {
    if (nc) {
        if (frame_plane) {
            ncplane_destroy(frame_plane);
        }
        notcurses_stop(nc);
    }
}

void Renderer::clear_screen() {
    if (frame_plane) {
        ncplane_destroy(frame_plane);
        frame_plane = nullptr;
    }
    ncplane_erase(stdplane);
    notcurses_render(nc);
}

void Renderer::draw_frame(const Frame& frame) {
    unsigned int term_dim_y, term_dim_x;
    notcurses_term_dim_yx(nc, &term_dim_y, &term_dim_x);

    int frame_height = frame.get_height();
    int frame_width = frame.get_width();

    if (frame_height == 0 || frame_width == 0) {
        return; // Don't attempt to draw an empty or unloaded frame
    }

    // Calculate vertical offset for centering the frame
    int pos_y = static_cast<int>((term_dim_y - frame_height) / 2);
    if (pos_y < 0) {
        pos_y = 0;
    }

    // Destroy any previous frame plane before creating a new one
    if (frame_plane) {
        ncplane_destroy(frame_plane);
        frame_plane = nullptr;
    }

    // Create a new plane that covers the entire screen
    ncplane_options nopts = {};
    nopts.y = 0;
    nopts.x = 0;
    nopts.rows = term_dim_y;
    nopts.cols = term_dim_x;

    frame_plane = ncplane_create(stdplane, &nopts);
    if (frame_plane == NULL) {
        return;
    }

    ncplane_erase(frame_plane);

    // Draw the frame's pixels (characters) to the plane
    const auto& pixels = frame.get_pixels();
    for (int y = 0; y < frame_height; ++y) {
        const std::string& line = pixels[y];
        int valid_bytes = 0;
        int valid_width = 0;
        int line_width = ncstrwidth(line.c_str(), &valid_bytes, &valid_width);
        if (line_width < 0) {
            line_width = valid_width;
        }
        int pos_x = static_cast<int>((term_dim_x - line_width) / 2);
        if (pos_x < 0) {
            pos_x = 0;
        }
        ncplane_putstr_yx(frame_plane, pos_y + y, pos_x, line.c_str());
    }

    // Render the virtual planes to the terminal
    notcurses_render(nc);
}

void Renderer::wait_for_quit() {
    while (true) {
        ncinput input;
        int32_t key = notcurses_get_blocking(nc, &input);
        if (key == 'q' || key == 'Q') {
            break;
        }
    }
}

bool Renderer::check_for_quit() {
    ncinput nci;
    // Don't block, just poll for input
    char32_t key = notcurses_get_nblock(nc, &nci);
    if (key == 'q' || key == 'Q') {
        return true;
    }
    return false;
}
