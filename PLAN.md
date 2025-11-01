# Project Plan: Frame

This document outlines the development plan for the `frame` project, a command-line ASCII art animation tool.

## Phase 1: Project Setup & Configuration (Completed)

**Objective:** Establish a solid foundation for the project, including build system, dependency management, and configuration loading.

- **Build System:** Configure `CMakeLists.txt` for the C++20 project.
- **Dependencies:** Integrate `cxxopts` (for command-line flags) and `toml++` (for `frame.toml` configuration) from the `external/` directory.
- **Configuration:** Implement a `Config` struct and a `parse_config()` function in `src/config.h` to unify settings from both the command line and the TOML file. Command-line flags override file settings.
- **Verification:** Update `main.cpp` to parse and print the loaded configuration to confirm the system works as expected.

---

## Phase 2: Core Components & Static Display (Completed)

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

## Phase 3: Animation Engine (Completed)

**Objective:** Implement a flexible animation system that supports multi-frame sequences with interpolation and pauses.

1.  **Configuration (`config.h`, `frame.toml`):**
    *   Added a `pause_ms` option to specify a pause duration between animation segments.

2.  **`Animator` Class (`src/animator.h`, `src/animator.cpp`):**
    *   Refactored into a stateless utility.
    *   Loads all frames specified in the configuration.
    *   Provides a `generate_interpolated_frame` method that can create a transition frame between any two given frames at a specific progress point.
    *   The interpolation logic was updated to be UTF-8 aware using standard C++ libraries (`<cstdlib>`, `<clocale>`) and to use a "dissolve" effect for better visuals.

3.  **Main Logic (`src/main.cpp`):**
    *   Re-architected the main loop to handle sequences of interpolations.
    *   The loop now iterates through adjacent pairs of frames (1->2, 2->3, etc.).
    *   For each pair, it performs a full interpolation.
    *   After each interpolation, it respects the `pause_ms` setting.
    *   Handles `loop = false` correctly by holding the final frame until the user quits.

4.  **Test Assets:**
    *   Created an `assets/test/end.txt` file to enable testing of a 2-frame animation.
    *   Updated `frame.toml` to default to a 3-frame sequence to demonstrate the new capabilities.

---

## Phase 4: Refinements & Review

**Objective:** Finalize the application, clean up the codebase, and perform final testing.

- Review all code for clarity, consistency, and adherence to C++ best practices.
- Add comments to complex sections of the code, particularly the interpolation logic.
- Thoroughly test all three modes (`static`, `interpolate`, `sequence`) with various command-line flags and `frame.toml` configurations to ensure everything works as specified.
