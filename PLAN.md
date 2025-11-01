# Project Plan: Frame

This document outlines the development plan for the `frame` project, a command-line ASCII art animation tool.

## Phase 1: Project Setup & Configuration (Completed)

**Objective:** Establish a solid foundation for the project, including build system, dependency management, and configuration loading.

- **Build System:** Configure `CMakeLists.txt` for the C++20 project.
- **Dependencies:** Integrate `cxxopts` (for command-line flags) and `toml++` (for `frame.toml` configuration) from the `external/` directory.
- **Configuration:** Implement a `Config` struct and a `parse_config()` function in `src/config.h` to unify settings from both the command line and the TOML file. Command-line flags override file settings.
- **Verification:** Update `main.cpp` to parse and print the loaded configuration to confirm the system works as expected.

---

## Phase 2: Core Components & Static Display

**Objective:** Implement the core classes and fulfill the first requirement: displaying a single, static ASCII art frame, centered in the terminal.

1.  **`Frame` Class (`src/frame.h`, `src/frame.cpp`):**
    *   **Purpose:** A data class to load and hold a single ASCII art frame.
    *   **Details:** It will read a specified text file, storing the content in a `std::vector<std::string>` and calculating the frame's `width` and `height`.

2.  **`Renderer` Class (`src/renderer.h`, `src/renderer.cpp`):**
    *   **Purpose:** To encapsulate all `notcurses` library interactions, separating rendering logic from the main application logic.
    *   **Details:** The class will handle `notcurses` initialization and shutdown. It will have a primary method, `draw_frame(const Frame&)`, which will create a centered `ncplane` and render the provided frame's content onto it.

3.  **Test Assets:**
    *   Create an initial test asset at `assets/test/start.txt` with simple ASCII art.

4.  **Main Logic (`src/main.cpp`):**
    *   Implement the logic for the `static` mode.
    *   The program will load the frame specified in the configuration, initialize the `Renderer`, draw the frame, and wait for a key press before exiting.

5.  **Build System (`CMakeLists.txt`):**
    *   Update the `add_executable` command to include the new `frame.cpp` and `renderer.cpp` source files.

---

## Phase 3: Animation Engine

**Objective:** Implement the animation and interpolation features, fulfilling requirements #2, #3, and #4.

1.  **`Animator` Class (`src/animator.h`, `src/animator.cpp`):**
    *   **Purpose:** To contain the complex logic for generating frames for both interpolation and sequence animations.
    *   **Interpolation Logic:** The `Animator` will be initialized with a start `Frame`, an end `Frame`, and the number of steps. A method, `get_interpolated_frame(int step)`, will calculate and return a *new, temporary `Frame` object* for that point in the transition.
    *   **Sequence Logic:** For simple frame-by-frame animations, the `Animator` will manage a list of `Frame`s and provide the next one in the sequence when asked.

2.  **Main Logic (`src/main.cpp`):**
    *   Implement the main animation loop.
    *   Add logic to handle the `interpolate` and `sequence` modes.
    *   The loop will be responsible for:
        1.  Requesting the correct frame for the current time from the `Animator`.
        2.  Passing that frame to the `Renderer` to be drawn.
        3.  Sleeping for a duration calculated from the `frame_rate` setting.
        4.  Handling the `loop` setting.

3.  **Test Assets:**
    *   Create an `assets/test/end.txt` and potentially other files to test multi-frame animations.

---

## Phase 4: Refinements & Review

**Objective:** Finalize the application, clean up the codebase, and perform final testing.

- Review all code for clarity, consistency, and adherence to C++ best practices.
- Add comments to complex sections of the code, particularly the interpolation logic.
- Thoroughly test all three modes (`static`, `interpolate`, `sequence`) with various command-line flags and `frame.toml` configurations to ensure everything works as specified.
