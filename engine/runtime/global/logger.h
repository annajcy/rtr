#pragma once

#include <iostream>

namespace rtr {

class Logger {
public:
    static void log(const std::string& message) {
        std::cout << message << std::endl;
    }

    static void log_error(const std::string& message) {
        std::cerr << "ERROR: " << message << std::endl;
    }

    static void log_warning(const std::string& message) {
        std::cerr << "WARNING: " << message << std::endl;
    }

    static void log_info(const std::string& message) {
        std::cout << "INFO: " << message << std::endl;
    }

    static void log_debug(const std::string& message) {
        std::cout << "DEBUG: " << message << std::endl;
    }

};
}