#pragma once

#include "engine/runtime/platform/rhi/rhi_imgui.h"

#include <iostream>
#include <memory>
#include <string>

namespace rtr {

namespace editor {

class Panel {
protected:
    std::shared_ptr<RHI_imgui> m_imgui{};
    std::string m_name{};
    bool m_is_open{true};

public:
    Panel(const std::string& name) : m_name(name) {}

    void render() {
        if (!m_imgui) {
            std::cout << "imgui instance is not initialized, panel will not be rendered" << std::endl;
            return;
        }
        if (!m_is_open) return;
        m_imgui->begin_render(m_name);
        draw_panel();
        m_imgui->end_render(); 
    }

    virtual void draw_panel() = 0;

    virtual ~Panel() = default;

    std::string name() const { return m_name; }
    void set_imgui(const std::shared_ptr<RHI_imgui>& imgui) { m_imgui = imgui; }
};



}

}