#pragma once

#include "engine/runtime/function/render/frontend/texture.h"
#include "engine/runtime/platform/rhi/rhi_shader_program.h"
#include <memory>

namespace rtr {

// Global shading settings

struct Shadow_setting {
    bool enable_csm{false};
    float shadow_bias{0.005f};
    float light_size{0.7f};
    float pcf_radius{0.02f};
    float pcf_tightness{1.2f};
    int pcf_sample_count{18};

    Shadow_setting() = default;

    static std::shared_ptr<Shadow_setting> create() {
        return std::make_shared<Shadow_setting>();
    }

    void modify_shader_uniform(const std::shared_ptr<RHI_shader_program>& shader_program) {
        shader_program->modify_uniform("shadow_bias", shadow_bias);
        shader_program->modify_uniform("light_size", light_size);
        shader_program->modify_uniform("pcf_radius", pcf_radius);
        shader_program->modify_uniform("pcf_tightness", pcf_tightness);
        shader_program->modify_uniform("pcf_sample_count", pcf_sample_count);
    }
};

struct Parallax_setting {
    float parallax_scale = 0.05f;
    float parallax_layer_count = 10.0f;
    static std::shared_ptr<Parallax_setting> create() {
        return std::make_shared<Parallax_setting>();
    }
    void modify_shader_uniform(const std::shared_ptr<RHI_shader_program>& shader_program) {
        shader_program->modify_uniform("parallax_scale", parallax_scale);
        shader_program->modify_uniform("parallax_layer_count", parallax_layer_count);
    }
};

// Phong material settings

struct Phong_material_setting {
    float transparency{1.0f};
    glm::vec3 ka = glm::vec3(0.1f);    
    glm::vec3 kd = glm::vec3(0.7f);
    glm::vec3 ks = glm::vec3(0.5f);    
    float shininess = 32.0f;    
    static std::shared_ptr<Phong_material_setting> create() {
        return std::make_shared<Phong_material_setting>();
    }

    void modify_shader_uniform(const std::shared_ptr<RHI_shader_program>& shader_program) {
        shader_program->modify_uniform("transparency", transparency);
        shader_program->modify_uniform("ka", ka);
        shader_program->modify_uniform("kd", kd);
        shader_program->modify_uniform("ks", ks);
        shader_program->modify_uniform("shininess", shininess);
    }
};

struct Phong_texture_setting {
    std::shared_ptr<Texture> albedo_map{};
    std::shared_ptr<Texture> specular_map{};
    std::shared_ptr<Texture> normal_map{};
    std::shared_ptr<Texture> alpha_map{};
    std::shared_ptr<Texture> height_map{};

    static std::shared_ptr<Phong_texture_setting> create() {
        return std::make_shared<Phong_texture_setting>();
    }
};


}