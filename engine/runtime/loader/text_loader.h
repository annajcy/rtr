#pragma once
#include "engine/global/base.h"

namespace rtr {

class Text_loader {

    static std::string load_from_file(const std::string &filename) {
        std::ifstream file{};
        std::string content{};
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            file.open(filename);
            std::stringstream buffer{};
            buffer << file.rdbuf();
            file.close();
            content = buffer.str();
            
        }
        catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
        return content;
    }

};

}