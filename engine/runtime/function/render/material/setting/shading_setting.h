#pragma once

#include "engine/runtime/function/render/frontend/texture.h"
#include "engine/runtime/platform/rhi/rhi_shader_program.h"
#include <memory>

namespace rtr {

struct Base_shading_setting : public Base_setting {
    virtual ~Base_shading_setting() = default;
};

struct Phong_shading_setting : public Base_shading_setting {
    float transparency{1.0f};
    glm::vec3 ka = glm::vec3(0.1f);    
    glm::vec3 kd = glm::vec3(0.7f);
    glm::vec3 ks = glm::vec3(0.5f);    
    float shininess {32.0f};

    std::shared_ptr<Texture> albedo_map{};
    std::shared_ptr<Texture> specular_map{};
    std::shared_ptr<Texture> normal_map{};
    std::shared_ptr<Texture> alpha_map{};
    std::shared_ptr<Texture> height_map{};

    Phong_shading_setting() = default;
    static std::shared_ptr<Phong_shading_setting> create() {
        return std::make_shared<Phong_shading_setting>();
    }

    void modify_shader_uniform(const std::shared_ptr<RHI_shader_program>& shader_program) override {
        shader_program->modify_uniform("transparency", transparency);
        shader_program->modify_uniform("ka", ka);
        shader_program->modify_uniform("kd", kd);
        shader_program->modify_uniform("ks", ks);
        shader_program->modify_uniform("shininess", shininess);
    }

};

struct PBR_shading_setting : public Base_shading_setting {

};






}