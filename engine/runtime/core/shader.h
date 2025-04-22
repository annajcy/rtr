#pragma once

#include "engine/runtime/global/enum.h"
#include "engine/runtime/global/guid.h"
#include "engine/runtime/platform/rhi/rhi_device.h"
#include "engine/runtime/platform/rhi/rhi_linker.h"
#include "engine/runtime/platform/rhi/rhi_shader_code.h"
#include "engine/runtime/platform/rhi/rhi_shader_program.h"
#include <bitset>
#include <cstddef>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
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

class Shader_code : public GUID, public RHI_linker<RHI_shader_code> {
protected:
    std::string m_code{};
    Shader_type m_shader_code_type{};

public:
    Shader_code(Shader_type type, const std::string& code) : m_code(code), m_shader_code_type(type) {}
    ~Shader_code() = default;
    const std::string& code() const { return m_code; }
    Shader_type shader_code_type() const { return m_shader_code_type; }
    static std::shared_ptr<Shader_code> create(Shader_type type, const std::string& code) {
        return std::make_shared<Shader_code>(type, code);
    }

    virtual void link(const std::shared_ptr<RHI_device>& device) override {
        m_rhi_resource = device->create_shader_code(m_shader_code_type, m_code);
    }
};

class Shader_program : public GUID, public RHI_linker<RHI_shader_program> {
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

        std::unordered_map<Shader_type, std::shared_ptr<RHI_shader_code>> rhi_shader_codes{};
        for (const auto& [type, code] : m_shader_codes) {
            if (!code->is_linked()) code->link(device);
            rhi_shader_codes[type] = code->rhi_resource();
        }

        m_rhi_resource = device->create_shader_program(
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

class Shader : public GUID {
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
        std::string variant_name = m_main_shader_program->name();

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
        
        // 预处理定义
        std::string defines = "// " + variant_name + "\n";
        for (const auto& feat : active_features) {
            defines += "#define ";
            defines += feat;
            defines += "\n";
        }

        std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> shader_codes{};

        // 处理着色器源码
        for (const auto& [type, source] : m_main_shader_program->shader_codes()) {
            shader_codes[type] = Shader_code::create(type, "#version 460 core\n" + defines + source->code());
        }

        auto variant_shader_program = std::make_shared<Shader_program>(
            variant_name,
            shader_codes,
            m_main_shader_program->uniforms()
        );

        m_variant_shader_programs.emplace(feature_set, variant_shader_program);
        return m_variant_shader_programs.at(feature_set);
    }

    std::vector<feature_set> get_shader_variants_permutation() {
        std::vector<feature_set> permutations{};

        std::function<void(size_t u, feature_set, std::vector<feature_set>&)> dfs = [&](size_t u, feature_set current, std::vector<feature_set>& permutations) {
            if (u == static_cast<size_t>(Shader_feature::MAX_FEATURES)) {
                permutations.push_back(current);
                return;
            }
            
            if (m_shader_feature_whitelist.test(u)) {
                // 情况1：该位为0
                dfs(u + 1, current, permutations);
                // 情况2：该位为1
                current.set(u);
                dfs(u + 1, current, permutations);
            } else {
                // 不在白名单则保持为0
                dfs(u + 1, current, permutations);
            }
        };

        dfs(0, feature_set{}, permutations);
        return permutations;
    }

    // 替代原来的空实现
    void premake_shader_variants() {
        auto permutations = get_shader_variants_permutation();
        for (const auto& perm : permutations) {
            get_shader_variant(perm);  // 预生成所有变体
        }
    }

    void link_all_shader_variants(const std::shared_ptr<RHI_device>& device) {
        for (auto &[_, shader_program] : m_variant_shader_programs) {
            if (!shader_program->is_linked())
                shader_program->link(device);
        }
    }

    static std::shared_ptr<Shader> create(
        const std::shared_ptr<Shader_program>& main_program,
        const feature_set& shader_feature_whitelist
    ) {
        return std::make_shared<Shader>(main_program, shader_feature_whitelist);
    }

    static std::string get_shader_code_from_url(const std::string& url) {
        std::unordered_set<std::string> processed_files{};
        return load_shader_code_with_includes(url, processed_files);
    }

    static feature_set get_shader_feature_set(const std::vector<Shader_feature>& feature_list) {
        feature_set feature_set{};
        for (const auto& feature : feature_list) {
            feature_set.set(static_cast<size_t>(feature));
        }
        return feature_set;
    }

    static std::shared_ptr<Shader> create_phong_shader() {

        std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> shader_codes{};

        shader_codes[Shader_type::VERTEX] = Shader_code::create(
            Shader_type::VERTEX,
            get_shader_code_from_url("assets/shader/phong.vert")
        ); 

        shader_codes[Shader_type::FRAGMENT] = Shader_code::create(
            Shader_type::FRAGMENT,
            get_shader_code_from_url("assets/shader/phong.frag")
        );


        auto main_program = std::make_shared<Shader_program>(
            "phong_shader",
            shader_codes,
            std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>{}
        );        

        return std::make_shared<Shader>(
            main_program,
            get_shader_feature_set(std::vector<Shader_feature>{
                Shader_feature::SHADOWS,
                Shader_feature::ALBEDO_MAP,
                Shader_feature::ALPHA_MAP,
                Shader_feature::NORMAL_MAP,
                Shader_feature::HEIGHT_MAP,
                Shader_feature::SPECULAR_MAP
            })
        );
    }

    static std::shared_ptr<Shader> create_pbr_shader() {

        std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> shader_codes{};

        shader_codes[Shader_type::VERTEX] = Shader_code::create(
            Shader_type::VERTEX,
            get_shader_code_from_url("assets/shader/pbr.vert")
        ); 

        shader_codes[Shader_type::FRAGMENT] = Shader_code::create(
            Shader_type::FRAGMENT,
            get_shader_code_from_url("assets/shader/pbr.frag")
        );

        auto main_program = std::make_shared<Shader_program>(
            "pbr_shader",
            shader_codes,
            std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>{}
        );        

        return std::make_shared<Shader>(
            main_program,
            get_shader_feature_set(std::vector<Shader_feature>{
                Shader_feature::SHADOWS,
                Shader_feature::ALBEDO_MAP,
                Shader_feature::ALPHA_MAP,
                Shader_feature::NORMAL_MAP,
                Shader_feature::HEIGHT_MAP,
                Shader_feature::METALLIC_MAP,
                Shader_feature::ROUGHNESS_MAP,
                Shader_feature::AO_MAP,
                Shader_feature::EMISSIVE_MAP
            })
        );

    }

    static std::shared_ptr<Shader> create_skybox_cubemap_shader() {

        std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> shader_codes{};
        shader_codes[Shader_type::VERTEX] = Shader_code::create(
            Shader_type::VERTEX,
            get_shader_code_from_url("assets/shader/skybox_cubemap.vert")
        );

        shader_codes[Shader_type::FRAGMENT] = Shader_code::create(
            Shader_type::FRAGMENT,
            get_shader_code_from_url("assets/shader/skybox_cubemap.frag")
        );

        auto main_program = std::make_shared<Shader_program>(
            "skybox_cubemap_shader",
            shader_codes,
            std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>{}
        );     

        return std::make_shared<Shader>(
            main_program,
            get_shader_feature_set({})
        );
        
    }

    static std::shared_ptr<Shader> create_skybox_spherical_shader() {

        std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> shader_codes{};
        shader_codes[Shader_type::VERTEX] = Shader_code::create(
            Shader_type::VERTEX,
            get_shader_code_from_url("assets/shader/skybox_spherical.vert")
        );

        shader_codes[Shader_type::FRAGMENT] = Shader_code::create(
            Shader_type::FRAGMENT,
            get_shader_code_from_url("assets/shader/skybox_spherical.frag")
        );

        auto main_program = std::make_shared<Shader_program>(
            "skybox_spherical_shader",
            shader_codes,
            std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>{}
        );

        return std::make_shared<Shader>(
            main_program,
            get_shader_feature_set({})
        );
    }

    static std::shared_ptr<Shader> create_gamma_shader() {
        std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> shader_codes{};
        shader_codes[Shader_type::VERTEX] = Shader_code::create(
            Shader_type::VERTEX,
            get_shader_code_from_url("assets/shader/gamma.vert")
        );
        shader_codes[Shader_type::FRAGMENT] = Shader_code::create(
            Shader_type::FRAGMENT,
            get_shader_code_from_url("assets/shader/gamma.frag")
        );
        auto main_program = std::make_shared<Shader_program>(
            "gamma_shader",
            shader_codes,
            std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>{}
        );
        return std::make_shared<Shader>(
            main_program,
            get_shader_feature_set({})
        );
    }

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