#ifndef FRAME_CONFIG_H
#define FRAME_CONFIG_H

#include <string>
#include <vector>
#include <iostream>

#include "cxxopts.hpp"
#include "toml.hpp"

struct Config {
    std::string mode;
    std::vector<std::string> frame_paths;
    int steps = 1;
    int frame_rate = 10;
    int pause_ms = 0;
    int loop_pause_ms = 0;
    bool loop = false;
    std::string config_file = "frame.toml";
};

inline Config parse_config(int argc, char** argv) {
    Config config;
    // The default path is relative to the build directory, so we go up one level.
    // A better solution might be to pass the project source dir from cmake.
    std::string config_path_from_cli;

    cxxopts::Options options("frame", "A simple ASCII art animation tool");

    options.add_options()
        ("m,mode", "Animation mode (static, interpolate, sequence)", cxxopts::value<std::string>())
        ("f,frames", "Paths to frame files", cxxopts::value<std::vector<std::string>>())
        ("s,steps", "Interpolation steps", cxxopts::value<int>())
        ("r,rate", "Frame rate", cxxopts::value<int>())
        ("l,loop", "Loop animation", cxxopts::value<bool>())
        ("p,pause", "Pause between interpolations in ms", cxxopts::value<int>())
        ("loop-pause", "Pause between loops in ms", cxxopts::value<int>())
        ("c,config", "Path to config file", cxxopts::value<std::string>(config_path_from_cli))
        ("h,help", "Print usage");
    
    options.parse_positional({"frames"});

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    if (!config_path_from_cli.empty()) {
        config.config_file = config_path_from_cli;
    }

    try {
        auto tbl = toml::parse_file(config.config_file);

        config.mode = tbl["mode"].value_or("static");
        if (auto frames = tbl["frames"].as_array()) {
            for (auto&& el : *frames) {
                config.frame_paths.push_back(el.value_or(""));
            }
        }
        config.steps = tbl["steps"].value_or(1);
        config.frame_rate = tbl["rate"].value_or(10);
        config.pause_ms = tbl["pause_ms"].value_or(0);
        config.loop_pause_ms = tbl["loop_pause_ms"].value_or(0);
        config.loop = tbl["loop"].value_or(false);

    } catch (const toml::parse_error& err) {
        // Don't fail if the config file doesn't exist, just use defaults.
        // But do fail if it's present and malformed.
        if (result.count("config")) { // user explicitly passed a config file
             std::cerr << "Parsing of config file failed: " << err << "\n";
             exit(1);
        }
    }

    // Command-line arguments override config file settings
    if (result.count("mode")) config.mode = result["mode"].as<std::string>();
    if (result.count("frames")) config.frame_paths = result["frames"].as<std::vector<std::string>>();
    if (result.count("steps")) config.steps = result["steps"].as<int>();
    if (result.count("rate")) config.frame_rate = result["rate"].as<int>();
    if (result.count("pause")) config.pause_ms = result["pause"].as<int>();
    if (result.count("loop-pause")) config.loop_pause_ms = result["loop-pause"].as<int>();
    if (result.count("loop")) config.loop = result["loop"].as<bool>();

    return config;
}

#endif // FRAME_CONFIG_H
