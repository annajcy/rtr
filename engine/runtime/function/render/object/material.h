#pragma once

#include "engine/runtime/function/render/object/shader.h"
#include "engine/runtime/function/render/object/texture.h"
#include "engine/runtime/function/render/core/render_object.h"
#include "engine/runtime/global/enum.h"
#include "engine/runtime/platform/rhi/rhi_pipeline_state.h"
#include "engine/runtime/platform/rhi/rhi_shader_program.h"
#include "glm/fwd.hpp"
#include <memory>
#include <unordered_map>

namespace rtr {

class Material : public Render_object {
    
protected:
    Material_type m_material_type{};
    std::shared_ptr<Shader> m_shader{}; 

public:
    Material(
        Material_type material_type, 
        const std::shared_ptr<Shader>& shader
    ) : Render_object(Render_object_type::MATERIAL),
        m_material_type(material_type),
        m_shader(shader) {}

    virtual ~Material() = default;

    Material_type material_type() const { return m_material_type; }
    const std::shared_ptr<Shader>& shader() const { return m_shader; }

    const std::shared_ptr<Shader_program> get_shader_program() const {
        auto shader_variant = m_shader->get_shader_variant(get_shader_feature_set());
        return shader_variant;
    }

    virtual Shader::Shader_feature_set get_shader_feature_set() const {
        Shader::Shader_feature_set feature_set{};
        return feature_set;
    }

    virtual Pipeline_state get_pipeline_state() const {
        return Pipeline_state::opaque_pipeline_state();
    }

    virtual std::unordered_map<unsigned int, std::shared_ptr<Texture>> get_texture_map() { return {}; }

    virtual void modify_shader_uniform(const std::shared_ptr<RHI_shader_program>& shader_program) {}
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
public:

    std::shared_ptr<Shadow_settings> shadow_settings{};
    std::shared_ptr<Phong_texture_settings> phong_texture_settings{};
    std::shared_ptr<Phong_material_settings> phong_material_settings{};
    std::shared_ptr<Parallax_settings> parallax_settings{};
    
    Phong_material(
        const std::shared_ptr<Phong_shader> &shader
    ) : Material(
        Material_type::PHONG,
        shader
    ) {}

    ~Phong_material() = default;

    Shader::Shader_feature_set get_shader_feature_set() const override { 
        Shader::Shader_feature_set feature_set{};

        if (phong_texture_settings->albedo_map) {
            feature_set.set(static_cast<size_t>(Shader_feature::ALBEDO_MAP));
        }

        if (phong_texture_settings->specular_map) {
            feature_set.set(static_cast<size_t>(Shader_feature::SPECULAR_MAP));
        }

        if (phong_texture_settings->normal_map) {
            feature_set.set(static_cast<size_t>(Shader_feature::NORMAL_MAP));
        }

        if (phong_texture_settings->alpha_map) {
            feature_set.set(static_cast<size_t>(Shader_feature::ALPHA_MAP));
        }

        if (phong_texture_settings->height_map && parallax_settings) {
            feature_set.set(static_cast<size_t>(Shader_feature::HEIGHT_MAP));
        }
        
        if (shadow_settings) {
            feature_set.set(static_cast<size_t>(Shader_feature::SHADOWS));
        }

        if (shadow_settings->enable_csm) {
            feature_set.set(static_cast<size_t>(Shader_feature::CSM_SHADOWS));
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

    static std::shared_ptr<Phong_material> create(
        const std::shared_ptr<Phong_shader> &shader
    ) {
        return std::make_shared<Phong_material>(
            shader
        );
    }

    void modify_shader_uniform(const std::shared_ptr<RHI_shader_program>& shader_program) override {
        phong_material_settings->modify_shader_uniform(shader_program);

        if (phong_texture_settings->height_map) 
            parallax_settings->modify_shader_uniform(shader_program);

        if (shadow_settings)
            shadow_settings->modify_shader_uniform(shader_program);
    }
};

class Skybox_cubemap_material : public Material {
public:
    std::shared_ptr<Texture> cube_map{};
    Skybox_cubemap_material(
        const std::shared_ptr<Skybox_cubemap_shader> &shader
    ) : Material(
        Material_type::SKYBOX_CUBEMAP, 
        shader
    ) {}
    ~Skybox_cubemap_material() = default;

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

    static std::shared_ptr<Skybox_cubemap_material> create(
        const std::shared_ptr<Skybox_cubemap_shader> &shader
    ) {
        return std::make_shared<Skybox_cubemap_material>(shader);
    }
};

class Skybox_spherical_material : public Material {
public:
    std::shared_ptr<Texture> spherical_map{};

    Skybox_spherical_material(
        const std::shared_ptr<Skybox_spherical_shader> &shader
    ) : Material(
        Material_type::SKYBOX_SPHERICAL, 
        shader
    ) {}

    ~Skybox_spherical_material() = default;

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

    static std::shared_ptr<Skybox_spherical_material> create(
        const std::shared_ptr<Skybox_spherical_shader> &shader
    ) {
        return std::make_shared<Skybox_spherical_material>(
            shader
        );
    }

};

class Gamma_material : public Material {
public:
    std::shared_ptr<Texture> screen_map{};

    Gamma_material(
        const std::shared_ptr<Gamma_shader> &shader
    ) : Material(
        Material_type::GAMMA,
        shader
    ) {}

    ~Gamma_material() = default;

    std::unordered_map<unsigned int, std::shared_ptr<Texture>> get_texture_map() override {
        if (screen_map) {
            return {
                {0, screen_map}
            };
        }
        return {};
    }

    static std::shared_ptr<Gamma_material> create(
        const std::shared_ptr<Gamma_shader> &shader
    ) {
        return std::make_shared<Gamma_material>(
            shader
        );
    }
};

class Shadow_caster_material : public Material {
public:
    Shadow_caster_material(
        const std::shared_ptr<Shadow_caster_shader> &shader
    ) : Material(
        Material_type::SHADOW_CASTER,
        shader
    ) {}
    
    ~Shadow_caster_material() = default;

    static std::shared_ptr<Shadow_caster_material> create(
        const std::shared_ptr<Shadow_caster_shader> &shader
    ) {
        return std::make_shared<Shadow_caster_material>(
            shader
        );
    }

    Pipeline_state get_pipeline_state() const override {
        return Pipeline_state::shadow_pipeline_state();
    }

};

}