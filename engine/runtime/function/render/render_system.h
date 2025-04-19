#pragma once

#include "engine/runtime/core/memory_buffer.h"
#include "engine/runtime/function/render/render_pipeline.h"

#include "engine/runtime/function/render/render_struct.h"
#include "engine/runtime/platform/rhi/rhi_device.h"
#include "engine/runtime/platform/rhi/rhi_frame_buffer.h"
#include "engine/runtime/platform/rhi/rhi_renderer.h"
#include "engine/runtime/platform/rhi/rhi_window.h"

#include <memory>

namespace rtr {

class Render_system {

protected:
    std::shared_ptr<RHI_device> m_device{};
    std::shared_ptr<RHI_frame_buffer> m_screen_buffer{};
    std::shared_ptr<RHI_renderer> m_renderer{};
    std::shared_ptr<Render_pipeline> m_render_pipeline{};

public:
    Render_system(
        const std::shared_ptr<RHI_device>& device, 
        const std::shared_ptr<RHI_window>& window
    ) : m_device(device), 
        m_screen_buffer(device->create_screen_frame_buffer(window)),
        m_renderer(device->create_renderer(Clear_state::enabled())) {
            m_render_pipeline = std::make_shared<Test_render_pipeline>(m_device);
        }

    static std::shared_ptr<Render_system> create(
        const std::shared_ptr<RHI_device>& device, 
        const std::shared_ptr<RHI_window>& window
    ) {
        return std::make_shared<Render_system>(device, window);
    }

    std::shared_ptr<RHI_device>& device() { return m_device; }
    std::shared_ptr<RHI_frame_buffer>& screen_buffer() { return m_screen_buffer; }
    std::shared_ptr<RHI_renderer>& renderer() { return m_renderer; }

    void set_render_pipeline(const std::shared_ptr<Render_pipeline>& pipeline) {
        m_render_pipeline = pipeline;
    }

    void tick(const Render_tick_context& tick_context) {
        m_render_pipeline->update_ubo(tick_context);
        m_render_pipeline->execute(tick_context);
    }

};

}