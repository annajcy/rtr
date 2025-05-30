#pragma once

#include "engine/runtime/function/render/frontend/frame_buffer.h"
#include "engine/runtime/function/render/frontend/geometry.h"
#include "engine/runtime/function/render/material/postprocess/gamma_material.h"
#include "engine/runtime/function/render/frontend/texture.h"
#include "engine/runtime/function/render/pass/base_pass.h"

#include <memory>

namespace rtr {
    
class Postprocess_pass : public Base_pass {
public:

    struct Execution_context {};

    struct Resource_flow {
        std::shared_ptr<Texture> texture_in{};
    };

    static std::shared_ptr<Postprocess_pass> create(
        RHI_global_resource& rhi_global_resource
    ) {
        return std::make_shared<Postprocess_pass>(rhi_global_resource);
    }

protected:
    std::shared_ptr<Gamma_material> m_gamma_material{};
    std::shared_ptr<Geometry> m_screen_geometry{};

    Resource_flow m_resource_input{};
    Execution_context m_context{};

    std::shared_ptr<Frame_buffer> m_frame_buffer{};

public:
    Postprocess_pass(
        RHI_global_resource& rhi_global_resource
    ) : Base_pass(rhi_global_resource) {
        m_gamma_material = Gamma_material::create();
        m_screen_geometry = Geometry::create_screen_plane();
    }

    ~Postprocess_pass() {}

    void set_resource_flow(const Resource_flow& input) {
        m_resource_input = input;
        m_gamma_material->screen_map = m_resource_input.texture_in;
    }

    void set_context(const Execution_context& context) {
        m_context = context;
    }

    void excute() override {
        m_rhi_global_resource.renderer->clear(m_rhi_global_resource.screen_buffer);

        auto shader = m_gamma_material->get_shader_program();
        auto texture_map = m_gamma_material->get_texture_map();
        for (auto &[location, tex] : texture_map) {
            tex->rhi(m_rhi_global_resource.device)->bind_to_unit(location);
        }

        m_rhi_global_resource.pipeline_state->state = m_gamma_material->get_pipeline_state();
        m_rhi_global_resource.pipeline_state->apply();

        m_rhi_global_resource.renderer->draw(
            shader->rhi(m_rhi_global_resource.device),
            m_screen_geometry->rhi(m_rhi_global_resource.device),
            m_rhi_global_resource.screen_buffer
        );
    }

};


}