#pragma once

#include "engine/runtime/platform/rhi/rhi_shader_program.h"
#include <memory>

namespace rtr {

struct Shadow_setting {
    bool enable_csm{false};
    float shadow_bias{0.005f};
    float light_size{0.7f};
    float pcf_radius{0.05f};
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
}