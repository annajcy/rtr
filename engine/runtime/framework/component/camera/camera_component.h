#pragma once

#include "engine/runtime/framework/component/camera/camera.h"
#include "engine/runtime/framework/component/component_base.h"
#include "engine/runtime/framework/component/node/node_component.h"
#include <memory>

namespace rtr {


class Camera_component : public Component_base {
protected:
    std::shared_ptr<Camera> m_camera{};
    bool m_is_shadow_caster{false};

public:
    Camera_component() : Component_base(Component_type::CAMERA) {}

    virtual ~Camera_component() override = default;

    const std::shared_ptr<Camera>& camera() const { return m_camera; }
    std::shared_ptr<Camera>& camera() { return m_camera; }

    const bool& is_shadow_caster() const { return m_is_shadow_caster; }
    bool& is_shadow_caster() { return m_is_shadow_caster; }

    void tick(const Logic_tick_context& tick_context) override {
    
        auto& data = tick_context.logic_swap_data;

        if (m_is_shadow_caster) {
            data.light_camera = Swap_camera{
                .view_matrix = m_camera->view_matrix(),
                .projection_matrix = m_camera->projection_matrix(),
                .camera_position = m_camera->node()->world_position(),
                .camera_direction = m_camera->node()->world_front(),
                .near = m_camera->near_bound(),
                .far = m_camera->far_bound()
            };
        } else {
            data.camera = Swap_camera{
                .view_matrix = m_camera->view_matrix(),
                .projection_matrix = m_camera->projection_matrix(),
                .camera_position = m_camera->node()->world_position(),
                .camera_direction = m_camera->node()->world_front(),
                .near = m_camera->near_bound(),
                .far = m_camera->far_bound()
            };
        }
    }

    void add_resize_callback(const std::shared_ptr<RHI_window>& window) {
        m_camera->set_aspect_ratio((float)window->width() / (float)window->height());
        window->window_resize_event().add([&](int width, int height) {
            m_camera->set_aspect_ratio((float)width / (float)height); 
        });
    }

    
    
};

class Perspective_camera_component : public Camera_component {

public:
    Perspective_camera_component() {}
    ~Perspective_camera_component() override = default;

    static std::shared_ptr<Perspective_camera_component> create() {
        return std::make_shared<Perspective_camera_component>(); 
    }

    std::shared_ptr<Perspective_camera> perspective_camera() { 
        return std::dynamic_pointer_cast<Perspective_camera>(m_camera); 
    }
    
    void on_add_to_game_object() override {
        auto node = get_component<Node_component>()->node();
        m_camera = Perspective_camera::create(node); 
    }

};

class Orthographic_camera_component : public Camera_component {

public:
    Orthographic_camera_component() {}
    ~Orthographic_camera_component() override = default;

    static std::shared_ptr<Orthographic_camera_component> create() {
        return std::make_shared<Orthographic_camera_component>(); 
    }

    std::shared_ptr<Orthographic_camera> orthographic_camera() { 
        return std::dynamic_pointer_cast<Orthographic_camera>(m_camera); 
    }

    void on_add_to_game_object() override {
        auto node = get_component<Node_component>()->node();
        m_camera = Orthographic_camera::create(node); 
    }

};

};
