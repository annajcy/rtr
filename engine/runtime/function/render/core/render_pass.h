#pragma once

#include "engine/runtime/context/swap_struct.h"
#include "engine/runtime/function/render/object/frame_buffer.h"
#include "engine/runtime/function/render/object/material.h"
#include "engine/runtime/function/render/object/shader.h"
#include "engine/runtime/function/render/object/skybox.h"
#include "engine/runtime/function/render/object/texture.h"
#include "engine/runtime/platform/rhi/opengl/rhi_error_opengl.h"

#include <memory>
#include <string>
#include <vector>

namespace rtr {
    
class Render_pass {
protected:
    RHI_global_resource& m_rhi_global_resource;

public:
    Render_pass(
        RHI_global_resource& rhi_global_resource
    ) : m_rhi_global_resource(rhi_global_resource) {}

    virtual ~Render_pass() {}
    virtual void excute() = 0;
};

class Shadow_pass : public Render_pass {
public:

    struct Execution_context {
        std::vector<Swap_shadow_caster_object> shadow_caster_swap_objects{};
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
    ) : Render_pass(rhi_global_resource), 
        m_shadow_caster_material(
            Shadow_caster_material::create(Shadow_caster_shader::create())
        ) {}

    ~Shadow_pass() {}

    void set_resource_flow(const Resource_flow& flow) {
        m_resource_flow = flow;

        auto depth_attachment = m_resource_flow.shadow_map_out;
        if (!depth_attachment->is_linked()) depth_attachment->link(m_rhi_global_resource.device);
        
        m_frame_buffer = Frame_buffer::create(
            depth_attachment->rhi_resource()->width(), depth_attachment->rhi_resource()->width(),
            std::vector<std::shared_ptr<Texture>> {}, 
            depth_attachment
        );

        m_frame_buffer->link(m_rhi_global_resource.device);
    }

    void set_context(const Execution_context& context) {
        m_context = context;
    }

    void excute() {

        m_rhi_global_resource.renderer->clear(m_frame_buffer->rhi_resource());

        m_rhi_global_resource.pipeline_state->state = m_shadow_caster_material->get_pipeline_state();
        m_rhi_global_resource.pipeline_state->apply();

        auto shader = m_shadow_caster_material->get_shader_program();
        if (!shader->is_linked()) shader->link(m_rhi_global_resource.device);

        for (auto& swap_object : m_context.shadow_caster_swap_objects) {
            
            auto geometry = swap_object.geometry;
            if (!geometry->is_linked()) geometry->link(m_rhi_global_resource.device);

            shader->rhi_resource()->modify_uniform("model", swap_object.model_matrix);
            shader->rhi_resource()->update_uniforms();

            m_rhi_global_resource.renderer->draw(
                shader->rhi_resource(),
                geometry->rhi_resource(),
                m_frame_buffer->rhi_resource()
            );
        }
    }
};

class Main_pass : public Render_pass {
public:

    struct Execution_context {
        std::shared_ptr<Skybox> skybox{};
        std::vector<Swap_object> render_swap_objects{};
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
    ) : Render_pass(rhi_global_resource) {}

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
        m_frame_buffer->link(m_rhi_global_resource.device);
    }

    void set_context(const Execution_context& context) {
        m_context = context;
    }

    void excute() override {
        
        m_rhi_global_resource.renderer->clear(m_frame_buffer->rhi_resource());

        if (m_context.skybox != nullptr) {

            auto shader = m_context.skybox->material()->get_shader_program();
            if (!shader->is_linked()) shader->link(m_rhi_global_resource.device);

            auto geometry = m_context.skybox->geometry();
            if (!geometry->is_linked()) geometry->link(m_rhi_global_resource.device);

            auto texture_map = m_context.skybox->material()->get_texture_map();
            gl_check_error();
            for (auto &[location, tex] : texture_map) {
                if (!tex->is_linked()) tex->link(m_rhi_global_resource.device);
                tex->rhi_resource()->bind_to_unit(location);
            }

            m_rhi_global_resource.pipeline_state->state = m_context.skybox->material()->get_pipeline_state();
            m_rhi_global_resource.pipeline_state->apply();

            m_rhi_global_resource.renderer->draw(
                shader->rhi_resource(),
                geometry->rhi_resource(),
                m_frame_buffer->rhi_resource()
            );
        }

        m_resource_flow.shadow_map_in->rhi_resource()->bind_to_unit(5);

        for (auto& swap_object : m_context.render_swap_objects) {
            auto shader = swap_object.material->get_shader_program();
            if (!shader->is_linked()) shader->link(m_rhi_global_resource.device);

            auto geometry = swap_object.geometry;
            if (!geometry->is_linked()) geometry->link(m_rhi_global_resource.device);

            auto texture_map = swap_object.material->get_texture_map();
            for (auto &[location, tex] : texture_map) {
                if (!tex->is_linked()) tex->link(m_rhi_global_resource.device);
                tex->rhi_resource()->bind_to_unit(location);
            }

            m_rhi_global_resource.pipeline_state->state = swap_object.material->get_pipeline_state();
            m_rhi_global_resource.pipeline_state->apply();

            swap_object.material->modify_shader_uniform(shader->rhi_resource());
            shader->rhi_resource()->modify_uniform("model", swap_object.model_matrix);
            shader->rhi_resource()->update_uniforms();

            m_rhi_global_resource.renderer->draw(
                shader->rhi_resource(),
                geometry->rhi_resource(),
                m_frame_buffer->rhi_resource()
            );
        }
    }
};

class Postprocess_pass : public Render_pass {
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
    ) : Render_pass(rhi_global_resource) {
        m_gamma_material = Gamma_material::create(Gamma_shader::create());
        
        m_screen_geometry = Geometry::create_screen_plane();
        m_screen_geometry->link(m_rhi_global_resource.device);
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
        if (!shader->is_linked()) shader->link(m_rhi_global_resource.device);

        auto texture_map = m_gamma_material->get_texture_map();
        for (auto &[location, tex] : texture_map) {
            if (!tex->is_linked()) tex->link(m_rhi_global_resource.device);
            tex->rhi_resource()->bind_to_unit(location);
        }

        m_rhi_global_resource.pipeline_state->state = m_gamma_material->get_pipeline_state();
        m_rhi_global_resource.pipeline_state->apply();

        m_rhi_global_resource.renderer->draw(
            shader->rhi_resource(),
            m_screen_geometry->rhi_resource(),
            m_rhi_global_resource.screen_buffer
        );
    }

};



}