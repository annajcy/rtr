#pragma once

#include "engine/runtime/platform/rhi/rhi_window.h"
#include <memory>

namespace rtr {
class Window_system {
protected:
    std::shared_ptr<RHI_window> m_window{};

public:
    Window_system(const std::shared_ptr<RHI_window>& window) : m_window(window) {}
    ~Window_system() = default;

    static std::shared_ptr<Window_system> create(const std::shared_ptr<RHI_window>& window) {
        return std::make_shared<Window_system>(window);
    }
    
    void set_window(const std::shared_ptr<RHI_window>& window) { m_window = window; }
    std::shared_ptr<RHI_window> window() const { return m_window; }

};
}