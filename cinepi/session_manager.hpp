#pragma once

#include <iostream>
#include <string>
#include <filesystem>
#include "raw_options.hpp"

namespace fs = std::filesystem;

class SessionManager {
public:
    SessionManager(RawOptions* options) : options_(options) {}

    bool isDiskMounted() const {
        return fs::exists(fs::path(options_->mediaDest));
    }

    bool startNewClip(unsigned int clip_number) {
        if (!isDiskMounted()) return false;
        generateFilename(clip_number);
        return fs::create_directories(options_->mediaDest + std::string("/") + options_->folder);
    }

    bool ensureStillsFolder() {
        if (!isDiskMounted()) return false;
        std::string stillsPath = options_->mediaDest + std::string("/stills");
        if (!fs::exists(fs::path(stillsPath))) {
            return fs::create_directories(stillsPath);
        }
        return true;
    }

private:
    RawOptions* options_;

    void generateFilename(unsigned int number) {
        char filename[128];
        std::time_t raw_time;
        std::time(&raw_time);
        char time_string[32];
        std::tm *time_info = std::localtime(&raw_time);
        std::strftime(time_string, sizeof(time_string), "%y-%m-%d_%H%M", time_info);
        snprintf(filename, sizeof(filename), "%s_%s_C%05d", "CINEPI", time_string, number);
        options_->folder = std::string(filename);
    }
};
