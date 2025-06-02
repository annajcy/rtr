#pragma once

#include "engine/editor/panel/base_panel.h"
#include "engine/runtime/function/render/material/setting.h"

#include <memory>
#include <string>

namespace rtr {

namespace editor {

class Shadow_settings_panel : public Base_panel {
protected:
    std::shared_ptr<Shadow_setting> m_shadow_settings{};
public:
    Shadow_settings_panel(
        const std::string& name
    ) : Base_panel(name) {}

    void set_shadow_settings(const std::shared_ptr<Shadow_setting>& shadow_settings) {
        m_shadow_settings = shadow_settings;
    }

    virtual void draw_panel() override {
        if (!m_shadow_settings) return;
        m_imgui->slider_float("shadow bias", &m_shadow_settings->shadow_bias, 0.0, 0.1);
        m_imgui->slider_float("light size", &m_shadow_settings->light_size, 0.1, 5.0);
        m_imgui->slider_float("pcf radius", &m_shadow_settings->pcf_radius, 0.0, 2.0);
        m_imgui->slider_float("pcf tightness", &m_shadow_settings->pcf_tightness, 0.1, 10.0);
        m_imgui->slider_int("pcf samples", &m_shadow_settings->pcf_sample_count, 1.0, 3.0);
    }

    static std::shared_ptr<Shadow_settings_panel> create(
        const std::string& name
    ) {
        return std::make_shared<Shadow_settings_panel>(name);
    }
};






}

}