#pragma once

#include "engine/runtime/platform/rhi/rhi_device.h"

namespace rtr {
    
class Render_pass {
protected:
    RHI_global_resource& m_rhi_global_resource;

public:
    Render_pass(
        RHI_global_resource& rhi_global_resource
    ) : m_rhi_global_resource(rhi_global_resource) {}

    virtual ~Render_pass() {}
    virtual void excute() = 0;
};

}