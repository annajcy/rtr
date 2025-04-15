#pragma once
#include "global_context.h"

#include "engine/runtime/function/input/input_system.h"
#include "engine/runtime/function/render/render_system.h"
#include "engine/runtime/function/window/window_system.h"
#include "engine/runtime/global/timer.h"
#include "engine/runtime/platform/hal/file_service.h"
#include "engine/runtime/platform/rhi/opengl/rhi_device_opengl.h"
#include "engine/runtime/framework/world.h"

#include <memory>

namespace rtr {

struct Engine_runtime_descriptor {
    int width{800};
    int height{600};
    std::string title{"RTR Engine"};
    std::shared_ptr<World> world{};
};


class Engine_runtime {
private:
    float m_delta_time {0.0f};
    std::shared_ptr<Global_context> m_global_context{};

public:
    Engine_runtime(const Engine_runtime_descriptor& descriptor) {

        auto device = RHI_device_OpenGL::create();

        auto window = device->create_window(
            descriptor.width, 
            descriptor.height, 
            descriptor.title
        );

        auto window_system = Window_system::create(window);
        auto input_system = Input_system::create(window);
        auto render_system = Render_system::create(device, window);
        auto file_service = File_service::create("assets");

        m_global_context = std::make_shared<Global_context>();
        
        m_global_context->rhi_device = device;
        m_global_context->window_system = window_system;
        m_global_context->input_system = input_system;
        m_global_context->file_service = file_service;
        m_global_context->render_system = render_system;
        m_global_context->world = descriptor.world;
    }

    static std::shared_ptr<Engine_runtime> create(const Engine_runtime_descriptor& descriptor) {
        return std::make_shared<Engine_runtime>(descriptor);
    }

    virtual ~Engine_runtime() = default;

    void run() {

        auto timer = std::make_shared<Timer>();
        timer->start();

        while (m_global_context->window_system->window()->is_active()) {
            m_global_context->window_system->window()->on_frame_begin();

            tick(Engine_tick_context{
                m_global_context, 
                get_delta_time(timer)
            });

            m_global_context->rhi_device->check_error();
            m_global_context->window_system->window()->on_frame_end();
            std::cout << "fps: " << get_fps() << '\n';
        }
    }

    float get_fps() const { return 1000.0f / m_delta_time; }

    float get_delta_time(const std::shared_ptr<Timer>& timer) {
        timer->pause();
        m_delta_time = timer->elapsed_ms();
        timer->start();
        return m_delta_time;
    }

    void tick(const Engine_tick_context& tick_context) {
        m_global_context->world->tick(tick_context);
        m_global_context->render_system->tick(tick_context);
    }
    
};
};