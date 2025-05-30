#pragma once

#include "engine/runtime/function/render/material/material.h"
#include "engine/runtime/function/render/frontend/texture.h"

#include "engine/runtime/platform/rhi/rhi_pipeline_state.h"
#include "engine/runtime/platform/rhi/rhi_shader_program.h"
#include "glm/fwd.hpp"
#include <memory>
#include <unordered_map>

#include "engine/runtime/resource/file_service.h"

namespace rtr {
    
class Shadow_caster_shader : public Shader<None_shader_feature> {
public:
    Shadow_caster_shader() : Shader(
        "shadow_caster_shader",
        std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> {
            {Shader_type::VERTEX, Shader_code::create(Shader_type::VERTEX, 
                Shader_code::load_shader_code(
                    File_ser::get_instance()->get_absolute_path("assets/shader/shadow_caster.vert")))},
            {Shader_type::FRAGMENT, 
                Shader_code::create(Shader_type::FRAGMENT, 
                    Shader_code::load_shader_code(
                        File_ser::get_instance()->get_absolute_path("assets/shader/shadow_caster.frag")))}
        },
        std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> {
            {"model", Uniform_entry<glm::mat4>::create(glm::mat4(1.0))}
        }
    ) {}

    ~Shadow_caster_shader() = default;

    static std::shared_ptr<Shadow_caster_shader> create() {
        return std::make_shared<Shadow_caster_shader>();
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