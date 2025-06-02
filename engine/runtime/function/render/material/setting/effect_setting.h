#pragma once

#include "engine/runtime/function/render/frontend/texture.h"
#include "engine/runtime/platform/rhi/rhi_shader_program.h"
#include <memory>

namespace rtr {

struct Parallax_setting : public Base_setting {
    float parallax_scale {0.05f};
    float parallax_layer_count {10.0f};
    static std::shared_ptr<Parallax_setting> create() {
        return std::make_shared<Parallax_setting>();
    }
    void modify_shader_uniform(const std::shared_ptr<RHI_shader_program>& shader_program) {
        shader_program->modify_uniform("parallax_scale", parallax_scale);
        shader_program->modify_uniform("parallax_layer_count", parallax_layer_count);
    }
};




}