#pragma once

#include "engine/runtime/framework/component/mesh_renderer/mesh_renderer_component.h"
#include "engine/runtime/function/render/object/material.h"
#include "engine/runtime/platform/rhi/rhi_imgui.h"
#include "engine/runtime/runtime.h"
#include "glm/gtc/type_ptr.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace rtr {

namespace editor {

class Panel {
protected:
    std::shared_ptr<RHI_imgui> m_imgui{};
    std::string m_name{};
    bool m_is_open{true};

public:
    Panel(
        const std::string& name,
        const std::shared_ptr<RHI_imgui>& imgui
    ) : m_name(name), m_imgui(imgui){}

    void render() {
        if (!m_is_open) return;
        m_imgui->begin_render(m_name);
        draw_panel();
        m_imgui->end_render(); 
    }

    virtual void draw_panel() = 0;

    virtual ~Panel() = default;

    std::string name() const { return m_name; }
};

class Shadow_settings_panel : public Panel {
protected:
    std::shared_ptr<Shadow_settings> m_shadow_settings{};
public:
    Shadow_settings_panel(
        const std::string& name,
        const std::shared_ptr<RHI_imgui>& imgui
    ) : Panel(name, imgui) {}

    void set_shadow_settings(const std::shared_ptr<Shadow_settings>& shadow_settings) {
        m_shadow_settings = shadow_settings;
    }

    virtual void draw_panel() override {
        if (!m_shadow_settings) return;
        m_imgui->slider_float("shadow bias", &m_shadow_settings->shadow_bias, 0.0, 0.1);
        m_imgui->slider_float("light size", &m_shadow_settings->light_size, 1.0, 20.0);
        m_imgui->slider_float("pcf radius", &m_shadow_settings->pcf_radius, 0.0, 1.0);
        m_imgui->slider_float("pcf tightness", &m_shadow_settings->pcf_tightness, 0.1, 1.0);
        m_imgui->slider_int("pcf samples", &m_shadow_settings->pcf_sample_count, 1.0, 20.0);
    }

    static std::shared_ptr<Shadow_settings_panel> create(
        const std::string& name,
        const std::shared_ptr<RHI_imgui>& imgui
    ) {
        return std::make_shared<Shadow_settings_panel>(name, imgui);
    }
};

class Phong_material_settings_panel : public Panel {
protected:
    std::shared_ptr<Phong_material_settings> m_phong_material_settings{};
public:
    Phong_material_settings_panel(
        const std::string& name,
        const std::shared_ptr<RHI_imgui>& imgui
    ) : Panel(name, imgui) {}

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
        const std::string& name,
        const std::shared_ptr<RHI_imgui>& imgui
    ) {
        return std::make_shared<Phong_material_settings_panel>(name, imgui);
    }
};

class Parallax_settings_panel : public Panel {
protected:
    std::shared_ptr<Parallax_settings> m_parallax_settings{};

public:
    Parallax_settings_panel(
        const std::string& name,
        const std::shared_ptr<RHI_imgui>& imgui
    ) : Panel(name, imgui) {}

    void set_parallax_settings(const std::shared_ptr<Parallax_settings>& parallax_settings) {
        m_parallax_settings = parallax_settings;
    }

    virtual void draw_panel() override {
        if (!m_parallax_settings) return;
        m_imgui->slider_float("parallax scale", &m_parallax_settings->parallax_scale, 0.0, 1.0);
        m_imgui->slider_float("parallax layer count", &m_parallax_settings->parallax_layer_count, 0.0, 20.0);
    }

    static std::shared_ptr<Parallax_settings_panel> create(
        const std::string& name,
        const std::shared_ptr<RHI_imgui>& imgui
    ) {
        return std::make_shared<Parallax_settings_panel>(name, imgui);
    }
};

class Editor {

private:
    std::shared_ptr<Engine_runtime> m_engine_runtime{};
    std::shared_ptr<RHI_imgui> m_imgui{};

    std::unordered_map<std::string, std::shared_ptr<Panel>> m_panel_map{};

public:

    Editor(const std::shared_ptr<Engine_runtime>& engine_runtime) : 
    m_engine_runtime(engine_runtime),
    m_imgui(engine_runtime->window_system()->window()->imgui()) {

        add_panel(Shadow_settings_panel::create("shadow settings", m_imgui));
        add_panel(Phong_material_settings_panel::create("phong material settings", m_imgui));
        add_panel(Parallax_settings_panel::create("parallax settings", m_imgui));

    }

    void add_panel(const std::shared_ptr<Panel>& panel) {
        m_panel_map[panel->name()] = panel;
    }

    void remove_panel(const std::string& name) {
        m_panel_map.erase(name);
    }

    template<typename T>
    std::shared_ptr<T> get_panel(const std::string& name) {
        return std::dynamic_pointer_cast<T>(m_panel_map[name]);
    }

    void tick(float delta_time) {
        runtime_tick(delta_time);
        editor_tick(delta_time);
    }

    void runtime_tick(float delta_time) {
        m_engine_runtime->tick(delta_time);
    }

    void editor_tick(float delta_time) {
        m_imgui->begin_frame();
        for (auto& [name, panel] : m_panel_map) {
            panel->render();
        }
        m_imgui->end_frame();
    }

    void run() {
        while (m_engine_runtime->is_active()) {
            tick(m_engine_runtime->get_delta_time());
        }
    }

    static std::shared_ptr<Editor> create(const std::shared_ptr<Engine_runtime>& engine_runtime) {
        return std::make_shared<Editor>(engine_runtime);
    }
    
};

}

}