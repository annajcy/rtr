#pragma once

#include "engine/runtime/function/render/frontend/texture.h"
#include "engine/runtime/platform/rhi/rhi_shader_program.h"
#include <memory>

namespace rtr {

struct Base_shadow_setting : public Base_setting {
    float shadow_bias{0.005f};

    Base_shadow_setting() = default;
    void modify_shader_uniform(const std::shared_ptr<RHI_shader_program>& shader_program) override {
        shader_program->modify_uniform("shadow_bias", shadow_bias);
    }
};

struct VSM_shadow_setting : public Base_shadow_setting {
    float vsm_moment_blur{0.5f};
    float vsm_min_variance {0.00002};
    float vsm_light_bleed_reduction {0.15};
    VSM_shadow_setting() = default;
    static std::shared_ptr<VSM_shadow_setting> create() {
        return std::make_shared<VSM_shadow_setting>();
    }
    void modify_shader_uniform(const std::shared_ptr<RHI_shader_program>& shader_program) override {
        shader_program->modify_uniform("shadow_bias", shadow_bias);
        shader_program->modify_uniform("vsm_moment_blur", vsm_moment_blur);
        shader_program->modify_uniform("vsm_min_variance", vsm_min_variance);
        shader_program->modify_uniform("vsm_light_bleed_reduction", vsm_light_bleed_reduction);
    }
};

struct VSSM_shadow_setting : public Base_shadow_setting {
    float vssm_light_size{50};
    float vssm_min_variance{0.00002}; 
    float vssm_light_bleed_reduction{0.15};

    VSSM_shadow_setting() = default;
    static std::shared_ptr<VSSM_shadow_setting> create() {
        return std::make_shared<VSSM_shadow_setting>();
    }
    void modify_shader_uniform(const std::shared_ptr<RHI_shader_program>& shader_program) override {
        shader_program->modify_uniform("shadow_bias", shadow_bias);
        shader_program->modify_uniform("vssm_light_size", vssm_light_size);
        shader_program->modify_uniform("vssm_min_variance", vssm_min_variance);
        shader_program->modify_uniform("vssm_light_bleed_reduction", vssm_light_bleed_reduction);
    }
};

struct PCF_shadow_setting : public Base_shadow_setting {
    float pcf_radius{0.2f};
    float pcf_tightness{1.2f};
    int pcf_sample_count{10};
    PCF_shadow_setting() = default;
    static std::shared_ptr<PCF_shadow_setting> create() {
        return std::make_shared<PCF_shadow_setting>();
    }
    void modify_shader_uniform(const std::shared_ptr<RHI_shader_program>& shader_program) override {
        shader_program->modify_uniform("shadow_bias", shadow_bias);
        shader_program->modify_uniform("pcf_radius", pcf_radius);
        shader_program->modify_uniform("pcf_tightness", pcf_tightness);
        shader_program->modify_uniform("pcf_sample_count", pcf_sample_count);
    }
};

struct PCSS_shadow_setting : public Base_shadow_setting {
    float pcss_light_size{0.5f};
    float pcss_radius{0.2f};
    float pcss_tightness{1.2f};
    int pcss_blocker_search_sample_count{10};
    int pcss_filter_sample_count{10};

    PCSS_shadow_setting() = default;
    static std::shared_ptr<PCSS_shadow_setting> create() {
        return std::make_shared<PCSS_shadow_setting>();
    }
    
    void modify_shader_uniform(const std::shared_ptr<RHI_shader_program>& shader_program) override {
        shader_program->modify_uniform("shadow_bias", shadow_bias);
        shader_program->modify_uniform("pcss_light_size", pcss_light_size);
        shader_program->modify_uniform("pcss_radius", pcss_radius);
        shader_program->modify_uniform("pcss_tightness", pcss_tightness);
        shader_program->modify_uniform("pcss_blocker_search_sample_count", pcss_blocker_search_sample_count);
        shader_program->modify_uniform("pcss_filter_sample_count", pcss_filter_sample_count);
    }
};


}