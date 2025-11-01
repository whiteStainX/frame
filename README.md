# frame

A simple command-line tool for displaying and animating ASCII art in the terminal, built with C++ and Notcurses.

## Features

- **Static Display:** Renders a single ASCII art file, centered in the terminal.
- **Interpolation:** Smoothly transitions between a start and an end frame over a configurable number of steps.
- **Sequence Animation:** Plays a series of frames in order, like a traditional flip-book animation.
- **Looping:** Supports optional looping for both interpolation and sequence animations.
- **Flexible Configuration:** Control all features via a `frame.toml` configuration file and/or command-line flags.

## Dependencies

- **Notcurses:** A library for creating terminal user interfaces.
- **CMake** (version 3.16 or higher)
- A C++20 compatible compiler (e.g., GCC, Clang, MSVC)

## Building

To build the project, run the following commands from the project root:

```bash
# Configure the project
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Build the executable
cmake --build build
```

The final executable will be located at `./build/frame`.

## Usage

The application can be controlled via command-line flags, which will override any settings in `frame.toml`.

```bash
# Display the help message to see all options
./build/frame --help

# Display a single static frame
./build/frame --mode static -f assets/test/start.txt

# Animate a transition between two frames and loop
./build/frame --mode sequence -f assets/test/start.txt assets/test/end.txt --loop

# Animate a sequence: 1 -> 2 -> 1. Pause 500ms between transitions.
./build/frame --mode sequence -f assets/test/start.txt assets/test/end.txt assets/test/start.txt --pause 500

# Create a "flip-book" style animation with instant transitions
./build/frame --mode sequence -f assets/test/start.txt assets/test/end.txt --steps 0 --pause 0
```

## Configuration File

You can define the default behavior in the `frame.toml` file. The `sequence` and `interpolate` modes now use the same powerful animation engine.

```toml
# Animation mode: "static" or "sequence"
mode = "sequence"

# List of frame files to use in the animation.
# - For "static" mode, only the first frame is used.
# - For "sequence" mode, the tool will animate transitions between
#   each adjacent pair of frames (e.g., 1->2, 2->3, etc.).
frames = ["assets/test/start.txt", "assets/test/end.txt", "assets/test/start.txt"]

# Number of interpolation steps for the dissolve effect between frames.
# Set to 0 for an instant, "flip-book" style transition.
steps = 30

# Frame rate in frames per second for animations.
rate = 24

# Pause in milliseconds between animation segments (e.g., after 1->2 finishes
# and before 2->3 starts).
pause_ms = 500

# Whether the entire animation sequence should loop.
loop = true
```