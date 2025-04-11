#pragma once

#include "engine/runtime/global/base.h"
#include "engine/runtime/platform/rhi/rhi_device.h"

namespace rtr {

class Render_system {
protected:
    RHI_device::Ptr m_device{};

public:
    Render_system(const RHI_device::Ptr& device) : m_device(device) {}
    void tick(float delta_time) {}

};

}