#pragma once

#include "engine/runtime/context/tick_context/render_tick_context.h"
#include "engine/runtime/function/render/frontend/memory_buffer.h"
#include "engine/runtime/function/render/frontend/shader.h"
#include "engine/runtime/function/render/frontend/texture.h"
#include "engine/runtime/function/render/material/setting.h"
#include "engine/runtime/function/render/pass/main_pass.h"
#include "engine/runtime/function/render/pass/postprocess_pass.h"
#include "engine/runtime/function/render/pass/shadow_pass.h"
#include "engine/runtime/function/render/pipeline/base_pipeline.h"
#include "engine/runtime/function/render/struct/camera_render_struct.h"
#include "engine/runtime/function/render/struct/light_render_struct.h"
#include "engine/runtime/platform/rhi/rhi_shader_code.h"
#include "engine/runtime/resource/resource_manager.h"
#include "glm/fwd.hpp"
#include <memory>

namespace rtr {

class Forward_pipeline : public Base_pipeline {
private:

    std::shared_ptr<Shadow_setting> m_shadow_setting{};
    std::shared_ptr<Parallax_setting> m_parallax_setting{};

    std::shared_ptr<Uniform_buffer<Camera_ubo>> m_camera_ubo{};
    std::shared_ptr<Uniform_buffer<Directional_light_ubo_array>> m_directional_light_ubo_array{};
    std::shared_ptr<Uniform_buffer<Point_light_ubo_array>> m_point_light_ubo_array{};
    std::shared_ptr<Uniform_buffer<Spot_light_ubo_array>> m_spot_light_ubo_array{};
    std::shared_ptr<Uniform_buffer<Orthographic_camera_ubo>> m_dl_shadow_camera_ubo{};

    std::shared_ptr<Main_pass> m_main_pass{};
    std::shared_ptr<Postprocess_pass> m_postprocess_pass{};
    std::shared_ptr<Shadow_pass> m_shadow_pass{};
    
public:
    Forward_pipeline (
        RHI_global_resource& rhi_global_resource
    ) : Base_pipeline(rhi_global_resource), 
        m_shadow_setting(Shadow_setting::create()),
        m_parallax_setting(Parallax_setting::create()) {
        init_ubo();
        init_render_passes();
    }

    ~Forward_pipeline() {}

    std::shared_ptr<Shadow_setting> shadow_setting() {
        return m_shadow_setting;
    }

    std::shared_ptr<Parallax_setting> parallax_setting() {
        return m_parallax_setting;
    }

    void update_render_resource(const Render_tick_context& tick_context) override {

        m_render_resource_manager.add("main_color_attachment", Texture_2D::create_color_attachemnt_rgba(
            m_rhi_global_resource.window->width(), 
            m_rhi_global_resource.window->height()
        ));

        auto dl_shadow_map = tick_context.render_swap_data.dl_shadow_casters.shadow_map;
        auto dl_shadow_map_rhi = dl_shadow_map->rhi(m_rhi_global_resource.device);
        dl_shadow_map_rhi->set_border_color(glm::vec4(1.0f));
        m_render_resource_manager.add("shadow_map", dl_shadow_map);
    }

    void init_ubo() override {
        m_camera_ubo = Uniform_buffer<Camera_ubo>::create(Camera_ubo{});
        m_rhi_global_resource.memory_binder->bind_memory_buffer(m_camera_ubo->rhi(m_rhi_global_resource.device), 0);

        m_directional_light_ubo_array = Uniform_buffer<Directional_light_ubo_array>::create(Directional_light_ubo_array{});
        m_rhi_global_resource.memory_binder->bind_memory_buffer(m_directional_light_ubo_array->rhi(m_rhi_global_resource.device), 1);

        m_point_light_ubo_array = Uniform_buffer<Point_light_ubo_array>::create(Point_light_ubo_array{});
        m_rhi_global_resource.memory_binder->bind_memory_buffer(m_point_light_ubo_array->rhi(m_rhi_global_resource.device), 2);
        
        m_spot_light_ubo_array = Uniform_buffer<Spot_light_ubo_array>::create(Spot_light_ubo_array{});
        m_rhi_global_resource.memory_binder->bind_memory_buffer(m_spot_light_ubo_array->rhi(m_rhi_global_resource.device), 3);

        m_dl_shadow_camera_ubo = Uniform_buffer<Orthographic_camera_ubo>::create(Orthographic_camera_ubo{});
        m_rhi_global_resource.memory_binder->bind_memory_buffer(m_dl_shadow_camera_ubo->rhi(m_rhi_global_resource.device), 4);
    }

    void update_ubo(const Render_tick_context& tick_context) override {

        m_camera_ubo->set_data(Camera_ubo{
            .view_matrix = tick_context.render_swap_data.camera.view_matrix,
            .projection_matrix = tick_context.render_swap_data.camera.projection_matrix,
            .camera_position = tick_context.render_swap_data.camera.camera_position,
            .camera_direction = tick_context.render_swap_data.camera.camera_direction,
            .near = tick_context.render_swap_data.camera.near,
            .far = tick_context.render_swap_data.camera.far
        });
        m_camera_ubo->push_to_rhi();

        auto dl_ubo_arr = Directional_light_ubo_array{};
        dl_ubo_arr.count = tick_context.render_swap_data.directional_lights.size();
        for (size_t i = 0; i < tick_context.render_swap_data.directional_lights.size(); i++) {
            dl_ubo_arr.directional_light_ubo[i] = Directional_light_ubo{
                .intensity = tick_context.render_swap_data.directional_lights[i].intensity,
                .color = tick_context.render_swap_data.directional_lights[i].color,
                .direction = tick_context.render_swap_data.directional_lights[i].direction,
            };
        }

        m_directional_light_ubo_array->set_data(dl_ubo_arr);
        m_directional_light_ubo_array->push_to_rhi();

        auto pl_ubo_arr = Point_light_ubo_array{};
        pl_ubo_arr.count = tick_context.render_swap_data.point_lights.size();
        for (size_t i = 0; i < tick_context.render_swap_data.point_lights.size(); i++) {
            pl_ubo_arr.point_light_ubo[i] = Point_light_ubo{
                .intensity = tick_context.render_swap_data.point_lights[i].intensity,
                .position = tick_context.render_swap_data.point_lights[i].position,
                .color = tick_context.render_swap_data.point_lights[i].color,
                .attenuation = tick_context.render_swap_data.point_lights[i].attenuation,
            };
        }

        m_point_light_ubo_array->set_data(pl_ubo_arr);
        m_point_light_ubo_array->push_to_rhi();

        auto sl_ubo_arr = Spot_light_ubo_array{};
        sl_ubo_arr.count = tick_context.render_swap_data.spot_lights.size();
        for (size_t i = 0; i < tick_context.render_swap_data.spot_lights.size(); i++) {
            sl_ubo_arr.spot_light_ubo[i] = Spot_light_ubo{
                .intensity = tick_context.render_swap_data.spot_lights[i].intensity,
                .inner_angle_cos = tick_context.render_swap_data.spot_lights[i].inner_angle_cos,
                .outer_angle_cos = tick_context.render_swap_data.spot_lights[i].outer_angle_cos,
                .direction = tick_context.render_swap_data.spot_lights[i].direction,
                .position = tick_context.render_swap_data.spot_lights[i].position,
                .color = tick_context.render_swap_data.spot_lights[i].color,
            };
        }

        m_spot_light_ubo_array->set_data(sl_ubo_arr);
        m_spot_light_ubo_array->push_to_rhi();

        auto dl_shadow_camera_ubo = Orthographic_camera_ubo{
            .view_matrix = tick_context.render_swap_data.dl_shadow_casters.shadow_camera.view_matrix,
            .projection_matrix = tick_context.render_swap_data.dl_shadow_casters.shadow_camera.projection_matrix,
            .camera_position = tick_context.render_swap_data.dl_shadow_casters.shadow_camera.camera_position,
            .camera_direction = tick_context.render_swap_data.dl_shadow_casters.shadow_camera.camera_direction,
            .near = tick_context.render_swap_data.dl_shadow_casters.shadow_camera.near,
            .far = tick_context.render_swap_data.dl_shadow_casters.shadow_camera.far,
            .left = tick_context.render_swap_data.dl_shadow_casters.shadow_camera.left,
            .right = tick_context.render_swap_data.dl_shadow_casters.shadow_camera.right,
            .bottom = tick_context.render_swap_data.dl_shadow_casters.shadow_camera.bottom,
            .top = tick_context.render_swap_data.dl_shadow_casters.shadow_camera.top
        };

        m_dl_shadow_camera_ubo->set_data(dl_shadow_camera_ubo);
        m_dl_shadow_camera_ubo->push_to_rhi();
    }

    void init_render_passes() override {
        m_shadow_pass = Shadow_pass::create(m_rhi_global_resource);
        m_main_pass = Main_pass::create(m_rhi_global_resource);
        m_postprocess_pass = Postprocess_pass::create(m_rhi_global_resource);
    }

    void update_render_pass(const Render_tick_context& tick_context) override {

        m_shadow_pass->set_resource_flow(Shadow_pass::Resource_flow{
            .shadow_map_out = m_render_resource_manager.get<Texture_2D>("shadow_map")
        });
        m_shadow_pass->set_context(Shadow_pass::Execution_context{
            .shadow_caster_swap_objects = tick_context.render_swap_data.get_shadow_casters(),
        });

        m_main_pass->set_resource_flow(Main_pass::Resource_flow{
            .color_attachment_out = m_render_resource_manager.get<Texture_2D>("main_color_attachment"),
            .shadow_map_in = m_render_resource_manager.get<Texture_2D>("shadow_map")
        });
        m_main_pass->set_context(Main_pass::Execution_context{
            .skybox = tick_context.render_swap_data.skybox,
            .render_swap_objects = tick_context.render_swap_data.render_objects
        });
        
        m_postprocess_pass->set_context(Postprocess_pass::Execution_context{});
        m_postprocess_pass->set_resource_flow(Postprocess_pass::Resource_flow{
            .texture_in = m_render_resource_manager.get<Texture_2D>("main_color_attachment")
        });
    }

    void execute(const Render_tick_context& tick_context) override {
        m_shadow_pass->excute();
        m_main_pass->excute();
        m_postprocess_pass->excute();
    }

    static std::shared_ptr<Forward_pipeline> create(RHI_global_resource& rhi_global_resource) {
        return std::make_shared<Forward_pipeline>(rhi_global_resource);
    }

};

}