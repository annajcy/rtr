#pragma once

#include "engine/runtime/context/engine_tick_context.h"
#include "engine/runtime/core/memory_buffer.h"
#include "engine/runtime/core/texture.h"
#include "engine/runtime/function/render/render_pass.h"
#include "engine/runtime/function/render/render_resource.h"
#include "engine/runtime/function/render/render_struct.h"
#include "engine/runtime/resource/resource_base.h"
#include <memory>

namespace rtr {

class Render_pipeline {
protected:
    RHI_global_render_resource& m_rhi_global_render_resource;
    Resource_manager<std::string, Render_resource> m_render_resource_manager{};

public:
    Render_pipeline(RHI_global_render_resource& global_render_resource) : 
    m_rhi_global_render_resource(global_render_resource) {}

    virtual ~Render_pipeline() {}

    virtual void execute(const Render_tick_context& tick_context) = 0;
    virtual void update_ubo(const Render_tick_context& tick_context) = 0;
    virtual void init_render_resource() = 0;
    virtual void init_ubo() = 0;
    virtual void init_render_passes() = 0;
};


class Test_render_pipeline : public Render_pipeline {
private:
    std::shared_ptr<Uniform_buffer<Camera_ubo>> m_camera_ubo{};

    std::shared_ptr<Main_pass> m_main_pass{};
    std::shared_ptr<Gamma_pass> m_gamma_pass{};
    
public:
    Test_render_pipeline (RHI_global_render_resource& global_render_resource) : Render_pipeline(global_render_resource) {
        init_render_resource();
        init_ubo();
        init_render_passes();
    }

    void init_render_resource() override {

        auto color_attachment = Texture_color_attachment::create(
            m_rhi_global_render_resource.window->width(), 
            m_rhi_global_render_resource.window->height()
        );
        color_attachment->link(m_rhi_global_render_resource.device);

        auto depth_attachment = Texture_depth_attachment::create(
            m_rhi_global_render_resource.window->width(),
            m_rhi_global_render_resource.window->height()
        );
        depth_attachment->link(m_rhi_global_render_resource.device);

        m_render_resource_manager.add("main_color_attachment", color_attachment);
        m_render_resource_manager.add("main_depth_attachment", depth_attachment);
    }

    void init_ubo() override {
        m_camera_ubo = Uniform_buffer<Camera_ubo>::create(Camera_ubo{});
        if (!m_camera_ubo->is_linked()) m_camera_ubo->link(m_rhi_global_render_resource.device);
        m_rhi_global_render_resource.memory_binder->bind_memory_buffer(m_camera_ubo->rhi_resource(), 0);
    }

    void init_render_passes() override {
        
        m_main_pass = Main_pass::create(m_rhi_global_render_resource);
        m_main_pass->set_resource_flow(Main_pass::Resource_flow{
            .color_attachment_in_out = m_render_resource_manager.get<Texture_color_attachment>("main_color_attachment"),
           .depth_attachment_in = m_render_resource_manager.get<Texture_depth_attachment>("main_depth_attachment")
        });

        m_gamma_pass = Gamma_pass::create(m_rhi_global_render_resource);
        m_gamma_pass->set_resource_flow(Gamma_pass::Resource_flow{
            .color_attachment_in = m_render_resource_manager.get<Texture_color_attachment>("main_color_attachment")
        });
    }

    ~Test_render_pipeline() {}

    void execute(const Render_tick_context& tick_context) override {
        auto& skybox = tick_context.render_swap_data.skybox;
        auto& render_objects = tick_context.render_swap_data.render_objects;

        m_main_pass->set_context(Main_pass::Execution_context{
            .skybox = skybox,
            .render_swap_objects = render_objects
        });

        m_main_pass->excute();
        m_gamma_pass->excute();
    }

    void update_ubo(const Render_tick_context& tick_context) override {
        m_camera_ubo->set_data(Camera_ubo{
            .view_matrix = tick_context.render_swap_data.camera.view_matrix,
            .projection_matrix = tick_context.render_swap_data.camera.projection_matrix,
            .camera_position = tick_context.render_swap_data.camera.camera_position
        });
        m_camera_ubo->push_to_rhi();
    }

    static std::shared_ptr<Test_render_pipeline> create(RHI_global_render_resource& global_render_resource) {
        return std::make_shared<Test_render_pipeline>(global_render_resource);
    }

};

class Forward_render_pipeline : public Render_pipeline {
public:
    Forward_render_pipeline(RHI_global_render_resource& global_render_resource) : Render_pipeline(global_render_resource) {}
    ~Forward_render_pipeline() {}

    void execute(const Render_tick_context& tick_context) override {

    }

    void update_ubo(const Render_tick_context& tick_context) override {
        
    }

};

class Deferred_render_pipeline : public Render_pipeline {
public:
    Deferred_render_pipeline(RHI_global_render_resource &global_render_resource) : Render_pipeline(global_render_resource) {}
    ~Deferred_render_pipeline() {}


    void execute(const Render_tick_context& tick_context) override {
        
    }

    void update_ubo(const Render_tick_context& tick_context) override {
        
    }

};

}