#pragma once

#include "engine/runtime/function/render/frontend/shader.h"
#include "engine/runtime/function/render/frontend/texture.h"

#include "engine/runtime/platform/rhi/rhi_pipeline_state.h"
#include "engine/runtime/platform/rhi/rhi_shader_program.h"
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace rtr {

class Shader_base {
protected:
    std::string m_shader_name{};
    std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> m_main_shader_codes{};

public:
    Shader_base(
        const std::string name,
        const std::unordered_map<Shader_type, std::shared_ptr<Shader_code>>& main_shader_codes
    ) : m_shader_name(name), 
        m_main_shader_codes(main_shader_codes) {}
    const std::string& name() const { return m_shader_name; }
    const std::unordered_map<Shader_type, std::shared_ptr<Shader_code>>& main_shader_codes() const { return m_main_shader_codes; }
    virtual ~Shader_base() = default;
};

template<typename Shader_feature>
class Shader_feature_dependency_graph {
private:
    std::unordered_map<Shader_feature, std::unordered_set<Shader_feature>> m_dependencies{};
    std::unordered_map<Shader_feature, std::unordered_set<Shader_feature>> m_exclusions{};

public:
    Shader_feature_dependency_graph() = default;

    Shader_feature_dependency_graph(
        std::unordered_map<Shader_feature, std::unordered_set<Shader_feature>> dependencies,
        std::unordered_map<Shader_feature, std::unordered_set<Shader_feature>> exclusions
    ) {
        for (const auto& [feature, deps] : dependencies) {
            for (const auto& dep : deps) {
                add_dependency(feature, dep);
            }
        }

        for (const auto& [feature, excls] : exclusions) {
            for (const auto& exc : excls) {
                add_exclusion(feature, exc);
            }
        }
    }

    ~Shader_feature_dependency_graph() = default;

    void add_dependency(Shader_feature feature, Shader_feature dependency) {
        m_dependencies[feature].insert(dependency);
    }

    void add_exclusion(Shader_feature feature, Shader_feature exclusion) {
        m_exclusions[feature].insert(exclusion);
    }

    bool is_dependent(Shader_feature feature, Shader_feature dependency) const {
        if (auto it = m_dependencies.find(feature); it != m_dependencies.end()) {
            return it->second.find(dependency) != it->second.end();
        }
        return false;
    }

    bool is_excluded(Shader_feature feature, Shader_feature exclusion) const {
        if (auto it = m_exclusions.find(feature); it != m_exclusions.end()) {
            return it->second.find(exclusion) != it->second.end();
        }
        return false;
    }

    std::unordered_set<Shader_feature> get_dependencies(Shader_feature feature) const {
        if (auto it = m_dependencies.find(feature); it!= m_dependencies.end()) {
            return it->second;
        }
        return {};
    }  

    std::unordered_set<Shader_feature> get_exclusions(Shader_feature feature) const {
        if (auto it = m_exclusions.find(feature); it!= m_exclusions.end()) {
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

    std::unordered_set<Shader_feature> get_all_exclusions(Shader_feature feature) const {
        std::unordered_set<Shader_feature> exclusions{};
        std::function<void(Shader_feature)> dfs = [&](Shader_feature feature) {
            for (const auto& exc : get_exclusions(feature)) {
                if (exclusions.find(exc) == exclusions.end()) {
                    exclusions.insert(exc);
                    dfs(exc);
                }
            }
        };
        dfs(feature);
        return exclusions;
    }
};

template<typename Shader_feature>
inline constexpr const char* shader_feature_to_defines(Shader_feature feature) {
    throw std::runtime_error("shader_feature_to_defines not implemented for this Shader_feature type");
}

template<typename Shader_feature>
class Shader : public Shader_base {
public:
    using Shader_feature_set = std::bitset<static_cast<size_t>(Shader_feature::MAX_FEATURES)>;
    using Shader_feature_dependency_graph_v = Shader_feature_dependency_graph<Shader_feature>;

    static Shader_feature_set get_shader_feature_set(const std::vector<Shader_feature>& feature_list) {
        Shader_feature_set feature_set{};
        for (const auto& feature : feature_list) {
            feature_set.set(static_cast<size_t>(feature));
        }
        return feature_set;
    }

protected:
    std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> m_main_shader_uniforms{};
    std::unordered_map<Shader_feature, std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>> m_feature_specific_shader_uniforms{};
    std::unordered_map<Shader_feature_set, std::shared_ptr<Shader_program>> m_variant_shader_programs{};
    Shader_feature_dependency_graph_v m_feature_dependency_graph{};

public:
    Shader(
        const std::string name,
        const std::unordered_map<Shader_type, std::shared_ptr<Shader_code>>& main_shader_codes,
        const std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> main_shader_uniforms,
        const std::unordered_map<Shader_feature, std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>> feature_specific_shader_uniforms,
        const Shader_feature_dependency_graph_v& feature_dependency_graph
    ) : Shader_base(name, main_shader_codes),
        m_main_shader_uniforms(main_shader_uniforms),
        m_feature_specific_shader_uniforms(feature_specific_shader_uniforms), 
        m_feature_dependency_graph(feature_dependency_graph) {}

    virtual ~Shader() = default;

    const std::unordered_map<Shader_feature_set, std::shared_ptr<Shader_program>> &variant_shader_programs() const { return m_variant_shader_programs; }

    const std::shared_ptr<Shader_program>& get_shader_variant(const Shader_feature_set& feature_set) {
        if (auto it = m_variant_shader_programs.find(feature_set); it != m_variant_shader_programs.end()) {
            return it->second;
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
                active_features.push_back(
                    shader_feature_to_defines<Shader_feature>(
                        static_cast<Shader_feature>(i)
                    )
                );
            }
        }

        for (const auto& feat : active_features) {
            variant_name += "_";
            variant_name += feat;
        }
        
        std::string defines = "// " + variant_name + "\n";
        for (const auto& feat : active_features) {
            defines += "#define ";
            defines += feat;
            defines += "\n";
        }

        std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> shader_codes{};

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

            if (u == static_cast<size_t>(Shader_feature::MAX_FEATURES)) {
                permutations.push_back(current);
                return;
            }

            // 情况1：该位为0
            dfs(u + 1, current, permutations);

            // 情况2：该位为1
            auto dependencies = m_feature_dependency_graph.get_all_dependencies(static_cast<Shader_feature>(u));
            for (const auto& dep : dependencies) {
                if (!current.test(static_cast<size_t>(dep))) {
                    return; // 如果依赖的特征未启用，则返回
                }
            }

            auto exclusions = m_feature_dependency_graph.get_all_exclusions(static_cast<Shader_feature>(u));
            for (const auto& exc : exclusions) {
                if (current.test(static_cast<size_t>(exc))) {
                    return; // 如果有冲突的特征启用，则返回
                }
            }

            // 启用该位
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
};

enum class None_shader_feature {};

template<>
class Shader<None_shader_feature> : public Shader_base {
private:
    std::shared_ptr<Shader_program> m_shader_program; 
    std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> m_main_shader_uniforms;

public:
    Shader(
        const std::string& shader_name,
        const std::unordered_map<Shader_type, std::shared_ptr<Shader_code>>& main_shader_codes,
        const std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>& main_shader_uniforms
    ) : Shader_base(shader_name, main_shader_codes),
        m_main_shader_uniforms(main_shader_uniforms) {}

    virtual ~Shader() = default;

    std::shared_ptr<Shader_program> get_shader_program() {
        if (!m_shader_program) {
            std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> processed_codes{};
            for (const auto& main_code_entry : this->m_main_shader_codes) { 
                processed_codes[main_code_entry.first] = Shader_code::create(
                    main_code_entry.first,
                    "#version 460 core\n" + main_code_entry.second->code() 
                );
            }

            m_shader_program = std::make_shared<Shader_program>(
                this->m_shader_name, 
                processed_codes,
                m_main_shader_uniforms
            );
        }
        return m_shader_program;
    }

    void link_shader(const std::shared_ptr<RHI_device>& device) {
        std::shared_ptr<Shader_program> program_to_link = get_shader_program();
        if (program_to_link && !program_to_link->is_linked()) {
            program_to_link->link(device);
        }
    }
};

enum class Material_type {
    PHONG,
    SKYBOX_CUBEMAP,
    SKYBOX_SPHERICAL,
    GAMMA,
    SHADOW_CASTER
};

class Material {
    
protected:
    Material_type m_material_type{};

public:
    Material(
        Material_type material_type
    ) : m_material_type(material_type){}

    virtual ~Material() = default;

    Material_type material_type() const { return m_material_type; }

    virtual std::shared_ptr<Shader_program> get_shader_program() = 0;
    virtual std::unordered_map<unsigned int, std::shared_ptr<Texture>> get_texture_map() = 0;
    virtual Pipeline_state get_pipeline_state() const = 0;
    virtual void modify_shader_uniform(const std::shared_ptr<RHI_shader_program>& shader_program) = 0;
};






}