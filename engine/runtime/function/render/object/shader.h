#pragma once

#include "engine/runtime/global/enum.h"
#include "engine/runtime/global/logger.h"
#include "engine/runtime/platform/rhi/rhi_device.h"
#include "engine/runtime/platform/rhi/rhi_linker.h"
#include "engine/runtime/platform/rhi/rhi_shader_code.h"
#include "engine/runtime/platform/rhi/rhi_shader_program.h"
#include "engine/runtime/resource/file_service.h"
#include "glm/fwd.hpp"
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
    CSM_SHADOWS,
    
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
        m_rhi_resource = device->create_shader_code(m_shader_code_type, m_code);
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

class Shader_feature_dependency_graph {
private:
    std::unordered_map<Shader_feature, std::unordered_set<Shader_feature>> m_dependencies{};

public:
    Shader_feature_dependency_graph() = default;

    Shader_feature_dependency_graph(std::initializer_list<std::pair<Shader_feature, Shader_feature>> dependencies) {
        for (const auto& [feature, dependency] : dependencies) {
            add_dependency(feature, dependency);
        }
    }

    Shader_feature_dependency_graph(const Shader_feature_dependency_graph&) = delete;
    Shader_feature_dependency_graph& operator=(const Shader_feature_dependency_graph&) = delete;
    ~Shader_feature_dependency_graph() = default;

    void add_dependency(Shader_feature feature, Shader_feature dependency) {
        m_dependencies[feature].insert(dependency);
    }

    bool is_dependent(Shader_feature feature, Shader_feature dependency) const {
        if (auto it = m_dependencies.find(feature); it != m_dependencies.end()) {
            return it->second.find(dependency) != it->second.end();
        }
        return false;
    }

    std::unordered_set<Shader_feature> get_dependencies(Shader_feature feature) const {
        if (auto it = m_dependencies.find(feature); it!= m_dependencies.end()) {
            return it->second;
        }
        return {};
    }  

    std::unordered_set<Shader_feature> get_all_dependencies(Shader_feature feature) const {
        std::unordered_set<Shader_feature> dependencies{};
        std::function<void(Shader_feature)> dfs = [&](Shader_feature feature) {
            for (const auto& dep : get_dependencies(feature)) {
                if (dependencies.find(dep) == dependencies.end()) {
                    dependencies.insert(dep);
                    dfs(dep);
                }
            }
        };
        dfs(feature);
        return dependencies;
    }
};

class Shader  {
public:
    using Shader_feature_set = std::bitset<static_cast<size_t>(Shader_feature::MAX_FEATURES)>;
    
    inline static Shader_feature_dependency_graph g_shader_feature_dependency_graph {
        {Shader_feature::HEIGHT_MAP, Shader_feature::NORMAL_MAP},
        {Shader_feature::CSM_SHADOWS, Shader_feature::SHADOWS},
    };

    inline static std::unordered_map<std::string, std::shared_ptr<Shader>> g_shader_cache{};

    static bool is_cached( 
        const std::string shader_name
    ) {
        return g_shader_cache.find(shader_name) != g_shader_cache.end();
    }

    template<typename T>
    static std::shared_ptr<T> get_cached_shader(
        const std::string shader_name
    ) {
        if (auto it = g_shader_cache.find(shader_name); it!= g_shader_cache.end()) {
            if (auto shader = std::dynamic_pointer_cast<T>(it->second)) {
                return shader;
            } else {
                throw std::runtime_error("Shader type mismatch");
            }
        }
        return nullptr;
    }

    static void delete_cache(
        const std::string shader_name
    ) {
        g_shader_cache.erase(shader_name);
    }

    static void add_shader_to_cache(
        const std::string shader_name,
        const std::shared_ptr<Shader>& shader
    ) {
        g_shader_cache[shader_name] = shader;
    }

    static void clear_cache() {
        g_shader_cache.clear();
    }

protected:
    std::string m_shader_name{};
    
    Shader_feature_set m_shader_feature_whitelist{};
    std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> m_main_shader_codes{};
    std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> m_main_shader_uniforms{};
    std::unordered_map<Shader_feature, std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>> m_feature_specific_shader_uniforms{};
    std::unordered_map<Shader_feature_set, std::shared_ptr<Shader_program>> m_variant_shader_programs{};

public:
    Shader(
        const std::string name,
        const std::unordered_map<Shader_type, std::shared_ptr<Shader_code>>& main_shader_codes,
        const std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> main_shader_uniforms,
        const std::unordered_map<Shader_feature, std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>> feature_specific_shader_uniforms,
        const Shader_feature_set& shader_feature_whitelist 
    ) : m_shader_name(name),
        m_main_shader_codes(main_shader_codes),
        m_main_shader_uniforms(main_shader_uniforms),
        m_feature_specific_shader_uniforms(feature_specific_shader_uniforms),
        m_shader_feature_whitelist(shader_feature_whitelist) {}

    virtual ~Shader() = default;

    const std::string& name() const { return m_shader_name; }
    const std::unordered_map<Shader_type, std::shared_ptr<Shader_code>>& main_shader_codes() const { return m_main_shader_codes; }

    const std::unordered_map<Shader_feature_set, std::shared_ptr<Shader_program>> &get_variant_shader_programs() const { return m_variant_shader_programs; }
    const Shader_feature_set& get_shader_feature_whitelist() const { return m_shader_feature_whitelist; }

    const std::shared_ptr<Shader_program>& get_shader_variant(const Shader_feature_set& feature_set) {
        if (auto it = m_variant_shader_programs.find(feature_set); it != m_variant_shader_programs.end()) {
            return it->second;
        }

        if ((feature_set & ~m_shader_feature_whitelist).any()) {
            throw std::invalid_argument("Invalid shader feature set");
        }

        auto [variant_name, shader_codes] = get_shader_codes(feature_set);

        auto variant_shader_program = std::make_shared<Shader_program>(
            variant_name,
            shader_codes,
            get_shader_uniforms(feature_set)
        );

        m_variant_shader_programs.emplace(feature_set, variant_shader_program);
        return m_variant_shader_programs.at(feature_set);
    }

    std::pair<
        std::string, 
        std::unordered_map<Shader_type, std::shared_ptr<Shader_code>>
    > get_shader_codes(const Shader_feature_set& feature_set) {
        std::string variant_name = m_shader_name;

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
        for (const auto& [type, source] : m_main_shader_codes) {
            shader_codes[type] = Shader_code::create(type, "#version 460 core\n" + defines + source->code());
        }
        return std::make_pair(variant_name, shader_codes);
    }

    std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> get_shader_uniforms(const Shader_feature_set& feature_set) {
        std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> uniforms = m_main_shader_uniforms;
        for (size_t i = 0; i < static_cast<size_t>(Shader_feature::MAX_FEATURES); ++i) {
            if (feature_set.test(i)) {
                auto it = m_feature_specific_shader_uniforms.find(static_cast<Shader_feature>(i));
                if (it != m_feature_specific_shader_uniforms.end()) {
                    for (const auto& [name, uniform] : it->second) {
                        uniforms.insert(std::make_pair(name, uniform));
                    }
                }
            }
        }
        return uniforms;
    }

    std::vector<Shader_feature_set> get_all_shader_variants_permutation() {
        std::vector<Shader_feature_set> permutations{};

        std::function<void(size_t u, Shader_feature_set, std::vector<Shader_feature_set>&)> dfs = [&](size_t u, Shader_feature_set current, std::vector<Shader_feature_set>& permutations) {
            
            if ((current & ~m_shader_feature_whitelist).any()) {
                return; // 如果当前组合不在白名单中，则返回
            }

            if (u == static_cast<size_t>(Shader_feature::MAX_FEATURES)) {
                permutations.push_back(current);
                return;
            }

            // 情况1：该位为0
            dfs(u + 1, current, permutations);

            // 情况2：该位为1
            auto dependencies = g_shader_feature_dependency_graph.get_all_dependencies(static_cast<Shader_feature>(u));
            for (const auto& dep : dependencies) {
                if (!current.test(static_cast<size_t>(dep))) {
                    return; // 如果依赖的特征未启用，则返回
                }
            }
            current.set(u);
            dfs(u + 1, current, permutations);
        };

        dfs(0, Shader_feature_set{}, permutations);
        return permutations;
    }

    void generate_all_shader_variants() {
        auto permutations = get_all_shader_variants_permutation();
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
        const std::string name,
        const std::unordered_map<Shader_type, std::shared_ptr<Shader_code>>& main_shader_codes,
        const std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> main_shader_uniforms,
        const std::unordered_map<Shader_feature, std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>> feature_specific_shader_uniforms,
        const Shader_feature_set& shader_feature_whitelist 
    ) {
        return std::make_shared<Shader>(
            name,
            main_shader_codes,
            main_shader_uniforms,
            feature_specific_shader_uniforms,
            shader_feature_whitelist
        );
    }

    static std::string get_shader_code_from_url(const std::string& url) {
        std::unordered_set<std::string> processed_files{};
        return load_shader_code_with_includes(url, processed_files);
    }

    static Shader_feature_set get_shader_feature_set(const std::vector<Shader_feature>& feature_list) {
        Shader_feature_set feature_set{};
        for (const auto& feature : feature_list) {
            feature_set.set(static_cast<size_t>(feature));
        }
        return feature_set;
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


class Phong_shader : public Shader {
public:
    Phong_shader() : Shader(
        "phong_shader", 
        std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> {
            {Shader_type::VERTEX, Shader_code::create(Shader_type::VERTEX, get_shader_code_from_url(File_ser::get_instance()->get_absolute_path("assets/shader/phong.vert")))},
            {Shader_type::FRAGMENT, Shader_code::create(Shader_type::FRAGMENT, get_shader_code_from_url(File_ser::get_instance()->get_absolute_path("assets/shader/phong.frag")))}
        }, 
        std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> {
            {"model", Uniform_entry<glm::mat4>::create(glm::mat4(1.0))},
            {"transparency", Uniform_entry<float>::create(1.0f)},
            {"ka", Uniform_entry<glm::vec3>::create(glm::vec3(0.1, 0.1, 0.1))},
            {"kd", Uniform_entry<glm::vec3>::create(glm::vec3(0.5, 0.5, 0.5))},
            {"ks", Uniform_entry<glm::vec3>::create(glm::vec3(1.0, 1.0, 1.0))},
            {"shininess", Uniform_entry<float>::create(32.0f)}
        },
        std::unordered_map<Shader_feature, std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>> {
            {
                Shader_feature::HEIGHT_MAP, 
                std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> {
                    {"parallax_scale", Uniform_entry<float>::create(0.05f)},
                    {"parallax_layer_count", Uniform_entry<float>::create(10.0f)}
                }
            },
            {
                Shader_feature::SHADOWS,
                std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> {
                    {"shadow_bias", Uniform_entry<float>::create(1.0f)},
                    {"light_size", Uniform_entry<float>::create(0)},
                    {"pcf_radius", Uniform_entry<float>::create(5)},
                    {"pcf_tightness", Uniform_entry<float>::create(100)},
                    {"pcf_sample_count", Uniform_entry<int>::create(100)},
                }
            }
        },
        Shader::Shader_feature_set {
        	Shader::get_shader_feature_set(std::vector<Shader_feature> {
                Shader_feature::ALBEDO_MAP,
                Shader_feature::SPECULAR_MAP,
                Shader_feature::NORMAL_MAP,
                Shader_feature::ALPHA_MAP,
                Shader_feature::HEIGHT_MAP,
                Shader_feature::SHADOWS,
                Shader_feature::CSM_SHADOWS,
            })
        }
    ) {}
    ~Phong_shader() = default;
    static std::shared_ptr<Phong_shader> create() {
        if (Shader::is_cached("phong_shader")) {
            return Shader::get_cached_shader<Phong_shader>("phong_shader");
        } else {
            auto shader = std::make_shared<Phong_shader>();
            Shader::add_shader_to_cache("phong_shader", shader);
            return shader;
        }
    }
};

class Skybox_cubemap_shader : public Shader {
public:
    Skybox_cubemap_shader() : Shader(
        "skybox_cubemap_shader",
        std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> {
            {Shader_type::VERTEX, Shader_code::create(Shader_type::VERTEX, get_shader_code_from_url(File_ser::get_instance()->get_absolute_path("assets/shader/skybox_cubemap.vert")))},
            {Shader_type::FRAGMENT, Shader_code::create(Shader_type::FRAGMENT, get_shader_code_from_url(File_ser::get_instance()->get_absolute_path("assets/shader/skybox_cubemap.frag")))}
        },
        std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> {},
        std::unordered_map<Shader_feature, std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>> {},
        Shader::Shader_feature_set {}
    ) {}

    ~Skybox_cubemap_shader() = default;
    static std::shared_ptr<Skybox_cubemap_shader> create() {
        if (Shader::is_cached("skybox_cubemap_shader")) {
            return Shader::get_cached_shader<Skybox_cubemap_shader>("skybox_cubemap_shader");
        }
        else {
            auto shader = std::make_shared<Skybox_cubemap_shader>();
            Shader::add_shader_to_cache("skybox_cubemap_shader", shader);
            return shader;
        }
    }
};

class Skybox_spherical_shader : public Shader {
public:
    Skybox_spherical_shader() : Shader(
        "skybox_spherical_shader",
        std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> {
            {Shader_type::VERTEX, Shader_code::create(Shader_type::VERTEX, get_shader_code_from_url(File_ser::get_instance()->get_absolute_path("assets/shader/skybox_spherical.vert")))},
            {Shader_type::FRAGMENT, Shader_code::create(Shader_type::FRAGMENT, get_shader_code_from_url(File_ser::get_instance()->get_absolute_path("assets/shader/skybox_spherical.frag")))}
        },
        std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> {},
        std::unordered_map<Shader_feature, std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>> {},
        Shader::Shader_feature_set {}
    ) {}

    ~Skybox_spherical_shader() = default;
    static std::shared_ptr<Skybox_spherical_shader> create() {
        if (Shader::is_cached("skybox_spherical_shader")) {
            return Shader::get_cached_shader<Skybox_spherical_shader>("skybox_spherical_shader");
        }
        else {
            auto shader = std::make_shared<Skybox_spherical_shader>();
            Shader::add_shader_to_cache("skybox_spherical_shader", shader);
            return shader;
        }
    }
};

class Gamma_shader : public Shader {
public:
    Gamma_shader() : Shader(
        "gamma_shader",
        std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> {
            {Shader_type::VERTEX, Shader_code::create(Shader_type::VERTEX, get_shader_code_from_url(File_ser::get_instance()->get_absolute_path("assets/shader/gamma.vert")))},
            {Shader_type::FRAGMENT, Shader_code::create(Shader_type::FRAGMENT, get_shader_code_from_url(File_ser::get_instance()->get_absolute_path("assets/shader/gamma.frag")))}
        },
        std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> {},
        std::unordered_map<Shader_feature, std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>> {},
        Shader::Shader_feature_set {}
    ) {}
    ~Gamma_shader() = default;
    static std::shared_ptr<Gamma_shader> create() {
        if (Shader::is_cached("gamma_shader")) {
            return Shader::get_cached_shader<Gamma_shader>("gamma_shader");
        } else {
            auto shader = std::make_shared<Gamma_shader>();
            Shader::add_shader_to_cache("gamma_shader", shader);
            return shader;
        }
    }
};

class Shadow_caster_shader : public Shader {
public:
    Shadow_caster_shader() : Shader(
        "shadow_caster_shader",
        std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> {
            {Shader_type::VERTEX, Shader_code::create(Shader_type::VERTEX, get_shader_code_from_url(File_ser::get_instance()->get_absolute_path("assets/shader/shadow_caster.vert")))},
            {Shader_type::FRAGMENT, Shader_code::create(Shader_type::FRAGMENT, get_shader_code_from_url(File_ser::get_instance()->get_absolute_path("assets/shader/shadow_caster.frag")))}
        },
        std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> {
            {"model", Uniform_entry<glm::mat4>::create(glm::mat4(1.0))}
        },
        std::unordered_map<Shader_feature, std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>> {},
        Shader::Shader_feature_set {}
    ) {}
    ~Shadow_caster_shader() = default;
    
    static std::shared_ptr<Shadow_caster_shader> create() {
        if (Shader::is_cached("shadow_caster_shader")) {
            return Shader::get_cached_shader<Shadow_caster_shader>("shadow_caster_shader");
        } else {
            auto shader = std::make_shared<Shadow_caster_shader>();
            Shader::add_shader_to_cache("shadow_caster_shader", shader);
            return shader;
        }
    }
};

}