#pragma once

#include "engine/runtime/framework/component/mesh_renderer/mesh_renderer_component.h"
#include "engine/runtime/platform/rhi/rhi_imgui.h"
#include "engine/runtime/runtime.h"
#include <memory>
#include <string>

namespace rtr {

namespace editor {

class Editor {

private:
    std::shared_ptr<Engine_runtime> m_engine_runtime{};
    std::shared_ptr<RHI_imgui> m_imgui{};

public:

    Editor(const std::shared_ptr<Engine_runtime>& engine_runtime) : 
    m_engine_runtime(engine_runtime),
    m_imgui(engine_runtime->window_system()->window()->imgui()) {}

    void tick(float delta_time) {
        runtime_tick(delta_time);
        gui_tick(delta_time);
    }

    void runtime_tick(float delta_time) {
        m_engine_runtime->tick(delta_time);
    }

    void gui_tick(float delta_time) {
        m_imgui->begin_frame();
        main_menu();
        parallax_map();
        shadow_map();
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

    
private:

    void shadow_map() {
        m_imgui->begin_render("shadow map");
        auto go_material = m_engine_runtime->world()->current_scene()->get_game_object("go1")->get_component<Mesh_renderer_component>()->mesh_renderer()->material();
        // if (auto test_material = std::dynamic_pointer_cast<Test_material>(go_material)) {
        //     m_imgui->slider_float("go shadow bias", &test_material->shadow_bias, 0.0, 0.01);
        // }
        // auto plane_material = m_engine_runtime->world()->current_scene()->get_game_object("plane")->get_component<Mesh_renderer_component>()->mesh_renderer()->material();
        // if (auto test_material = std::dynamic_pointer_cast<Test_material>(plane_material)) {
        //     m_imgui->slider_float("plane shadow bias", &test_material->shadow_bias, 0.0, 0.01);
        // }
        m_imgui->end_render();
    }

    void parallax_map() {
        m_imgui->begin_render("parallax map");
        auto material = m_engine_runtime->world()->current_scene()->get_game_object("go1")->get_component<Mesh_renderer_component>()->mesh_renderer()->material();
        // if (auto test_material = std::dynamic_pointer_cast<Test_material>(material)) {
        //     m_imgui->slider_float("parallax_scale", &test_material->parallax_scale, 0.0, 1.0);
        //     m_imgui->slider_float("parallax_layer_count", &test_material->parallax_layer_count, 0.0, 20.0);
        // }
        m_imgui->end_render();
    }

    void main_menu() {
        m_imgui->begin_render("main menu");
        m_imgui->text("fps", std::to_string(m_engine_runtime->get_fps()));
        m_imgui->end_render();
    }
    
};

}

}