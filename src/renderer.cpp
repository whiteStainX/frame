#include "renderer.h"
#include "frame.h" // The full definition of Frame is needed here
#include <iostream>

Renderer::Renderer() {
    notcurses_options opts = {};
    // NCOPTION_SUPPRESS_BANNERS: Don't show Notcurses startup/shutdown messages.
    // NCOPTION_NO_ALTERNATE_SCREEN: Draw on the main screen so the output persists after exit.
    opts.flags = NCOPTION_SUPPRESS_BANNERS | NCOPTION_NO_ALTERNATE_SCREEN;

    nc = notcurses_init(&opts, NULL);
    if (nc == NULL) {
        std::cerr << "Error: Could not initialize Notcurses." << std::endl;
        exit(1);
    }
    stdplane = notcurses_stdplane(nc);
}

Renderer::~Renderer() {
    if (nc) {
        notcurses_stop(nc);
    }
}

void Renderer::clear_screen() {
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

    // Calculate top-left position for centering the frame
    int pos_y = (term_dim_y - frame_height) / 2;
    int pos_x = (term_dim_x - frame_width) / 2;

    // Create a new plane for our frame content
    ncplane_options nopts = {};
    nopts.y = pos_y;
    nopts.x = pos_x;
    nopts.rows = frame_height;
    nopts.cols = frame_width;

    struct ncplane* frame_plane = ncplane_create(stdplane, &nopts);
    if (frame_plane == NULL) {
        return;
    }

    // Draw the frame's pixels (characters) to the plane
    const auto& pixels = frame.get_pixels();
    for (int y = 0; y < frame_height; ++y) {
        ncplane_putstr_yx(frame_plane, y, 0, pixels[y].c_str());
    }

    // Render the virtual planes to the terminal
    notcurses_render(nc);

    // Clean up the plane we created for this frame
    ncplane_destroy(frame_plane);
}

void Renderer::wait_for_input() {
    // Blocks until any character is read from stdin
    notcurses_get_nblock(nc, NULL);
}