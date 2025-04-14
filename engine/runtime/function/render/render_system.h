#pragma once

#include "engine/runtime/global/base.h"
#include "engine/runtime/platform/rhi/rhi_device.h"
#include <memory>

namespace rtr {

struct Render_swap_data {

};

class Render_system {
protected:
    std::shared_ptr<RHI_device> m_device{};

public:
    Render_system(const RHI_device::Ptr& device) : m_device(device) {}

    static std::shared_ptr<Render_system> create(const RHI_device::Ptr& device) {
        return std::make_shared<Render_system>(device);
    }

    void tick(float delta_time) {
        glClearColor(1.0f, 0.75f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

};

}