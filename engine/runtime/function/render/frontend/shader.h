#pragma once

#include "engine/runtime/tool/logger.h"
#include "engine/runtime/platform/rhi/rhi_device.h"
#include "engine/runtime/platform/rhi/rhi_linker.h"
#include "engine/runtime/platform/rhi/rhi_shader_code.h"
#include "engine/runtime/platform/rhi/rhi_shader_program.h"
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <regex>
#include <filesystem>

namespace rtr {

class Shader_code : public RHI_linker<RHI_shader_code> {
protected:
    std::string m_code{};
    Shader_type m_shader_code_type{};

public:
    Shader_code(
        Shader_type type, 
        const std::string& code
    ) : m_code(code), 
        m_shader_code_type(type) {}
        
    ~Shader_code() = default;
    
    const std::string& code() const { return m_code; }

    Shader_type shader_code_type() const { return m_shader_code_type; }

    static std::shared_ptr<Shader_code> create(Shader_type type, const std::string& code) {
        return std::make_shared<Shader_code>(type, code);
    }

    virtual void link(const std::shared_ptr<RHI_device>& device) override {
        m_rhi = device->create_shader_code(m_shader_code_type, m_code);
    }

    static std::string load_shader_code(const std::string& url) {
        std::unordered_set<std::string> processed_files{};
        return dfs(
            url, 
            processed_files
        );
    }

    static std::string dfs(
        const std::string& file_path,
        std::unordered_set<std::string>& processed
    ) {
        namespace fs = std::filesystem;
        
        // 获取规范化的绝对路径
        fs::path full_path = fs::absolute(fs::path(file_path)).lexically_normal();
        std::string path_str = full_path.string();
        
        // 检查循环包含
        if (processed.count(path_str)) {
            std::cerr << "Warning: Circular include detected: " << path_str << std::endl;
            return "";
        }

        processed.insert(path_str);

        // 读取文件内容
        std::ifstream file(full_path);
        if (!file.is_open()) throw std::runtime_error("Failed to open: " + path_str);
        
        std::stringstream buffer{};
        std::string line{};
        std::regex include_pattern(R"(^\s*#include\s+\"(.+)\"\s*$)");

        // 逐行处理
        while (std::getline(file, line)) {
            std::smatch match{};
            if (std::regex_match(line, match, include_pattern)) {
                // 获取相对路径并递归加载
                fs::path include_path = full_path.parent_path() / match[1].str();
                buffer << dfs(include_path.string(), processed) << "\n";
            } else {
                buffer << line << "\n";
            }
        }
        return buffer.str();
    }
};

class Shader_program : public RHI_linker<RHI_shader_program> {
private:
    std::string m_name{};
    std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> m_shader_codes{};
    std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> m_uniforms{};

public:
    Shader_program(
        const std::string& name,
        const std::unordered_map<Shader_type, std::shared_ptr<Shader_code>>& shader_codes,
        const std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>& uniforms
    ) : m_name(name), 
        m_shader_codes(shader_codes), 
        m_uniforms(uniforms) {}

    ~Shader_program() = default;

    const std::string& name() const { return m_name; }
    const std::unordered_map<Shader_type, std::shared_ptr<Shader_code>>& shader_codes() const { return m_shader_codes; }
    const std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>& uniforms() const { return m_uniforms; }

    virtual void link(const std::shared_ptr<RHI_device>& device) override {

        Log_sys::get_instance()->log(Logging_system::Level::info, "shader {} trys to link to rhi", m_name);

        std::unordered_map<Shader_type, std::shared_ptr<RHI_shader_code>> rhi_shader_codes{};
        for (const auto& [type, code] : m_shader_codes) {
            rhi_shader_codes[type] = code->rhi(device);
        }

        m_rhi = device->create_shader_program(
            rhi_shader_codes,
            m_uniforms
        );
    }

    static std::shared_ptr<Shader_program> create(
        const std::string& name,
        const std::unordered_map<Shader_type, std::shared_ptr<Shader_code>>& shader_codes,
        const std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>& uniforms
    ) {
        return std::make_shared<Shader_program>(name, shader_codes, uniforms);
    }
};

}