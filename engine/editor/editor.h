#pragma once

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
        m_imgui->color_edit("bg color", glm::value_ptr(m_engine_runtime->render_system()->renderer()->clear_state().color_clear_value));
        if (m_imgui->button("change", 50.0, 30.0)) {
            m_engine_runtime->render_system()->renderer()->apply_clear_state();
        }
        m_imgui->frame_rate();
        m_imgui->end_render();
    }
    
};

}

}