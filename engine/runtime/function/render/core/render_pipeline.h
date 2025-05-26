#pragma once

#include "engine/runtime/context/engine_tick_context.h"
#include "engine/runtime/function/render/object/memory_buffer.h"
#include "engine/runtime/function/render/object/texture.h"
#include "engine/runtime/function/render/core/render_pass.h"
#include "engine/runtime/function/render/core/render_resource.h"
#include "engine/runtime/function/render/core/render_struct.h"
#include "engine/runtime/resource/resource_manager.h"
#include "glm/fwd.hpp"
#include <memory>

namespace rtr {

class Render_pipeline {
protected:
    RHI_global_render_resource& m_rhi_global_render_resource;
    Resource_manager<std::string, Render_resource> m_render_resource_manager{};

public:
    Render_pipeline(RHI_global_render_resource& global_render_resource) : 
    m_rhi_global_render_resource(global_render_resource) {}

    virtual ~Render_pipeline() {}

    virtual void execute(const Render_tick_context& tick_context) = 0;
   
    virtual void init_ubo() = 0;
    virtual void update_ubo(const Render_tick_context& tick_context) = 0;

    virtual void init_render_passes() = 0;
    virtual void update_render_pass(const Render_tick_context& tick_context) = 0;

    virtual void update_render_resource(const Render_tick_context& tick_context) = 0;
};


class Forward_render_pipeline : public Render_pipeline {
private:

    std::shared_ptr<Uniform_buffer<Camera_ubo>> m_camera_ubo{};
    std::shared_ptr<Uniform_buffer<Directional_light_ubo_array>> m_directional_light_ubo_array{};
    std::shared_ptr<Uniform_buffer<Point_light_ubo_array>> m_point_light_ubo_array{};
    std::shared_ptr<Uniform_buffer<Spot_light_ubo_array>> m_spot_light_ubo_array{};
    std::shared_ptr<Uniform_buffer<Orthographic_camera_ubo>> m_dl_shadow_camera_ubo{};

    std::shared_ptr<Main_pass> m_main_pass{};
    std::shared_ptr<Postprocess_pass> m_postprocess_pass{};
    std::shared_ptr<Shadow_pass> m_shadow_pass{};
    
public:
    Forward_render_pipeline (
        RHI_global_render_resource& global_render_resource
    ) : Render_pipeline(global_render_resource) {
        init_ubo();
        init_render_passes();
    }

    ~Forward_render_pipeline() {}

    void update_render_resource(const Render_tick_context& tick_context) override {

        auto color_attachment = Texture_2D::create_color_attachemnt(
            m_rhi_global_render_resource.window->width(), 
            m_rhi_global_render_resource.window->height()
        );
        color_attachment->link(m_rhi_global_render_resource.device);

        auto depth_stencil_attachment = Texture_2D::create_depth_stencil_attachemnt(
            m_rhi_global_render_resource.window->width(),
            m_rhi_global_render_resource.window->height()
        );
        depth_stencil_attachment->link(m_rhi_global_render_resource.device);

        auto dl_shadow_map = tick_context.render_swap_data.dl_shadow_casters.shadow_map;
       
        if (!dl_shadow_map->is_linked()) dl_shadow_map->link(m_rhi_global_render_resource.device);
        dl_shadow_map->rhi_resource()->set_border_color(glm::vec4(1.0f));

        m_render_resource_manager.add("main_color_attachment", color_attachment);
        m_render_resource_manager.add("main_depth_attachment", depth_stencil_attachment);
        m_render_resource_manager.add("shadow map", dl_shadow_map);
    }

    void init_ubo() override {
        m_camera_ubo = Uniform_buffer<Camera_ubo>::create(Camera_ubo{});
        if (!m_camera_ubo->is_linked()) m_camera_ubo->link(m_rhi_global_render_resource.device);
        m_rhi_global_render_resource.memory_binder->bind_memory_buffer(m_camera_ubo->rhi_resource(), 0);

        m_directional_light_ubo_array = Uniform_buffer<Directional_light_ubo_array>::create(Directional_light_ubo_array{});
        if (!m_directional_light_ubo_array->is_linked()) m_directional_light_ubo_array->link(m_rhi_global_render_resource.device);
        m_rhi_global_render_resource.memory_binder->bind_memory_buffer(m_directional_light_ubo_array->rhi_resource(), 1);

        m_point_light_ubo_array = Uniform_buffer<Point_light_ubo_array>::create(Point_light_ubo_array{});
        if (!m_point_light_ubo_array->is_linked()) m_point_light_ubo_array->link(m_rhi_global_render_resource.device);
        m_rhi_global_render_resource.memory_binder->bind_memory_buffer(m_point_light_ubo_array->rhi_resource(), 2);
        
        m_spot_light_ubo_array = Uniform_buffer<Spot_light_ubo_array>::create(Spot_light_ubo_array{});
        if (!m_spot_light_ubo_array->is_linked()) m_spot_light_ubo_array->link(m_rhi_global_render_resource.device);
        m_rhi_global_render_resource.memory_binder->bind_memory_buffer(m_spot_light_ubo_array->rhi_resource(), 3);

        m_dl_shadow_camera_ubo = Uniform_buffer<Orthographic_camera_ubo>::create(Orthographic_camera_ubo{});
        if (!m_dl_shadow_camera_ubo->is_linked()) m_dl_shadow_camera_ubo->link(m_rhi_global_render_resource.device);
        m_rhi_global_render_resource.memory_binder->bind_memory_buffer(m_dl_shadow_camera_ubo->rhi_resource(), 4);
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
        m_shadow_pass = Shadow_pass::create(m_rhi_global_render_resource);
        m_main_pass = Main_pass::create(m_rhi_global_render_resource);
        m_postprocess_pass = Postprocess_pass::create(m_rhi_global_render_resource);
    }

    void update_render_pass(const Render_tick_context& tick_context) override {

        m_shadow_pass->set_resource_flow(Shadow_pass::Resource_flow{
            .shadow_map_out = m_render_resource_manager.get<Texture_2D>("shadow map")
        });
        m_shadow_pass->set_context(Shadow_pass::Execution_context{
            .shadow_caster_swap_objects = tick_context.render_swap_data.get_shadow_casters(),
        });

        m_main_pass->set_resource_flow(Main_pass::Resource_flow{
            .color_attachment_out = m_render_resource_manager.get<Texture_2D>("main_color_attachment"),
            .depth_attachment_out = m_render_resource_manager.get<Texture_2D>("main_depth_attachment"),
            .shadow_map_in = m_render_resource_manager.get<Texture_2D>("shadow map")
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

    static std::shared_ptr<Forward_render_pipeline> create(RHI_global_render_resource& global_render_resource) {
        return std::make_shared<Forward_render_pipeline>(global_render_resource);
    }

};

}