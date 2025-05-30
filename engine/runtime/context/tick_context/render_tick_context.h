#pragma once

#include "engine/runtime/context/swap/swap_data.h"
#include "engine/runtime/function/input/input_system.h"

namespace rtr {
    
struct Render_tick_context {
    Swap_data &render_swap_data;
    float delta_time{};
    
    Render_tick_context(
        Swap_data &render_swap_data,
        float delta_time
    ) : render_swap_data(render_swap_data),
        delta_time(delta_time) {}
};

}