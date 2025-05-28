#pragma once

#include "engine/editor/panel/panel.h"
#include "engine/runtime/framework/component/mesh_renderer/mesh_renderer_component.h"
#include "engine/runtime/function/render/render_material/material.h"
#include "engine/runtime/platform/rhi/rhi_imgui.h"
#include "engine/runtime/runtime.h"

#include "glm/gtc/type_ptr.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace rtr {

namespace editor {

class Editor {

private:
    std::shared_ptr<Engine_runtime> m_engine_runtime{};
    std::shared_ptr<RHI_imgui> m_imgui{};

    std::unordered_map<std::string, std::shared_ptr<Panel>> m_panel_map{};

public:

    Editor(
        const std::shared_ptr<Engine_runtime>& engine_runtime,
        const std::vector<std::shared_ptr<Panel>>& panels
    ) : m_engine_runtime(engine_runtime),
        m_imgui(engine_runtime->rhi_global_resource().window->imgui()) {

        for (auto& panel : panels) {
            add_panel(panel);
        }
    }

    void add_panel(const std::shared_ptr<Panel>& panel) {
        panel->set_imgui(m_imgui);
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

    static std::shared_ptr<Editor> create(
        const std::shared_ptr<Engine_runtime>& engine_runtime,
        const std::vector<std::shared_ptr<Panel>>& panels) {
        return std::make_shared<Editor>(engine_runtime, panels);
    }
    
};

}

}