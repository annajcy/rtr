#pragma once

#include "engine/runtime/platform/rhi/rhi_device.h"
#include <memory>

namespace rtr {

template <typename T>
class RHI_linker {
protected:
    std::shared_ptr<T> m_rhi_resource{};

public:

    RHI_linker() {}
    virtual ~RHI_linker() {}

    virtual void link(const std::shared_ptr<RHI_device>& device) = 0; 

    const std::shared_ptr<T>& rhi_resource() const {
        return m_rhi_resource;
    }

    bool is_linked() const {
        return m_rhi_resource != nullptr;
    }
};

};