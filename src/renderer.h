#ifndef FRAME_RENDERER_H
#define FRAME_RENDERER_H

#include <notcurses/notcurses.h>

// Forward-declare the Frame class to avoid including the full frame.h here.
// This is a good practice to reduce compilation times.
class Frame;

class Renderer {
public:
    // Constructor: Initializes the Notcurses environment.
    Renderer();

    // Destructor: Shuts down the Notcurses environment.
    ~Renderer();

    // Clears the entire screen.
    void clear_screen();

    // Draws a single frame, centered on the screen.
    void draw_frame(const Frame& frame);
    
    // Waits until the user presses the Notcurses quit key ('q').
    void wait_for_quit();

    // Checks for 'q' or 'Q' input without blocking. Returns true if quit was requested.
    bool check_for_quit();

private:
    struct notcurses* nc;
    struct ncplane* stdplane;
    struct ncplane* frame_plane;
};

#endif //FRAME_RENDERER_H
