#pragma once

#include "engine/runtime/context/engine_tick_context.h"
#include "engine/runtime/core/frame_buffer.h"
#include "engine/runtime/core/geometry.h"
#include "engine/runtime/core/memory_buffer.h"
#include "engine/runtime/core/shader.h"
#include "engine/runtime/core/texture.h"
#include "engine/runtime/function/render/render_struct.h"
#include "engine/runtime/global/enum.h"
#include "engine/runtime/platform/rhi/rhi_buffer.h"
#include "engine/runtime/platform/rhi/rhi_device.h"
#include "engine/runtime/platform/rhi/rhi_pipeline_state.h"
#include "engine/runtime/platform/rhi/rhi_shader_program.h"
#include <memory>
#include <unordered_map>
#include <vector>

namespace rtr {

class Render_pipeline {
public:
    Render_pipeline() {}
    virtual ~Render_pipeline() {}

    virtual void execute(const Render_tick_context& tick_context) = 0;
    virtual void update_ubo(const Render_tick_context& tick_context) = 0;
};

class Test_render_pipeline : public Render_pipeline {
private:
    std::shared_ptr<RHI_device> m_device{};
    std::shared_ptr<RHI_renderer> m_renderer{};
    std::shared_ptr<RHI_frame_buffer> m_screen_frame_buffer{};
    std::shared_ptr<RHI_memory_buffer_binder> m_memory_binder{};
    std::shared_ptr<RHI_pipeline_state> m_pipeline_state{};

    std::shared_ptr<Uniform_buffer<Camera_ubo>> m_camera_ubo{};

    std::shared_ptr<Shader_program> m_gamma_shader{};
    std::shared_ptr<Frame_buffer> m_frame_buffer{};
    std::shared_ptr<Geometry> m_screen_geometry{};
    
public:
    Test_render_pipeline(
        const std::shared_ptr<RHI_device>& device,
        const std::shared_ptr<RHI_renderer>& renderer,
        const std::shared_ptr<RHI_frame_buffer>& screen_frame_buffer,
        const std::shared_ptr<RHI_memory_buffer_binder>& memory_binder
    ) : m_device(device),
        m_renderer(renderer),
        m_screen_frame_buffer(screen_frame_buffer),
        m_memory_binder(memory_binder),
        m_pipeline_state(device->create_pipeline_state()),
        m_camera_ubo(Uniform_buffer<Camera_ubo>::create(Camera_ubo{})) {

        if (!m_camera_ubo->is_linked()) m_camera_ubo->link(m_device);
        m_memory_binder->bind_memory_buffer(m_camera_ubo->rhi_resource(), 0);

        m_gamma_shader = Shader_program::create("gamma", std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> {
            {Shader_type::VERTEX, Shader_code::create(Shader_type::VERTEX, Shader::get_shader_code_from_url("assets/shader/gamma.vert"))},
            {Shader_type::FRAGMENT, Shader_code::create(Shader_type::FRAGMENT, Shader::get_shader_code_from_url("assets/shader/gamma.frag"))}
        }, std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> {});
        m_gamma_shader->link(m_device);

        m_frame_buffer = Frame_buffer::create(
            m_screen_frame_buffer->window()->width(), 
            m_screen_frame_buffer->window()->height(),
            std::vector<std::shared_ptr<Texture>> {
                Texture_color_attachment::create(
                    m_screen_frame_buffer->window()->width(),
                    m_screen_frame_buffer->window()->height()
                )
            },
            Texture_depth_attachment::create(
                m_screen_frame_buffer->window()->width(),
                m_screen_frame_buffer->window()->height()
            )
        );
        m_frame_buffer->link(m_device);

        m_screen_geometry = Geometry::create_screen_plane();
        m_screen_geometry->link(m_device);
    
    }

    ~Test_render_pipeline() {}

    void execute(const Render_tick_context& tick_context) override {
        m_renderer->clear(m_frame_buffer->rhi_resource());

        //draw skybox
        if (tick_context.render_swap_data.has_skybox) {

            auto mat = tick_context.render_swap_data.skybox.skybox_material;

            m_pipeline_state->pipeline_state = mat->get_pipeline_state();
            m_pipeline_state->apply();

            auto tex_map = mat->get_texture_map();
            for (auto& [slot, tex] : tex_map) {
                if (!tex->is_linked()) tex->link(m_device);
                tex->rhi_resource()->bind_to_unit(slot);
            }

            auto shader = mat->get_shader_program();
            if (!shader->is_linked()) shader->link(m_device);

            auto geo = tick_context.render_swap_data.skybox.skybox_geometry;
            if (!geo->is_linked()) geo->link(m_device);

            m_renderer->draw(
                shader->rhi_resource(),
                geo->rhi_resource(),
                m_frame_buffer->rhi_resource()
            );
        }

        //draw objects
        for (auto &go : tick_context.render_swap_data.render_objects) {
            auto mat = go.material;

            m_pipeline_state->pipeline_state = mat->get_pipeline_state();
            m_pipeline_state->apply();

            auto geo = go.geometry;
            if (!geo->is_linked()) geo->link(m_device);

            auto shader = mat->get_shader_program();
            if (!shader->is_linked()) shader->link(m_device);

            shader->rhi_resource()->modify_uniform("model", go.model_matrix);
            shader->rhi_resource()->update_uniforms();

            auto tex_map = mat->get_texture_map();
            for (auto& [slot, tex] : tex_map) {
                if (!tex->is_linked()) tex->link(m_device);
                tex->rhi_resource()->bind_to_unit(slot);
            }

            m_renderer->draw(
                shader->rhi_resource(), 
                geo->rhi_resource(), 
                m_frame_buffer->rhi_resource()
            );
        }

        m_renderer->clear(m_screen_frame_buffer);

        //draw gamma
        m_pipeline_state->pipeline_state = Pipeline_state::opaque_pipeline_state();
        m_pipeline_state->apply();

        m_frame_buffer->rhi_resource()->color_attachments()[0]->bind_to_unit(0);

        m_renderer->draw(
            m_gamma_shader->rhi_resource(),
            m_screen_geometry->rhi_resource(),
            m_screen_frame_buffer
        );
    }

    void update_ubo(const Render_tick_context& tick_context) override {
        m_camera_ubo->set_data(Camera_ubo{
            .view_matrix = tick_context.render_swap_data.camera.view_matrix,
            .projection_matrix = tick_context.render_swap_data.camera.projection_matrix,
            .camera_position = tick_context.render_swap_data.camera.camera_position
        });

        m_camera_ubo->push_to_rhi();
    }

};

class Forward_render_pipeline : public Render_pipeline {
public:
    Forward_render_pipeline() {}
    ~Forward_render_pipeline() {}

    void execute(const Render_tick_context& tick_context) override {

    }

    void update_ubo(const Render_tick_context& tick_context) override {
        
    }

};

class Deferred_render_pipeline : public Render_pipeline {
public:
    Deferred_render_pipeline() {}
    ~Deferred_render_pipeline() {}


    void execute(const Render_tick_context& tick_context) override {
        
    }

    void update_ubo(const Render_tick_context& tick_context) override {
        
    }

};

}