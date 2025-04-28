#pragma once

#include "../component_base.h"
#include "../node/node_component.h"
#include "engine/runtime/framework/object/camera/camera.h"
#include "engine/runtime/platform/rhi/rhi_window.h"
#include <memory>

namespace rtr {


class Camera_component : public Component_base {
protected:
    std::shared_ptr<Camera> m_camera{};

public:
    Camera_component() : Component_base(Component_type::CAMERA) {}

    virtual ~Camera_component() override = default;

    const std::shared_ptr<Camera>& camera() const { return m_camera; }
    std::shared_ptr<Camera>& camera() { return m_camera; }

    void tick(const Logic_tick_context& tick_context) override {
    
        auto& data = tick_context.logic_swap_data;

        data.camera = Swap_camera{
            .view_matrix = m_camera->view_matrix(),
            .projection_matrix = m_camera->projection_matrix(),
            .camera_position = m_camera->node()->world_position(),
        };
    }

    virtual void add_resize_callback(const std::shared_ptr<RHI_window>& window) = 0;
    
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

    void add_resize_callback(const std::shared_ptr<RHI_window>& window) override {
        perspective_camera()->aspect_ratio() = (float)window->width() / (float)window->height();
        window->window_resize_event().add([&](int width, int height) {
            perspective_camera()->aspect_ratio() = (float)width / (float)height;
        }); 
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

    std::shared_ptr<Orthographic_camera_component> orthographic_camera_component() { 
        return std::dynamic_pointer_cast<Orthographic_camera_component>(m_camera); 
    }

    void add_resize_callback(const std::shared_ptr<RHI_window>& window) override {}

    void on_add_to_game_object() override {
        auto node = get_component<Node_component>()->node();
        m_camera = Orthographic_camera::create(node); 
    }

};

};
