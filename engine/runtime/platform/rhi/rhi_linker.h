#pragma once

#include "engine/runtime/platform/rhi/rhi_device.h"
#include <memory>

namespace rtr {

template <typename T>
class RHI_linker {
protected:
    std::shared_ptr<T> m_rhi{};

public:

    RHI_linker() {}
    virtual ~RHI_linker() {}

    std::shared_ptr<T>& rhi(const std::shared_ptr<RHI_device>& device) {
        if (!is_linked()) {
            link(device);
        }
        return m_rhi;
    }

    bool is_linked() const {
        return m_rhi != nullptr;
    }

    virtual void link(const std::shared_ptr<RHI_device>& device) = 0; 
};

};