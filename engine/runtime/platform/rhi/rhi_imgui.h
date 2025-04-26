#pragma once

#include "engine/runtime/global/base.h" 
#include "rhi_window.h"
#include <memory>

namespace rtr {
class RHI_imgui {
protected:
    std::shared_ptr<RHI_window> m_window{};

public:

    RHI_imgui(const std::shared_ptr<RHI_window>& window) : m_window(window) {}
    ~RHI_imgui() = default;

    virtual void begin_frame() = 0;
    virtual void end_frame() = 0;
    virtual void begin_render(const std::string& title) = 0;
    virtual void end_render() = 0;

    virtual void color_edit(const std::string& title, float* color) = 0;
    virtual bool button(const std::string& title, float width, float height) = 0;
    virtual bool checkbox(const std::string& title, bool* value) = 0;
    virtual void text(const std::string& title, const std::string& text) = 0;
    virtual void text_edit(const std::string& title, std::string* text) = 0;
    virtual bool slider_float(const std::string& title, float* value, float min, float max) = 0;
    virtual float frame_rate() = 0;

};
}