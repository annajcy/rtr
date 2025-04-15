#pragma once

#include "engine/runtime/runtime.h"
#include <memory>

namespace rtr {

namespace editor {

class Editor {

private:
    std::shared_ptr<Engine_runtime> m_engine_runtime{};

public:

    Editor(const std::shared_ptr<Engine_runtime>& engine_runtime) : m_engine_runtime(engine_runtime) {}
    void tick(float delta_time) {}

    
private:

    void render_main_menu() {}
    void render_scene_hierarchy() {}
    void render_inspector() {}
    void render_console() {}
    void render_stats() {}

};

}

}