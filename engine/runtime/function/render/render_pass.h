#pragma once

#include "engine/runtime/global/base.h"
#include "engine/runtime/platform/rhi/rhi_device.h"
#include <memory>
#include <vector>

namespace rtr {
    
class Execute_context;

class Render_pass {
public:
    Render_pass() {}
    virtual ~Render_pass() {}
    virtual void init(const std::shared_ptr<RHI_device>& device) = 0;
    virtual void excute(const Execute_context& context) = 0;
};

}