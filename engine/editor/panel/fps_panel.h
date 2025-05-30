#pragma once

#include "engine/editor/panel/base_panel.h"

#include <memory>
#include <string>

namespace rtr {

namespace editor {

class FPS_panel : public Base_panel {

public:
    FPS_panel(
        const std::string& name
    ) : Base_panel(name) {}

    virtual void draw_panel() override {
        m_imgui->text("fps: %f", std::to_string(m_imgui->frame_rate()));
    }

    static std::shared_ptr<FPS_panel> create(
        const std::string& name
    ) {
        return std::make_shared<FPS_panel>(name);
    }
};      

}

}