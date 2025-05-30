#pragma once

#include "engine/editor/panel/base_panel.h"
#include "engine/runtime/function/render/material/shading/phong_material.h"

#include <memory>
#include <string>

namespace rtr {

namespace editor {

class Parallax_settings_panel : public Base_panel {
protected:
    std::shared_ptr<Parallax_settings> m_parallax_settings{};

public:
    Parallax_settings_panel(const std::string& name) : Base_panel(name) {}

    void set_parallax_settings(const std::shared_ptr<Parallax_settings>& parallax_settings) {
        m_parallax_settings = parallax_settings;
    }

    virtual void draw_panel() override {
        if (!m_parallax_settings) return;
        m_imgui->slider_float("parallax scale", &m_parallax_settings->parallax_scale, 0.0, 1.0);
        m_imgui->slider_float("parallax layer count", &m_parallax_settings->parallax_layer_count, 0.0, 20.0);
    }

    static std::shared_ptr<Parallax_settings_panel> create(
        const std::string& name
    ) {
        return std::make_shared<Parallax_settings_panel>(name);
    }
};

}

}