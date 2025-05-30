#pragma once

#include "engine/runtime/context/tick_context/render_tick_context.h"
#include "engine/runtime/context/tick_context/logic_tick_context.h"
#include "engine/runtime/function/input/input_system.h"
#include "engine/runtime/function/render/render_system.h"
#include "engine/runtime/global/logger.h"
#include "engine/runtime/global/timer.h"
#include "engine/runtime/platform/rhi/opengl/rhi_device_opengl.h"
#include "engine/runtime/framework/core/world.h"
#include "engine/runtime/platform/rhi/rhi_device.h"
#include "engine/runtime/platform/rhi/rhi_renderer.h"
#include "engine/runtime/resource/file_service.h"
#include <memory>

namespace rtr {

struct Engine_runtime_descriptor {
    int width{800};
    int height{600};
    std::string title{"RTR Engine"};
    API_type api_type{API_type::OPENGL};
    Clear_state clear_state{Clear_state::enabled()};
};

class Engine_runtime {
private:
    float m_delta_time {0.0f};

    Swap_data m_swap_data[2];
    int m_render_swap_data_index = 0;
    int m_logic_swap_data_index = 1;

    RHI_global_resource m_rhi_global_resource{};
    
    std::shared_ptr<World> m_world{};
    std::shared_ptr<Timer> m_timer{};

    std::shared_ptr<Input_system> m_input_system{};
    std::shared_ptr<Render_system> m_render_system{};

public:

    Engine_runtime(const Engine_runtime_descriptor& descriptor) {

        std::shared_ptr<RHI_device> device{};

        if (descriptor.api_type == API_type::OPENGL) {
            device = RHI_device_OpenGL::create();
        } else {
            throw std::runtime_error("Unsupported API type");
        }

        auto window = device->create_window(
            descriptor.width, 
            descriptor.height, 
            descriptor.title
        );

        m_rhi_global_resource.device = device;
        m_rhi_global_resource.window = window;
        m_rhi_global_resource.renderer = device->create_renderer(descriptor.clear_state);
        m_rhi_global_resource.screen_buffer = device->create_screen_buffer(window);
        m_rhi_global_resource.memory_binder = device->create_memory_buffer_binder();
        m_rhi_global_resource.pipeline_state = device->create_pipeline_state();
        m_rhi_global_resource.texture_builder = device->create_texture_builder();

        auto input_system = Input_system::create(m_rhi_global_resource.window);
        auto render_system = Render_system::create(m_rhi_global_resource);
        
        m_input_system = input_system;
        m_render_system = render_system;

        m_timer = std::make_shared<Timer>();
        m_timer->start();

        File_ser::get_instance()->set_root("/home/jinceyang/Desktop/rtr");
        Log_sys::get_instance()->log(Logging_system::Level::info, "Engine Runtime Created");
    }
    
    std::shared_ptr<Input_system>& input_system() { return m_input_system; }
    std::shared_ptr<Render_system>& render_system() { return m_render_system; }

    std::shared_ptr<World>& world() { return m_world; }

    static std::shared_ptr<Engine_runtime> create(const Engine_runtime_descriptor& descriptor) {
        return std::make_shared<Engine_runtime>(descriptor);
    }

    virtual ~Engine_runtime() {
        Log_sys::get_instance()->log(Logging_system::Level::info, "Engine Runtime Destroyed");
    }

    bool is_active() const { 
        return m_rhi_global_resource.window->is_active(); 
    }

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

        m_rhi_global_resource.window->on_frame_begin();

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

        m_rhi_global_resource.device->check_error();
        m_rhi_global_resource.window->on_frame_end();
    }

    RHI_global_resource& rhi_global_resource() {
        return m_rhi_global_resource;
    }

    const RHI_global_resource& rhi_global_resource() const {
        return m_rhi_global_resource;
    }
    
};
};