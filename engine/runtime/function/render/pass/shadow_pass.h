#pragma once

#include "engine/runtime/context/swap/renderable_object.h"
#include "engine/runtime/function/render/frontend/frame_buffer.h"
#include "engine/runtime/function/render/material/shadow/shadow_caster_material.h"
#include "engine/runtime/function/render/frontend/texture.h"
#include "engine/runtime/function/render/pass/base_pass.h"

#include <memory>
#include <string>
#include <vector>

namespace rtr {
    
class Shadow_pass : public Base_pass {
public:

    struct Execution_context {
        std::vector<Swap_shadow_caster_renderable_object> shadow_caster_swap_objects{};
    };

    struct Resource_flow {
        std::shared_ptr<Texture> shadow_map_out{};
    };

    static std::shared_ptr<Shadow_pass> create(
        RHI_global_resource& rhi_global_resource
    ) {
        return std::make_shared<Shadow_pass>(rhi_global_resource);
    }

protected:
    std::shared_ptr<Shadow_caster_material> m_shadow_caster_material{}; 
    std::shared_ptr<Frame_buffer> m_frame_buffer{};

    Execution_context m_context{};
    Resource_flow m_resource_flow{};

public:

    Shadow_pass(
        RHI_global_resource& rhi_global_resource
    ) : Base_pass(rhi_global_resource), 
        m_shadow_caster_material(
            Shadow_caster_material::create()
        ) {}

    ~Shadow_pass() {}

    void set_resource_flow(const Resource_flow& flow) {
        m_resource_flow = flow;

        auto depth_attachment = m_resource_flow.shadow_map_out;
        m_frame_buffer = Frame_buffer::create(
            depth_attachment->rhi(m_rhi_global_resource.device)->width(), 
            depth_attachment->rhi(m_rhi_global_resource.device)->height(),
            std::vector<std::shared_ptr<Texture>> {}, 
            depth_attachment
        );
    }

    void set_context(const Execution_context& context) {
        m_context = context;
    }

    void excute() {

        m_rhi_global_resource.renderer->clear(m_frame_buffer->rhi(m_rhi_global_resource.device));

        m_rhi_global_resource.pipeline_state->state = m_shadow_caster_material->get_pipeline_state();
        m_rhi_global_resource.pipeline_state->apply();

        auto shader = m_shadow_caster_material->get_shader_program();

        for (auto& swap_object : m_context.shadow_caster_swap_objects) {
            
            auto geometry = swap_object.geometry;
            shader->rhi(m_rhi_global_resource.device)->modify_uniform("model", swap_object.model_matrix);
            shader->rhi(m_rhi_global_resource.device)->update_uniforms();

            m_rhi_global_resource.renderer->draw(
                shader->rhi(m_rhi_global_resource.device),
                geometry->rhi(m_rhi_global_resource.device),
                m_frame_buffer->rhi(m_rhi_global_resource.device)
            );
        }
    }
};


}