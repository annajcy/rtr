#pragma once

#include "engine/runtime/context/swap_struct.h"
#include "engine/runtime/function/input/input_system.h"
#include "engine/runtime/platform/rhi/rhi_frame_buffer.h"
#include "engine/runtime/platform/rhi/rhi_renderer.h"
#include <memory>

namespace rtr {
    
struct Logic_tick_context {
    const Input_system_state& input_state;
    Swap_data &logic_swap_data;
    float delta_time{};

    Logic_tick_context(
        const Input_system_state& input_state,
        Swap_data &logic_swap_data,
        float delta_time
    ) : input_state(input_state),
        logic_swap_data(logic_swap_data),
        delta_time(delta_time) {}
};

struct Render_tick_context {
    std::shared_ptr<RHI_renderer> renderer{};
    std::shared_ptr<RHI_frame_buffer> screen_frame_buffer{};
    const Swap_data &render_swap_data;
    float delta_time{};
    
    Render_tick_context(
        const std::shared_ptr<RHI_renderer>& renderer,
        std::shared_ptr<RHI_frame_buffer> screen_frame_buffer,
        const Swap_data &render_swap_data,
        float delta_time
    ) : renderer(renderer),
        screen_frame_buffer(screen_frame_buffer),
        render_swap_data(render_swap_data),
        delta_time(delta_time) {}
};

}