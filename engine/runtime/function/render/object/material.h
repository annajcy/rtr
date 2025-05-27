#pragma once

#include "engine/runtime/function/render/object/shader.h"
#include "engine/runtime/function/render/object/texture.h"
#include "engine/runtime/function/render/core/render_resource.h"

#include "engine/runtime/platform/rhi/rhi_pipeline_state.h"
#include "engine/runtime/platform/rhi/rhi_shader_program.h"
#include "glm/fwd.hpp"
#include <memory>
#include <unordered_map>

namespace rtr {

enum class Material_type {
    PHONG,
    SKYBOX_CUBEMAP,
    SKYBOX_SPHERICAL,
    GAMMA,
    SHADOW_CASTER
};

class Material : public Render_resource {
    
protected:
    Material_type m_material_type{};

public:
    Material(
        Material_type material_type
    ) : Render_resource(Render_resource_type::MATERIAL),
        m_material_type(material_type){}

    virtual ~Material() = default;

    Material_type material_type() const { return m_material_type; }

    virtual std::shared_ptr<Shader_program> get_shader_program() = 0;
    virtual std::unordered_map<unsigned int, std::shared_ptr<Texture>> get_texture_map() = 0;
    virtual Pipeline_state get_pipeline_state() const = 0;
    virtual void modify_shader_uniform(const std::shared_ptr<RHI_shader_program>& shader_program) = 0;
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

class Skybox_cubemap_material : public Material {
public:
    inline static std::shared_ptr<Skybox_cubemap_shader> s_skybox_cubemap_shader{};
    
    std::shared_ptr<Texture> cube_map{};

    Skybox_cubemap_material() : Material(Material_type::SKYBOX_CUBEMAP) {}
    ~Skybox_cubemap_material() = default;

    std::shared_ptr<Shader_program> get_shader_program() override {
        return skybox_cubemap_shader()->get_shader_program();
    }

    void modify_shader_uniform(const std::shared_ptr<RHI_shader_program>& shader_program) override {}

    std::unordered_map<unsigned int, std::shared_ptr<Texture>> get_texture_map() override {
        if (cube_map) {
            return {
                {0, cube_map}
            };
        }
        return {};
    }

    Pipeline_state get_pipeline_state() const override {
        return Pipeline_state::skybox_pipeline_state();
    }

    static std::shared_ptr<Skybox_cubemap_material> create() {
        return std::make_shared<Skybox_cubemap_material>();
    }

    static std::shared_ptr<Skybox_cubemap_shader> skybox_cubemap_shader() {
        if (!s_skybox_cubemap_shader) {
            s_skybox_cubemap_shader = Skybox_cubemap_shader::create();
        }
        return s_skybox_cubemap_shader;
    }
};

class Skybox_spherical_material : public Material {
public:
    inline static std::shared_ptr<Skybox_spherical_shader> s_skybox_spherical_shader{};
    std::shared_ptr<Texture> spherical_map{};

    Skybox_spherical_material() : Material(Material_type::SKYBOX_SPHERICAL) {}

    ~Skybox_spherical_material() = default;

    std::shared_ptr<Shader_program> get_shader_program() override { return skybox_spherical_shader()->get_shader_program(); }

    std::unordered_map<unsigned int, std::shared_ptr<Texture>> get_texture_map() override {
        if (spherical_map) {
            return {
                {0, spherical_map}
            };
        }
        return {};
    }

    Pipeline_state get_pipeline_state() const override {
        return Pipeline_state::skybox_pipeline_state();
    }

    void modify_shader_uniform(const std::shared_ptr<RHI_shader_program>& shader_program) override {}

    static std::shared_ptr<Skybox_spherical_material> create() {
        return std::make_shared<Skybox_spherical_material>();
    }

    static std::shared_ptr<Skybox_spherical_shader> skybox_spherical_shader() {
        if (!s_skybox_spherical_shader) {
            s_skybox_spherical_shader = Skybox_spherical_shader::create();
        }
        return s_skybox_spherical_shader;
    }

};

class Gamma_material : public Material {
protected:
    inline static std::shared_ptr<Gamma_shader> s_gamma_shader{};
public:
    std::shared_ptr<Texture> screen_map{};

    Gamma_material() : Material(Material_type::GAMMA) {}

    ~Gamma_material() = default;

    Pipeline_state get_pipeline_state() const override {
        return Pipeline_state::opaque_pipeline_state();
    }

    void modify_shader_uniform(const std::shared_ptr<RHI_shader_program>& shader_program) override {}

    std::unordered_map<unsigned int, std::shared_ptr<Texture>> get_texture_map() override {
        if (screen_map) {
            return {
                {0, screen_map}
            };
        }
        return {};
    }

    std::shared_ptr<Shader_program> get_shader_program() override {
        return gamma_shader()->get_shader_program();
    }

    static std::shared_ptr<Gamma_material> create() {
        return std::make_shared<Gamma_material>();
    }

    static std::shared_ptr<Gamma_shader> gamma_shader() {
        if (!s_gamma_shader) {
            s_gamma_shader = Gamma_shader::create();
        }
        return s_gamma_shader;
    }

    
};

class Shadow_caster_material : public Material {
protected:
    inline static std::shared_ptr<Shadow_caster_shader> s_shadow_caster_shader{};

public:
    Shadow_caster_material() : Material(
        Material_type::SHADOW_CASTER
    ) {}
    
    ~Shadow_caster_material() = default;

    Pipeline_state get_pipeline_state() const override {
        return Pipeline_state::shadow_pipeline_state();
    }

    std::shared_ptr<Shader_program> get_shader_program() override {
        return shadow_caster_shader()->get_shader_program();
    }

    std::unordered_map<unsigned int, std::shared_ptr<Texture>> get_texture_map() override {
        return {};
    }

    void modify_shader_uniform(const std::shared_ptr<RHI_shader_program>& shader_program) override {}

    static std::shared_ptr<Shadow_caster_material> create() {
        return std::make_shared<Shadow_caster_material>();
    }

    static std::shared_ptr<Shadow_caster_shader> shadow_caster_shader() {
        if (!s_shadow_caster_shader) {
            s_shadow_caster_shader = Shadow_caster_shader::create();
        }
        return s_shadow_caster_shader;
    }

};

}