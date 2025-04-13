#pragma once
#include "engine/runtime/function/context/global_context.h"
#include "engine/runtime/global/base.h"
#include "engine/runtime/platform/rhi/opengl/rhi_device_opengl.h"
#include <memory>

namespace rtr {
class Engine_runtime {
private:
    std::shared_ptr<Global_context> m_global_context{};

public:
    Engine_runtime() {
        m_global_context = std::shared_ptr<Global_context>();
        m_global_context->rhi_device = RHI_device_OpenGL::create();
    }
    virtual ~Engine_runtime() = default;
    
    static std::shared_ptr<Engine_runtime> create() {
        return std::make_shared<Engine_runtime>();
    }

    void run() {
        
    }

    float get_fps() const {
        return 0.0f;
    }

    float get_delta_time() const {
        return 0.0f;
    }

    void render_tick(float delta_time) {

    }

    void logic_tick(float delta_time) {
        
    }

    void tick(float delta_time) {
        logic_tick(delta_time);
        render_tick(delta_time);
    }


    
    
};
};