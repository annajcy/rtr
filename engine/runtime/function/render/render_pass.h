#pragma once


#include "engine/runtime/context/swap_struct.h"
#include "engine/runtime/core/frame_buffer.h"
#include "engine/runtime/core/material.h"
#include "engine/runtime/core/skybox.h"
#include "engine/runtime/core/texture.h"
#include "engine/runtime/function/render/render_resource.h"
#include "engine/runtime/resource/resource_base.h"
#include <memory>
#include <string>
#include <vector>

namespace rtr {
    
class Render_pass {
protected:
    RHI_global_render_resource& m_rhi_global_render_resource;
    Resource_manager<std::string, Render_resource>& m_render_resource_manager;

public:
    Render_pass(
        RHI_global_render_resource& rhi_global_render_resource,
        Resource_manager<std::string, Render_resource>& render_resource_manager
    ) : m_rhi_global_render_resource(rhi_global_render_resource), 
        m_render_resource_manager(render_resource_manager) {}

    virtual ~Render_pass() {}
    virtual void excute() = 0;
};

class Main_pass : public Render_pass {
public:

struct Execution_context {
    std::shared_ptr<Skybox> skybox{};
    std::vector<Swap_object> render_swap_objects{};
};

protected:
    std::shared_ptr<Frame_buffer> m_frame_buffer{};
    Execution_context m_context{};
    
public:
    Main_pass(
        RHI_global_render_resource& rhi_global_render_resource,
        Resource_manager<std::string, Render_resource>& render_resource_manager
    ) : Render_pass(rhi_global_render_resource, render_resource_manager) {
        int width = rhi_global_render_resource.window->width();
        int height = rhi_global_render_resource.window->height();

        auto color_attachment = m_render_resource_manager.get<Texture>("main_color_attachment");
        auto depth_attachment =  m_render_resource_manager.get<Texture>("main_depth_attachment");

        m_frame_buffer = Frame_buffer::create(
            width, height, 
            std::vector<std::shared_ptr<Texture>> {
                color_attachment,
            }, depth_attachment
        );
        m_frame_buffer->link(m_rhi_global_render_resource.device);
    }

    ~Main_pass() {}

    void set_context(const Execution_context& context) {
        m_context = context;
    }

    static std::shared_ptr<Main_pass> create(
        RHI_global_render_resource& rhi_global_render_resource,
        Resource_manager<std::string, Render_resource>& render_resource_manager
    ) {
        return std::make_shared<Main_pass>(rhi_global_render_resource, render_resource_manager);
    }

    void excute() {

        m_rhi_global_render_resource.renderer->clear(m_frame_buffer->rhi_resource());

        if (m_context.skybox != nullptr) {

            auto shader = m_context.skybox->material()->get_shader_program();
            if (!shader->is_linked()) shader->link(m_rhi_global_render_resource.device);

            auto geometry = m_context.skybox->geometry();
            if (!geometry->is_linked()) geometry->link(m_rhi_global_render_resource.device);

            auto texture_map = m_context.skybox->material()->get_texture_map();
            for (auto &[location, tex] : texture_map) {
                if (!tex->is_linked()) tex->link(m_rhi_global_render_resource.device);
                tex->rhi_resource()->bind_to_unit(location);
            }

            m_rhi_global_render_resource.pipeline_state->pipeline_state = m_context.skybox->material()->get_pipeline_state();
            m_rhi_global_render_resource.pipeline_state->apply();

            m_rhi_global_render_resource.renderer->draw(
                shader->rhi_resource(),
                geometry->rhi_resource(),
                m_frame_buffer->rhi_resource()
            );
        }

        for (auto& swap_object : m_context.render_swap_objects) {
            auto shader = swap_object.material->get_shader_program();
            if (!shader->is_linked()) shader->link(m_rhi_global_render_resource.device);

            auto geometry = swap_object.geometry;
            if (!geometry->is_linked()) geometry->link(m_rhi_global_render_resource.device);

            auto texture_map = swap_object.material->get_texture_map();
            for (auto &[location, tex] : texture_map) {
                if (!tex->is_linked()) tex->link(m_rhi_global_render_resource.device);
                tex->rhi_resource()->bind_to_unit(location);
            }

            m_rhi_global_render_resource.pipeline_state->pipeline_state = swap_object.material->get_pipeline_state();
            m_rhi_global_render_resource.pipeline_state->apply();

            shader->rhi_resource()->modify_uniform("model", swap_object.model_matrix);
            shader->rhi_resource()->update_uniforms();

            m_rhi_global_render_resource.renderer->draw(
                shader->rhi_resource(),
                geometry->rhi_resource(),
                m_frame_buffer->rhi_resource()
            );
        }
    }

};

class Gamma_pass : public Render_pass {
protected:
    std::shared_ptr<Gamma_material> m_gamma_material{};
    std::shared_ptr<Geometry> m_screen_geometry{};

public:
    Gamma_pass(
        RHI_global_render_resource& rhi_global_render_resource,
        Resource_manager<std::string, Render_resource>& render_resource_manager
    ) : Render_pass(rhi_global_render_resource, render_resource_manager) {
        m_gamma_material = Gamma_material::create();
        m_gamma_material->screen_map = m_render_resource_manager.get<Texture>("main_color_attachment");
        
        m_screen_geometry = Geometry::create_screen_plane();
        m_screen_geometry->link(m_rhi_global_render_resource.device);
    }

    ~Gamma_pass() {}

    static std::shared_ptr<Gamma_pass> create(
        RHI_global_render_resource& rhi_global_render_resource,
        Resource_manager<std::string, Render_resource>& render_resource_manager
    ) {
        return std::make_shared<Gamma_pass>(rhi_global_render_resource, render_resource_manager);
    }

    void excute() {
        m_rhi_global_render_resource.renderer->clear(m_rhi_global_render_resource.screen_buffer);

        auto shader = m_gamma_material->get_shader_program();
        if (!shader->is_linked()) shader->link(m_rhi_global_render_resource.device);

        auto texture_map = m_gamma_material->get_texture_map();
        for (auto &[location, tex] : texture_map) {
            if (!tex->is_linked()) tex->link(m_rhi_global_render_resource.device);
            tex->rhi_resource()->bind_to_unit(location);
        }

        m_rhi_global_render_resource.pipeline_state->pipeline_state = m_gamma_material->get_pipeline_state();
        m_rhi_global_render_resource.pipeline_state->apply();

        m_rhi_global_render_resource.renderer->draw(
            shader->rhi_resource(),
            m_screen_geometry->rhi_resource(),
            m_rhi_global_render_resource.screen_buffer
        );
    }

};



}