#pragma once

#include "engine/runtime/context/swap/swap_data.h"
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

}