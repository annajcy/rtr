#pragma once


#include "engine/runtime/context/swap/renderable_object.h"
#include "engine/runtime/function/render/frontend/frame_buffer.h"
#include "engine/runtime/function/render/frontend/texture.h"
#include "engine/runtime/function/render/pass/base_pass.h"
#include "engine/runtime/function/render/utils/skybox.h"

#include <memory>
#include <string>
#include <vector>

namespace rtr {
    
class Main_pass : public Base_pass {
public:

    struct Execution_context {
        std::shared_ptr<Skybox> skybox{};
        std::vector<Swap_renderable_object> render_swap_objects{};
    };

    struct Resource_flow {
        std::shared_ptr<Texture> color_attachment_out{};
        std::shared_ptr<Texture> depth_attachment_out{};
        std::shared_ptr<Texture> shadow_map_in{};
    };

    static std::shared_ptr<Main_pass> create(
        RHI_global_resource& rhi_global_resource
    ) {
        return std::make_shared<Main_pass>(rhi_global_resource);
    }

protected:
    std::shared_ptr<Frame_buffer> m_frame_buffer{};
    Execution_context m_context{};
    Resource_flow m_resource_flow{};
    
public:
    Main_pass(
        RHI_global_resource& rhi_global_resource
    ) : Base_pass(rhi_global_resource) {}

    ~Main_pass() {}

    void set_resource_flow(const Resource_flow& flow) {
        m_resource_flow = flow;
        int width = m_rhi_global_resource.window->width();
        int height = m_rhi_global_resource.window->height();

        auto color_attachment = m_resource_flow.color_attachment_out;
        auto depth_attachment = m_resource_flow.depth_attachment_out;

        m_frame_buffer = Frame_buffer::create(
            width, height, 
            std::vector<std::shared_ptr<Texture>> {
                color_attachment,
            }, depth_attachment
        );
    }

    void set_context(const Execution_context& context) {
        m_context = context;
    }

    void excute() override {
        
        m_rhi_global_resource.renderer->clear(m_frame_buffer->rhi(m_rhi_global_resource.device));

        if (m_context.skybox != nullptr) {

            auto shader = m_context.skybox->material()->get_shader_program();
            auto geometry = m_context.skybox->geometry();
            auto texture_map = m_context.skybox->material()->get_texture_map();
        
            for (auto &[location, tex] : texture_map) {
                tex->rhi(m_rhi_global_resource.device)->bind_to_unit(location);
            }

            m_rhi_global_resource.pipeline_state->state = m_context.skybox->material()->get_pipeline_state();
            m_rhi_global_resource.pipeline_state->apply();

            m_rhi_global_resource.renderer->draw(
                shader->rhi(m_rhi_global_resource.device),
                geometry->rhi(m_rhi_global_resource.device),
                m_frame_buffer->rhi(m_rhi_global_resource.device)
            );
        }

        m_resource_flow.shadow_map_in->rhi(m_rhi_global_resource.device)->bind_to_unit(5);

        for (auto& swap_object : m_context.render_swap_objects) {
            auto shader = swap_object.material->get_shader_program();
            auto geometry = swap_object.geometry;

            auto texture_map = swap_object.material->get_texture_map();
            for (auto &[location, tex] : texture_map) {
                tex->rhi(m_rhi_global_resource.device)->bind_to_unit(location);
            }

            m_rhi_global_resource.pipeline_state->state = swap_object.material->get_pipeline_state();
            m_rhi_global_resource.pipeline_state->apply();

            swap_object.material->modify_shader_uniform(shader->rhi(m_rhi_global_resource.device));
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