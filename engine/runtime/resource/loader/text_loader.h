#pragma once

#include "engine/runtime/global/base.h" 
#include "engine/runtime/resource/resource_base.h"
#include <memory>

namespace rtr {

class Text {
private:
    std::string m_text{};

public:
    Text(const std::string& path) {
        std::ifstream file{};
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            file.open(path);
            std::stringstream buffer{};
            buffer << file.rdbuf();
            file.close();
            m_text = buffer.str();
        }
        catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
    }

    ~Text() = default;

    const std::string& content() const { return m_text; }
    const char* c_str() const { return m_text.c_str(); }
    int size() const { return m_text.size(); }
    bool empty() const { return m_text.empty(); }

    static std::shared_ptr<Text> create(const std::string& path) {
        return std::make_shared<Text>(path);
    }
};


}