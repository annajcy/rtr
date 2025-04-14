#pragma once
#include "engine/runtime/function/context/global_context.h"
#include "engine/runtime/function/framework/world.h"
#include "engine/runtime/function/render/render_system.h"
#include "engine/runtime/function/window/window_system.h"
#include "engine/runtime/global/base.h"
#include "engine/runtime/platform/hal/file_service.h"
#include "engine/runtime/platform/rhi/opengl/rhi_device_opengl.h"
#include <memory>

namespace rtr {

struct Engine_runtime_descriptor {
    int width = 1280;
    int height = 720;
    std::string title = "RTR Engine";
    std::shared_ptr<World> world{};
};

class Engine_runtime {

public:
    Engine_runtime(const Engine_runtime_descriptor& descriptor) {

        Global_context::rhi_device = RHI_device_OpenGL::create();

        Global_context::window_system = Window_system::create(Global_context::rhi_device->create_window(
            descriptor.width, 
            descriptor.height, 
            descriptor.title
        ));

        Global_context::input_system = Input_system::create(
            Global_context::window_system->window()
        );

        Global_context::render_system  = Render_system::create(Global_context::rhi_device);
        Global_context::file_service = File_service::create("assets");
        Global_context::world = descriptor.world;
    }

    static std::shared_ptr<Engine_runtime> create(const Engine_runtime_descriptor& descriptor) {
        return std::make_shared<Engine_runtime>(descriptor);
    }

    virtual ~Engine_runtime() = default;

    void run() {
        while (Global_context::window_system->window()->is_active()) {
            Global_context::window_system->window()->on_frame_begin();
            tick(get_delta_time());
            Global_context::rhi_device->check_error();
            Global_context::window_system->window()->on_frame_end();
        }
    }

    float get_fps() const {
        return 0.0f;
    }

    float get_delta_time() const {
        return 0.0f;
    }

    void render_tick(float delta_time) {
        Global_context::render_system->tick(delta_time);
    }

    void logic_tick(float delta_time) {
        Global_context::world->tick(delta_time);
    }

    void tick(float delta_time) {
        logic_tick(delta_time);
        render_tick(delta_time);
    }
    
};
};