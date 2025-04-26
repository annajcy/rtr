#pragma once


#include "engine/runtime/context/swap_struct.h"
#include "engine/runtime/function/render/object/frame_buffer.h"
#include "engine/runtime/function/render/object/material.h"
#include "engine/runtime/function/render/object/skybox.h"
#include "engine/runtime/function/render/object/texture.h"
#include "engine/runtime/function/render/core/render_object.h"
#include "engine/runtime/resource/resource_base.h"
#include <memory>
#include <string>
#include <vector>

namespace rtr {
    
class Render_pass {
protected:
    RHI_global_render_object& m_rhi_global_render_resource;

public:
    Render_pass(
        RHI_global_render_object& rhi_global_render_resource
    ) : m_rhi_global_render_resource(rhi_global_render_resource) {}

    virtual ~Render_pass() {}
    virtual void excute() = 0;
};

class Main_pass : public Render_pass {
public:

struct Execution_context {
    std::shared_ptr<Skybox> skybox{};
    std::vector<Swap_object> render_swap_objects{};
};

struct Resource_flow {
    std::shared_ptr<Texture> color_attachment_in_out{};
    std::shared_ptr<Texture> depth_attachment_in{};
};

protected:
    std::shared_ptr<Frame_buffer> m_frame_buffer{};
    Execution_context m_context{};
    Resource_flow m_resource_input{};
    
public:
    Main_pass(
        RHI_global_render_object& rhi_global_render_resource
    ) : Render_pass(rhi_global_render_resource) {}

    ~Main_pass() {}

    void set_resource_flow(const Resource_flow& input) {
        m_resource_input = input;
        int width = m_rhi_global_render_resource.window->width();
        int height = m_rhi_global_render_resource.window->height();

        auto color_attachment = m_resource_input.color_attachment_in_out;
        auto depth_attachment = m_resource_input.depth_attachment_in;

        m_frame_buffer = Frame_buffer::create(
            width, height, 
            std::vector<std::shared_ptr<Texture>> {
                color_attachment,
            }, depth_attachment
        );
        m_frame_buffer->link(m_rhi_global_render_resource.device);
    }

    void set_context(const Execution_context& context) {
        m_context = context;
    }

    static std::shared_ptr<Main_pass> create(
        RHI_global_render_object& rhi_global_render_resource
    ) {
        return std::make_shared<Main_pass>(rhi_global_render_resource);
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

            swap_object.material->modify_shader_uniform(shader->rhi_resource());
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
public:

    struct Resource_flow {
        std::shared_ptr<Texture> texture_in{};
    };

protected:
    std::shared_ptr<Gamma_material> m_gamma_material{};
    std::shared_ptr<Geometry> m_screen_geometry{};

    Resource_flow m_resource_input{};

    std::shared_ptr<Frame_buffer> m_frame_buffer{};

public:
    Gamma_pass(
        RHI_global_render_object& rhi_global_render_resource
    ) : Render_pass(rhi_global_render_resource) {
        m_gamma_material = Gamma_material::create();
        
        m_screen_geometry = Geometry::create_screen_plane();
        m_screen_geometry->link(m_rhi_global_render_resource.device);
    }

    ~Gamma_pass() {}

    static std::shared_ptr<Gamma_pass> create(
        RHI_global_render_object& rhi_global_render_resource
    ) {
        return std::make_shared<Gamma_pass>(rhi_global_render_resource);
    }

    void set_resource_flow(const Resource_flow& input) {
        m_resource_input = input;
        m_gamma_material->screen_map = m_resource_input.texture_in;
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