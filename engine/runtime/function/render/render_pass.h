#pragma once

#include "engine/runtime/context/engine_tick_context.h"
#include "engine/runtime/platform/rhi/rhi_device.h"
#include <memory>

namespace rtr {
    
class Render_pass {
public:
    Render_pass() {}
    virtual ~Render_pass() {}
    virtual void init(const std::shared_ptr<RHI_device>& device) = 0;
    virtual void excute(const Logic_tick_context& tick_context) = 0;
};



}