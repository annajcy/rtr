#pragma once

#include "engine/runtime/function/context/global_context.h"
#include "engine/runtime/global/base.h"
#include "engine/runtime/global/enum.h"
#include "engine/runtime/platform/rhi/rhi_device.h"
#include "engine/runtime/platform/rhi/rhi_shader_code.h"
#include "engine/runtime/platform/rhi/rhi_shader_program.h"
#include <bitset>
#include <cstddef>
#include <format>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
#include <filesystem>

namespace rtr {

enum class Shader_feature {
    SHADOWS,
    ALBEDO_MAP,
    ALPHA_MAP,
    NORMAL_MAP,
    HEIGHT_MAP,

    // Phong 专属
    SPECULAR_MAP,

    // PBR 专属
    METALLIC_MAP,
    ROUGHNESS_MAP,
    AO_MAP,
    EMISSIVE_MAP,
    MAX_FEATURES,
};

inline constexpr const char* shader_feature_to_defines(Shader_feature feature) {
    switch (feature) {
        case Shader_feature::SHADOWS:
            return "ENABLE_SHADOWS";
        case Shader_feature::ALBEDO_MAP:
            return "ENABLE_ALBEDO_MAP";
        case Shader_feature::ALPHA_MAP:
            return "ENABLE_ALPHA_MAP";
        case Shader_feature::NORMAL_MAP:
            return "ENABLE_NORMAL_MAP";
        case Shader_feature::HEIGHT_MAP:
            return "ENABLE_HEIGHT_MAP";
        case Shader_feature::SPECULAR_MAP:
            return "ENABLE_SPECULAR_MAP";
        case Shader_feature::METALLIC_MAP:
            return "ENABLE_METALLIC_MAP";
        case Shader_feature::ROUGHNESS_MAP:
            return "ENABLE_ROUGHNESS_MAP";
        case Shader_feature::AO_MAP:
            return "ENABLE_AO_MAP";
        case Shader_feature::EMISSIVE_MAP:
            return "ENABLE_EMISSIVE_MAP";
        default:
            return "UNKNOWN";
    }
}

struct Shader_program {
    std::string name{};
    std::unordered_map<Shader_type, std::string> shader_sources{};
};

class Shader {
public:
    using feature_set = std::bitset<static_cast<size_t>(Shader_feature::MAX_FEATURES)>;

protected:
    std::shared_ptr<Shader_program> m_main_shader_program{};
    feature_set m_shader_feature_whitelist{};

    std::unordered_map<feature_set, std::shared_ptr<Shader_program>> m_variant_shader_programs{};

public:
    Shader(
        const std::shared_ptr<Shader_program>& main_program, 
        const feature_set& shader_feature_whitelist 
    ) : m_main_shader_program(main_program),
        m_shader_feature_whitelist(shader_feature_whitelist) {}
    ~Shader() = default;

    const std::shared_ptr<Shader_program>& get_main_shader() const { return m_main_shader_program; }
    const std::unordered_map<feature_set, std::shared_ptr<Shader_program>> &get_variant_shader_programs() const { return m_variant_shader_programs; }
    const feature_set& get_shader_feature_whitelist() const { return m_shader_feature_whitelist; }

    const std::shared_ptr<Shader_program>& get_shader_variant(const feature_set& feature_set) {
        if (auto it = m_variant_shader_programs.find(feature_set); it != m_variant_shader_programs.end()) {
            return it->second;
        }

        if ((feature_set & ~m_shader_feature_whitelist).any()) {
            throw std::invalid_argument("Invalid shader feature set");
        }

        // 生成变体名称
        std::string variant_name = m_main_shader_program->name;
        std::vector<std::string> active_features{};
        for (size_t i = 0; i < static_cast<size_t>(Shader_feature::MAX_FEATURES); ++i) {
            if (feature_set.test(i)) {
                active_features.push_back(shader_feature_to_defines(static_cast<Shader_feature>(i)));
            }
        }

        for (const auto& feat : active_features) {
            variant_name += "_";
            variant_name += feat;
        }

        // 创建变体程序
        auto variant_shader_program = std::make_shared<Shader_program>();
        variant_shader_program->name = variant_name;
        
        // 预处理定义
        std::string defines = "// " + variant_name + "\n";
        for (const auto& feat : active_features) {
            defines += "#define ";
            defines += feat;
            defines += "\n";
        }

        // 处理着色器源码
        for (const auto& [type, source] : m_main_shader_program->shader_sources) {
            variant_shader_program->shader_sources[type] = defines + source;
        }

        m_variant_shader_programs.emplace(feature_set, variant_shader_program);
        return m_variant_shader_programs.at(feature_set);
    }


    static std::string get_shader_code_from_url(const std::string& url) {
        std::unordered_set<std::string> processed_files{};
        return load_shader_code_with_includes(url, processed_files);
    }

    static feature_set get_phong_shader_feature_whitelist() {
        feature_set keyword_whitelist{};
        keyword_whitelist.set(static_cast<size_t>(Shader_feature::SHADOWS));
        keyword_whitelist.set(static_cast<size_t>(Shader_feature::ALBEDO_MAP));
        keyword_whitelist.set(static_cast<size_t>(Shader_feature::ALPHA_MAP));
        keyword_whitelist.set(static_cast<size_t>(Shader_feature::NORMAL_MAP));
        keyword_whitelist.set(static_cast<size_t>(Shader_feature::HEIGHT_MAP));

        keyword_whitelist.set(static_cast<size_t>(Shader_feature::SPECULAR_MAP));
        return keyword_whitelist;
    }

    static feature_set get_pbr_shader_feature_whitelist() {
        feature_set keyword_whitelist{};
        keyword_whitelist.set(static_cast<size_t>(Shader_feature::SHADOWS));
        keyword_whitelist.set(static_cast<size_t>(Shader_feature::ALBEDO_MAP));
        keyword_whitelist.set(static_cast<size_t>(Shader_feature::ALPHA_MAP));
        keyword_whitelist.set(static_cast<size_t>(Shader_feature::NORMAL_MAP));
        keyword_whitelist.set(static_cast<size_t>(Shader_feature::HEIGHT_MAP));
        
        keyword_whitelist.set(static_cast<size_t>(Shader_feature::METALLIC_MAP));
        keyword_whitelist.set(static_cast<size_t>(Shader_feature::ROUGHNESS_MAP));
        keyword_whitelist.set(static_cast<size_t>(Shader_feature::AO_MAP));
        keyword_whitelist.set(static_cast<size_t>(Shader_feature::EMISSIVE_MAP));
        return keyword_whitelist;
    }

    static feature_set get_skybox_cubemap_shader_feature_whitelist() {
        feature_set keyword_whitelist{};
        return keyword_whitelist;
    }

    static feature_set get_skybox_spherical_shader_feature_whitelist() {
        feature_set keyword_whitelist{};
        return keyword_whitelist;
    }

    static std::shared_ptr<Shader> create_phong_shader() {
        auto main_program = std::make_shared<Shader_program>();
        main_program->name = "phong_shader";
        main_program->shader_sources[Shader_type::VERTEX] = get_shader_code_from_url("assets/shader/phong.vert");
        main_program->shader_sources[Shader_type::FRAGMENT] = get_shader_code_from_url("assets/shader/phong.frag");
        return std::make_shared<Shader>(
            main_program,
            get_phong_shader_feature_whitelist()
        );
    }

    static std::shared_ptr<Shader> create_pbr_shader() {
        auto main_program = std::make_shared<Shader_program>();
        main_program->name = "pbr_shader";
        main_program->shader_sources[Shader_type::VERTEX] = get_shader_code_from_url("assets/shader/pbr.vert");
        main_program->shader_sources[Shader_type::FRAGMENT] = get_shader_code_from_url("assets/shader/pbr.frag");
        return std::make_shared<Shader>(
            main_program,
            get_pbr_shader_feature_whitelist()
        );
    }

    static std::shared_ptr<Shader> create_skybox_cubemap_shader() {
        auto main_program = std::make_shared<Shader_program>();
        main_program->name = "skybox_cubemap_shader";
        main_program->shader_sources[Shader_type::VERTEX] = get_shader_code_from_url("assets/shader/skybox_cubemap.vert");
        main_program->shader_sources[Shader_type::FRAGMENT] = get_shader_code_from_url("assets/shader/skybox_cubemap.frag");
        return std::make_shared<Shader>(
            main_program,
            get_skybox_cubemap_shader_feature_whitelist()
        );
    }

    static std::shared_ptr<Shader> create_skybox_spherical_shader() {
        auto main_program = std::make_shared<Shader_program>();
        main_program->name = "skybox_spherical_shader";
        main_program->shader_sources[Shader_type::VERTEX] = get_shader_code_from_url("assets/shader/skybox_spherical.vert");
        main_program->shader_sources[Shader_type::FRAGMENT] = get_shader_code_from_url("assets/shader/skybox_spherical.frag");
        return std::make_shared<Shader>(
            main_program,
            get_skybox_spherical_shader_feature_whitelist()
        );
    }

private:
    static std::string load_shader_code_with_includes(
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
                buffer << load_shader_code_with_includes(include_path.string(), processed) << "\n";
            } else {
                buffer << line << "\n";
            }
        }
        return buffer.str();
    }
};

}