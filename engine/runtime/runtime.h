#pragma once

#include "engine/runtime/context/engine_tick_context.h"
#include "engine/runtime/context/swap_struct.h"
#include "engine/runtime/function/input/input_system.h"
#include "engine/runtime/function/render/render_system.h"
#include "engine/runtime/global/logger.h"
#include "engine/runtime/global/timer.h"
#include "engine/runtime/platform/rhi/opengl/rhi_device_opengl.h"
#include "engine/runtime/framework/core/world.h"
#include "engine/runtime/platform/rhi/rhi_device.h"
#include "engine/runtime/platform/rhi/rhi_window.h"
#include "engine/runtime/resource/file_service.h"

namespace rtr {

struct Engine_runtime_descriptor {
    int width{800};
    int height{600};
    std::string title{"RTR Engine"};
};

class Engine_runtime {
private:
    float m_delta_time {0.0f};

    Swap_data m_swap_data[2];
    int m_render_swap_data_index = 0;
    int m_logic_swap_data_index = 1;

    std::shared_ptr<RHI_device> m_rhi_device{};
    std::shared_ptr<RHI_window> m_rhi_window{};
    
    std::shared_ptr<World> m_world{};
    std::shared_ptr<Timer> m_timer{};

    std::shared_ptr<Input_system> m_input_system{};
    std::shared_ptr<Render_system> m_render_system{};

public:

    Engine_runtime(const Engine_runtime_descriptor& descriptor) {

        auto device = RHI_device_OpenGL::create();

        auto window = device->create_window(
            descriptor.width, 
            descriptor.height, 
            descriptor.title
        );

        auto input_system = Input_system::create(window);
        auto render_system = Render_system::create(device, window);
        
        m_rhi_device = device;
        m_rhi_window = window;
        m_input_system = input_system;
        m_render_system = render_system;

        m_timer = std::make_shared<Timer>();
        m_timer->start();

        auto test_render_pipeline = Forward_render_pipeline::create(render_system->global_render_resource());
        render_system->set_render_pipeline(test_render_pipeline);

        File_ser::get_instance()->set_root("/home/jinceyang/Desktop/rtr");
        Log_sys::get_instance()->log(Logging_system::Level::info, "Engine Runtime Created");
    }

    std::shared_ptr<RHI_device>& rhi_device() { return m_rhi_device; }
    std::shared_ptr<RHI_window>& rhi_window() { return m_rhi_window; }
    
    std::shared_ptr<Input_system>& input_system() { return m_input_system; }
    std::shared_ptr<Render_system>& render_system() { return m_render_system; }

    std::shared_ptr<World>& world() { return m_world; }

    static std::shared_ptr<Engine_runtime> create(const Engine_runtime_descriptor& descriptor) {
        return std::make_shared<Engine_runtime>(descriptor);
    }

    virtual ~Engine_runtime() {
        Log_sys::get_instance()->log(Logging_system::Level::info, "Engine Runtime Destroyed");
    }

    bool is_active() const { return m_rhi_window->is_active(); }

    void run() {
        while (is_active()) {
            tick(get_delta_time());
        }
    }

    float get_fps() const { return 1000.0f / m_delta_time; }

    float get_delta_time() {
        m_timer->pause();
        m_delta_time = m_timer->elapsed_ms();
        m_timer->start();
        return m_delta_time;
    }

    Swap_data& render_swap_data() { return m_swap_data[m_render_swap_data_index]; }
    Swap_data& logic_swap_data() { return m_swap_data[m_logic_swap_data_index]; }

    void swap() {
        std::swap(m_render_swap_data_index, m_logic_swap_data_index);
    }

    void tick(float delta_time) {
        rhi_window()->on_frame_begin();

        world()->tick(Logic_tick_context{
            m_input_system->state(),
            logic_swap_data(),
            delta_time
        });

        swap();
        logic_swap_data().clear();

        render_system()->tick(Render_tick_context{
            render_swap_data(),
            delta_time
        });

        rhi_device()->check_error();
        rhi_window()->on_frame_end();
    }
    
};
};