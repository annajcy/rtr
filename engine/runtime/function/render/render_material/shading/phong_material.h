#pragma once

#include "engine/runtime/function/render/render_material/material.h"
#include "engine/runtime/function/render/render_frontend/texture.h"

#include "engine/runtime/platform/rhi/rhi_pipeline_state.h"
#include "engine/runtime/platform/rhi/rhi_shader_program.h"
#include "glm/fwd.hpp"
#include <memory>
#include <unordered_map>

#include "engine/runtime/resource/file_service.h"

namespace rtr {

enum class Phong_shader_feature {
    SHADOWS,
    ALBEDO_MAP,
    ALPHA_MAP,
    NORMAL_MAP,
    HEIGHT_MAP,
    SPECULAR_MAP,
    MAX_FEATURES,
};

template<>
inline constexpr const char* shader_feature_to_defines<Phong_shader_feature>(Phong_shader_feature feature) {
    switch (feature) {
        case Phong_shader_feature::SHADOWS:
            return "ENABLE_SHADOWS";
        case Phong_shader_feature::ALBEDO_MAP:
            return "ENABLE_ALBEDO_MAP";
        case Phong_shader_feature::ALPHA_MAP:
            return "ENABLE_ALPHA_MAP";
        case Phong_shader_feature::NORMAL_MAP:
            return "ENABLE_NORMAL_MAP";
        case Phong_shader_feature::HEIGHT_MAP:
            return "ENABLE_HEIGHT_MAP";
        case Phong_shader_feature::SPECULAR_MAP:
            return "ENABLE_SPECULAR_MAP";
        default:
            return "UNKNOWN";
    }
}

class Phong_shader : public Shader<Phong_shader_feature> {
public:
    Phong_shader() : Shader(
        "phong_shader", 
        std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> {
            {Shader_type::VERTEX, Shader_code::create(Shader_type::VERTEX, 
                Shader_code::load_shader_code(
                    File_ser::get_instance()->get_absolute_path("assets/shader/phong.vert")))},
            {Shader_type::FRAGMENT, Shader_code::create(Shader_type::FRAGMENT, 
                Shader_code::load_shader_code(
                    File_ser::get_instance()->get_absolute_path("assets/shader/phong.frag")))}
        }, 
        std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> {
            {"model", Uniform_entry<glm::mat4>::create(glm::mat4(1.0))},
            {"transparency", Uniform_entry<float>::create(1.0f)},
            {"ka", Uniform_entry<glm::vec3>::create(glm::vec3(0.1, 0.1, 0.1))},
            {"kd", Uniform_entry<glm::vec3>::create(glm::vec3(0.5, 0.5, 0.5))},
            {"ks", Uniform_entry<glm::vec3>::create(glm::vec3(1.0, 1.0, 1.0))},
            {"shininess", Uniform_entry<float>::create(32.0f)}
        },
        std::unordered_map<Phong_shader_feature, std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>> {
            {
                Phong_shader_feature::HEIGHT_MAP, 
                std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> {
                    {"parallax_scale", Uniform_entry<float>::create(0.05f)},
                    {"parallax_layer_count", Uniform_entry<float>::create(10.0f)}
                }
            },
            {
                Phong_shader_feature::SHADOWS,
                std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> {
                    {"shadow_bias", Uniform_entry<float>::create(1.0f)},
                    {"light_size", Uniform_entry<float>::create(0)},
                    {"pcf_radius", Uniform_entry<float>::create(5)},
                    {"pcf_tightness", Uniform_entry<float>::create(100)},
                    {"pcf_sample_count", Uniform_entry<int>::create(100)},
                }
            }
        },
        std::unordered_map<Phong_shader_feature, std::unordered_set<Phong_shader_feature>> {
            {Phong_shader_feature::HEIGHT_MAP, {Phong_shader_feature::NORMAL_MAP}}
        }
    ) {}

    ~Phong_shader() = default;
    static std::shared_ptr<Phong_shader> create() {
        return std::make_shared<Phong_shader>();
    }
};

struct Shadow_settings {
    bool enable_csm{false};
    float shadow_bias{0.005f};
    float light_size{0.7f};
    float pcf_radius{0.75f};
    float pcf_tightness{1.2f};
    int pcf_sample_count{18};

    Shadow_settings() = default;

    static std::shared_ptr<Shadow_settings> create() {
        return std::make_shared<Shadow_settings>();
    }

    void modify_shader_uniform(const std::shared_ptr<RHI_shader_program>& shader_program) {
        shader_program->modify_uniform("shadow_bias", shadow_bias);
        shader_program->modify_uniform("light_size", light_size);
        shader_program->modify_uniform("pcf_radius", pcf_radius);
        shader_program->modify_uniform("pcf_tightness", pcf_tightness);
        shader_program->modify_uniform("pcf_sample_count", pcf_sample_count);
    }
};

struct Phong_material_settings {
    float transparency{1.0f};
    glm::vec3 ka = glm::vec3(0.1f);    
    glm::vec3 kd = glm::vec3(0.7f);
    glm::vec3 ks = glm::vec3(0.5f);    
    float shininess = 32.0f;    
    static std::shared_ptr<Phong_material_settings> create() {
        return std::make_shared<Phong_material_settings>();
    }

    void modify_shader_uniform(const std::shared_ptr<RHI_shader_program>& shader_program) {
        shader_program->modify_uniform("transparency", transparency);
        shader_program->modify_uniform("ka", ka);
        shader_program->modify_uniform("kd", kd);
        shader_program->modify_uniform("ks", ks);
        shader_program->modify_uniform("shininess", shininess);
    }
};

struct Parallax_settings {
    float parallax_scale = 0.05f;
    float parallax_layer_count = 10.0f;
    static std::shared_ptr<Parallax_settings> create() {
        return std::make_shared<Parallax_settings>();
    }
    void modify_shader_uniform(const std::shared_ptr<RHI_shader_program>& shader_program) {
        shader_program->modify_uniform("parallax_scale", parallax_scale);
        shader_program->modify_uniform("parallax_layer_count", parallax_layer_count);
    }
};

struct Phong_texture_settings {
    std::shared_ptr<Texture> albedo_map{};
    std::shared_ptr<Texture> specular_map{};
    std::shared_ptr<Texture> normal_map{};
    std::shared_ptr<Texture> alpha_map{};
    std::shared_ptr<Texture> height_map{};

    static std::shared_ptr<Phong_texture_settings> create() {
        return std::make_shared<Phong_texture_settings>();
    }
};


class Phong_material : public Material {
protected:
    inline static std::shared_ptr<Phong_shader> s_phong_shader{};

public:
    std::shared_ptr<Shadow_settings> shadow_settings{};
    std::shared_ptr<Phong_texture_settings> phong_texture_settings{};
    std::shared_ptr<Phong_material_settings> phong_material_settings{};
    std::shared_ptr<Parallax_settings> parallax_settings{};
    
    Phong_material() : Material(Material_type::PHONG) {}
    ~Phong_material() = default;

    std::shared_ptr<Shader_program> get_shader_program() override {
        return phong_shader()->get_shader_variant(
            get_shader_feature_set()
        );
    }

    Phong_shader::Shader_feature_set get_shader_feature_set() const { 
        Phong_shader::Shader_feature_set feature_set{};

        if (phong_texture_settings->albedo_map) {
            feature_set.set(static_cast<size_t>(Phong_shader_feature::ALBEDO_MAP));
        }

        if (phong_texture_settings->specular_map) {
            feature_set.set(static_cast<size_t>(Phong_shader_feature::SPECULAR_MAP));
        }

        if (phong_texture_settings->normal_map) {
            feature_set.set(static_cast<size_t>(Phong_shader_feature::NORMAL_MAP));
        }

        if (phong_texture_settings->alpha_map) {
            feature_set.set(static_cast<size_t>(Phong_shader_feature::ALPHA_MAP));
        }

        if (phong_texture_settings->height_map && parallax_settings) {
            feature_set.set(static_cast<size_t>(Phong_shader_feature::HEIGHT_MAP));
        }
        
        if (shadow_settings) {
            feature_set.set(static_cast<size_t>(Phong_shader_feature::SHADOWS));
        }

        return feature_set;
    }

    std::unordered_map<unsigned int, std::shared_ptr<Texture>> get_texture_map() override {
        std::unordered_map<unsigned int, std::shared_ptr<Texture>> texture_map{};
            if (phong_texture_settings->albedo_map) {
                texture_map[0] = phong_texture_settings->albedo_map;
            }
            if (phong_texture_settings->specular_map) {
                texture_map[1] = phong_texture_settings->specular_map;
            }
            if (phong_texture_settings->normal_map) {
                texture_map[2] = phong_texture_settings->normal_map;
            }
            if (phong_texture_settings->alpha_map) {
                texture_map[3] = phong_texture_settings->alpha_map;
            }
            if (phong_texture_settings->height_map && parallax_settings) {
                texture_map[4] = phong_texture_settings->height_map;
            }
            return texture_map;
    }

    Pipeline_state get_pipeline_state() const override {
        if (phong_material_settings->transparency < 1.0 || phong_texture_settings->alpha_map) {
            return Pipeline_state::translucent_pipeline_state();
        } else {
            return Pipeline_state::opaque_pipeline_state();
        }
    }

    void modify_shader_uniform(const std::shared_ptr<RHI_shader_program>& shader_program) override {
        phong_material_settings->modify_shader_uniform(shader_program);

        if (phong_texture_settings->height_map) 
            parallax_settings->modify_shader_uniform(shader_program);

        if (shadow_settings)
            shadow_settings->modify_shader_uniform(shader_program);
    }

    static std::shared_ptr<Phong_material> create() {
        return std::make_shared<Phong_material>();
    }

    static std::shared_ptr<Phong_shader> phong_shader() {
        if (!s_phong_shader) {
            s_phong_shader = Phong_shader::create();
        }
        return s_phong_shader;
    }
  
};

}