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
./build/frame --mode static --frames assets/test/start.txt

# Interpolate between two frames over 30 steps, with a frame rate of 24fps, and loop
./build/frame --mode interpolate --steps 30 --rate 24 --loop --frames assets/test/start.txt assets/test/end.txt

# Play a sequence of frames
./build/frame --mode sequence --frames assets/test/frame1.txt assets/test/frame2.txt assets/test/frame3.txt
```

## Configuration File

You can define the default behavior in the `frame.toml` file.

```toml
# Animation mode: "static", "interpolate", or "sequence"
mode = "static"

# List of frame files to use.
# For "static" mode, only the first one is used.
# For "interpolate" mode, the first two are used (start and end).
# For "sequence" mode, all are used in order.
frames = ["assets/test/start.txt"]

# Number of interpolation steps between frames.
# 1 means no interpolation (direct transition).
steps = 10

# Frame rate in frames per second.
rate = 12

# Whether the animation should loop.
loop = false
```