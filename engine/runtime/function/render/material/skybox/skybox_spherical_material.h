#pragma once

#include "engine/runtime/function/render/material/material.h"
#include "engine/runtime/function/render/frontend/texture.h"

#include "engine/runtime/platform/rhi/rhi_pipeline_state.h"
#include "engine/runtime/platform/rhi/rhi_shader_program.h"
#include <memory>
#include <unordered_map>

#include "engine/runtime/resource/file_service.h"

namespace rtr {
    
class Skybox_spherical_shader : public Shader<None_shader_feature> {
public:
    Skybox_spherical_shader() : Shader(
        "skybox_spherical_shader",
        std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> {
            {Shader_type::VERTEX, Shader_code::create(Shader_type::VERTEX, 
                Shader_code::load_shader_code(
                    File_ser::get_instance()->get_absolute_path("assets/shader/skybox_spherical.vert")))},
            {Shader_type::FRAGMENT, Shader_code::create(Shader_type::FRAGMENT, 
                Shader_code::load_shader_code(
                    File_ser::get_instance()->get_absolute_path("assets/shader/skybox_spherical.frag")))}
        },
        std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> {}
    ) {}

    ~Skybox_spherical_shader() = default;
    static std::shared_ptr<Skybox_spherical_shader> create() {
        return std::make_shared<Skybox_spherical_shader>();
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

}