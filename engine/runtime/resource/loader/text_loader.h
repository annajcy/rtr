#pragma once
#include "engine/global/base.h"
#include "engine/runtime/resource/resource_base.h"

namespace rtr {

class Text : public Resource_base {
private:
    std::string m_text{};
public:
    Text(const std::string& path) : 
    Resource_base(
        Resource_type::TEXT, 
        Hash::from_string(path)) {
       load_from_file(path, m_text);
    }

    Text(const char* data) : 
    Resource_base(
        Resource_type::TEXT, 
        Hash::from_raw_data(reinterpret_cast<const unsigned char*>(data), strlen(data))), 
        m_text(data) {}

    using Ptr = std::shared_ptr<Text>;

    ~Text() = default;

    const std::string& content() const { return m_text; }
    const char* c_str() const { return m_text.c_str(); }
    int size() const { return m_text.size(); }
    bool empty() const { return m_text.empty(); }

private:
    void load_from_file(const std::string& path, std::string& str) {
        std::ifstream file{};
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            file.open(path);
            std::stringstream buffer{};
            buffer << file.rdbuf();
            file.close();
            str = buffer.str();
        }
        catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
    }

};

class Text_loader {
public:
    Text_loader() = default;
    ~Text_loader() = default;
    static Text::Ptr load_from_path(const std::string& path) {
        return std::make_shared<Text>(path);
    }

};

}