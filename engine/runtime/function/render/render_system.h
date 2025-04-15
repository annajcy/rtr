#pragma once

#include "engine/runtime/function/render/render_struct.h"
#include "engine/runtime/function/render/render_pipeline.h"

#include "engine/runtime/global_context.h"
#include "engine/runtime/platform/rhi/rhi_device.h"
#include "engine/runtime/platform/rhi/rhi_frame_buffer.h"
#include "engine/runtime/platform/rhi/rhi_renderer.h"
#include "engine/runtime/platform/rhi/rhi_window.h"

#include <memory>
#include <utility>

namespace rtr {

class Render_system {

protected:
    std::shared_ptr<RHI_device> m_device{};
    std::shared_ptr<RHI_window> m_window{};
    std::shared_ptr<RHI_frame_buffer> m_screen_buffer{};
    std::shared_ptr<RHI_renderer> m_renderer{};
    Swap_data m_swap_data[2];
    int m_render_swap_data_index = 0;
    int m_logic_swap_data_index = 1;

    std::shared_ptr<Render_pipeline> m_render_pipeline{};

public:
    Render_system(
        const RHI_device::Ptr& device, 
        const std::shared_ptr<RHI_window>& window
    ) : m_device(device), 
        m_window(window), 
        m_screen_buffer(device->create_screen_frame_buffer(window)),
        m_renderer(device->create_renderer(Clear_state::enabled())) {}

    static std::shared_ptr<Render_system> create(
        const RHI_device::Ptr& device,
        const std::shared_ptr<RHI_window>& window
    ) {
        return std::make_shared<Render_system>(device, window);
    }

    Swap_data& render_swap_data() { return m_swap_data[m_render_swap_data_index]; }
    Swap_data& logic_swap_data() { return m_swap_data[m_logic_swap_data_index]; }

    void swap() {
        std::swap(m_render_swap_data_index, m_logic_swap_data_index);
    }

    void prepare_ubo() {

    }

    void tick(const Engine_tick_context& tick_context) {
        swap();
        prepare_ubo();
        m_renderer->clear(m_screen_buffer);
    }

};

}