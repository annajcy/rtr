#pragma once

#include "engine/runtime/function/framework/world.h"
#include "engine/runtime/function/input/input_system.h"
#include "engine/runtime/function/render/render_system.h"
#include "engine/runtime/global/base.h" 
#include "engine/runtime/global/singleton.h"
#include "engine/runtime/platform/rhi/rhi_device.h"
#include <memory>

namespace rtr {
    
struct Global_context {
    std::shared_ptr<RHI_device> rhi_device{};
    
    std::shared_ptr<Input_system> input_system{};
    std::shared_ptr<Render_system> render_system{};

    std::shared_ptr<World> world{};
};

}