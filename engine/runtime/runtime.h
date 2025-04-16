#pragma once

#include "engine/runtime/context/engine_tick_context.h"
#include "engine/runtime/function/input/input_system.h"
#include "engine/runtime/function/render/render_system.h"
#include "engine/runtime/function/window/window_system.h"
#include "engine/runtime/global/timer.h"
#include "engine/runtime/platform/hal/file_service.h"
#include "engine/runtime/platform/rhi/opengl/rhi_device_opengl.h"
#include "engine/runtime/framework/world.h"
#include "engine/runtime/platform/rhi/rhi_device.h"

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

    Swap_data m_swap_data[2];
    int m_render_swap_data_index = 0;
    int m_logic_swap_data_index = 1;

    std::shared_ptr<RHI_device> m_rhi_device{};
    std::shared_ptr<Input_system> m_input_system{};
    std::shared_ptr<Render_system> m_render_system{};
    std::shared_ptr<Window_system> m_window_system{};

    std::shared_ptr<File_service> m_file_service{};
    std::shared_ptr<World> m_world{};

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
        
        m_rhi_device = device;
        m_window_system = window_system;
        m_input_system = input_system;
        m_file_service = file_service;
        m_render_system = render_system;
        m_world = descriptor.world;
    }

    std::shared_ptr<RHI_device>& rhi_device() { return m_rhi_device; }
    std::shared_ptr<Input_system>& input_system() { return m_input_system; }
    std::shared_ptr<Render_system>& render_system() { return m_render_system; }
    std::shared_ptr<Window_system>& window_system() { return m_window_system; }
    std::shared_ptr<File_service>& file_service() { return m_file_service; }
    std::shared_ptr<World>& world() { return m_world; }

    static std::shared_ptr<Engine_runtime> create(const Engine_runtime_descriptor& descriptor) {
        return std::make_shared<Engine_runtime>(descriptor);
    }

    virtual ~Engine_runtime() = default;

    void run() {

        auto timer = std::make_shared<Timer>();
        timer->start();

        while (m_window_system->window()->is_active()) {
            tick(get_delta_time(timer));
        }
    }

    float get_fps() const { return 1000.0f / m_delta_time; }

    float get_delta_time(const std::shared_ptr<Timer>& timer) {
        timer->pause();
        m_delta_time = timer->elapsed_ms();
        timer->start();
        return m_delta_time;
    }

    Swap_data& render_swap_data() { return m_swap_data[m_render_swap_data_index]; }
    Swap_data& logic_swap_data() { return m_swap_data[m_logic_swap_data_index]; }

    void swap() {
        std::swap(m_render_swap_data_index, m_logic_swap_data_index);
    }

    void tick(float delta_time) {
        m_window_system->window()->on_frame_begin();

        m_world->tick(Logic_tick_context(
            m_input_system->state(),
            logic_swap_data(),
            delta_time
        ));

        swap();

        m_render_system->tick(Render_tick_context(
            m_render_system->renderer(),
            m_render_system->screen_buffer(),
            render_swap_data(),
            delta_time
        ));

        m_rhi_device->check_error();
        m_window_system->window()->on_frame_end();
    }
    
};
};