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
    
    // Waits for any key press from the user.
    void wait_for_input();

private:
    struct notcurses* nc;
    struct ncplane* stdplane;
};

#endif //FRAME_RENDERER_H
