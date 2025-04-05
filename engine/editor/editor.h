#pragma once
#include "engine/global/base.h"
#include "engine/runtime/rhi/rhi_window.h"
#include <functional>
#include <string>

namespace rtr {
class Editor {
protected:
    RHI_window::Ptr m_window{};

public:
    using Ptr = std::shared_ptr<Editor>;
    Editor(const RHI_window::Ptr& window) : m_window(window) {}
    ~Editor() = default;

    virtual void begin_frame() = 0;
    virtual void end_frame() = 0;
    virtual void begin_render(const std::string& title) = 0;
    virtual void end_render() = 0;
    virtual void color_edit(const std::string& title, float* color) = 0;
    virtual bool button(const std::string& title, float width, float height) = 0;

};
}