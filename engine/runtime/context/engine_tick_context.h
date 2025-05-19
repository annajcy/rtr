#pragma once

#include "engine/runtime/context/swap_struct.h"
#include "engine/runtime/function/input/input_system.h"

namespace rtr {
    
struct Logic_tick_context {
    Input_state input_state{};
    Swap_data &logic_swap_data;
    float delta_time{};

    Logic_tick_context(
        Input_state input_state,
        Swap_data &logic_swap_data,
        float delta_time
    ) : input_state(input_state),
        logic_swap_data(logic_swap_data),
        delta_time(delta_time) {}
};

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