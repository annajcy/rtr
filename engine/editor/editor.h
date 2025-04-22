#pragma once

#include "engine/runtime/framework/component/camera/camera_component.h"
#include "engine/runtime/framework/component/camera/camera_control_component.h"
#include "engine/runtime/framework/component/mesh_renderer/mesh_renderer_component.h"
#include "engine/runtime/framework/component/node/node_component.h"
#include "engine/runtime/platform/rhi/rhi_imgui.h"
#include "engine/runtime/platform/rhi/rhi_window.h"
#include "engine/runtime/runtime.h"
#include "glm/gtc/type_ptr.hpp"
#include <memory>
#include <string>

namespace rtr {

namespace editor {

class Editor {

private:
    std::shared_ptr<Engine_runtime> m_engine_runtime{};
    std::shared_ptr<RHI_window> m_window{};
    std::shared_ptr<RHI_imgui> m_imgui{};

public:

    Editor(const std::shared_ptr<Engine_runtime>& engine_runtime) : 
    m_engine_runtime(engine_runtime),
    m_window(engine_runtime->window_system()->window()),
    m_imgui(engine_runtime->rhi_device()->create_imgui(m_engine_runtime->window_system()->window())) {}

    void tick(float delta_time) {

        if (m_engine_runtime) {
            m_engine_runtime->tick(delta_time);
        }

        if (m_imgui) {
            m_imgui->begin_frame();
            render_main_menu();
            render_inspector();
            m_imgui->end_frame();
        }
        
    }

    void run() {
        auto timer = std::make_shared<Timer>();
        timer->start();

        while (m_window->is_active()) {
            tick(m_engine_runtime->get_delta_time(timer));
        }
    }

    static std::shared_ptr<Editor> create(const std::shared_ptr<Engine_runtime>& engine_runtime) {
        return std::make_shared<Editor>(engine_runtime);
    }

    
private:

    void render_main_menu() {
        m_imgui->begin_render("main menu");
        m_imgui->text("fps", std::to_string(m_engine_runtime->get_fps()));
        m_imgui->end_render();
    }

    void render_inspector() {
        m_imgui->begin_render("inspector");
        m_imgui->color_edit("bg color", glm::value_ptr(m_engine_runtime->render_system()->global_render_resource().renderer->clear_state().color_clear_value));
        if (m_imgui->button("change", 50.0, 30.0)) {
            m_engine_runtime->render_system()->global_render_resource().renderer->apply_clear_state();
        }
        for (auto& gos : m_engine_runtime->world()->current_scene()->game_objects()) {
            m_imgui->text("game object", gos->name());
            for (auto& component : gos->component_list()->components()) {
                if (component->component_type() == Component_type::NODE) {
                    auto node = std::dynamic_pointer_cast<Node_component>(component);
                    m_imgui->text("position", glm::to_string(node->position()));
                    m_imgui->text("rotation", glm::to_string(node->rotation()));
                    m_imgui->text("scale", glm::to_string(node->scale()));
                } else if (component->component_type() == Component_type::MESH_RENDERER) {
                    auto mesh_renderer = std::dynamic_pointer_cast<Mesh_renderer_component>(component);
                    m_imgui->text("material", mesh_renderer->material()->shader()->get_main_shader()->name());
                } else if (component->component_type() == Component_type::CAMERA) {
                    auto camera = std::dynamic_pointer_cast<Camera_component>(component);
                    m_imgui->text("camera type", camera->camera_type() == Camera_type::PERSPECTIVE ? "perspective" : "orthographic");
                } else if (component->component_type() == Component_type::CAMERA_CONTROL) {
                    auto camera_control = std::dynamic_pointer_cast<Camera_control_component>(component);
                    m_imgui->text("camera control type", camera_control->camera_control_type() == Camera_control_type::TRACKBALL? "trackball" : "orbit");
                }
            }
        }
        m_imgui->frame_rate();
        m_imgui->end_render();
    }
    
};

}

}