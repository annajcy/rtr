#pragma once

#include "engine/runtime/function/render/render_material/material.h"
#include "engine/runtime/function/render/render_frontend/texture.h"

#include "engine/runtime/platform/rhi/rhi_pipeline_state.h"
#include "engine/runtime/platform/rhi/rhi_shader_program.h"
#include <memory>
#include <unordered_map>

#include "engine/runtime/resource/file_service.h"

namespace rtr {
class Gamma_shader : public Shader<None_shader_feature> {
public:
    Gamma_shader() : Shader(
        "gamma_shader",
        std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> {
            {Shader_type::VERTEX, Shader_code::create(Shader_type::VERTEX, 
                Shader_code::load_shader_code(
                    File_ser::get_instance()->get_absolute_path("assets/shader/gamma.vert")))},
            {Shader_type::FRAGMENT, Shader_code::create(Shader_type::FRAGMENT, 
                Shader_code::load_shader_code(
                    File_ser::get_instance()->get_absolute_path("assets/shader/gamma.frag")))}
        },
        std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> {}
    ) {}

    ~Gamma_shader() = default;
    static std::shared_ptr<Gamma_shader> create() {
        return std::make_shared<Gamma_shader>();
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
        
}