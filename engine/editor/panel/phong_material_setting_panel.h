#pragma once

#include "engine/editor/panel/panel.h"

#include "engine/runtime/function/render/render_material/shading/phong_material.h"
#include "glm/gtc/type_ptr.hpp"
#include <memory>
#include <string>

namespace rtr {

namespace editor {

class Phong_material_settings_panel : public Panel {
protected:
    std::shared_ptr<Phong_material_settings> m_phong_material_settings{};
public:
    Phong_material_settings_panel(
        const std::string& name
    ) : Panel(name) {}

    void set_phong_material_settings(const std::shared_ptr<Phong_material_settings>& phong_material_settings) {
        m_phong_material_settings = phong_material_settings;
    }
    virtual void draw_panel() override {
        if (!m_phong_material_settings) return;
        m_imgui->color_edit("ambient", glm::value_ptr(m_phong_material_settings->ka));
        m_imgui->color_edit("diffuse", glm::value_ptr(m_phong_material_settings->kd));
        m_imgui->color_edit("specular", glm::value_ptr(m_phong_material_settings->ks));
        m_imgui->slider_float("shininess", &m_phong_material_settings->shininess, 1.0, 64.0);
    }

    static std::shared_ptr<Phong_material_settings_panel> create(
        const std::string& name
    ) {
        return std::make_shared<Phong_material_settings_panel>(name);
    }
};
        

}

}