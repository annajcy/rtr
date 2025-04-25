#pragma once

#include "engine/runtime/function/render/render_pipeline.h"
#include "engine/runtime/platform/rhi/rhi_device.h"
#include "engine/runtime/platform/rhi/rhi_renderer.h"
#include "engine/runtime/platform/rhi/rhi_window.h"

#include <memory>

namespace rtr {

class Render_system {

protected:
    RHI_global_render_resource m_global_render_resource{};
    std::shared_ptr<Render_pipeline> m_render_pipeline{};

public:
    Render_system(
        const std::shared_ptr<RHI_device>& device, 
        const std::shared_ptr<RHI_window>& window
    ) {
        m_global_render_resource.device = device;
        m_global_render_resource.window = window;
        m_global_render_resource.renderer = device->create_renderer(Clear_state::enabled());
        m_global_render_resource.screen_buffer = device->create_screen_buffer(window);
        m_global_render_resource.memory_binder = device->create_memory_buffer_binder();
        m_global_render_resource.pipeline_state = device->create_pipeline_state();
    }

    static std::shared_ptr<Render_system> create(
        const std::shared_ptr<RHI_device>& device, 
        const std::shared_ptr<RHI_window>& window
    ) {
        return std::make_shared<Render_system>(device, window);
    }

    const RHI_global_render_resource& global_render_resource() const {
        return m_global_render_resource;
    }

    RHI_global_render_resource& global_render_resource() {
        return m_global_render_resource;
    }

    void set_render_pipeline(const std::shared_ptr<Render_pipeline>& pipeline) {
        m_render_pipeline = pipeline;
    }

    void tick(const Render_tick_context& tick_context) {
        m_render_pipeline->update_ubo(tick_context);
        m_render_pipeline->update_render_pass(tick_context);
        m_render_pipeline->execute(tick_context);
    }

};

}