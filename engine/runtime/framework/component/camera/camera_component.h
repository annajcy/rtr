#pragma once

#include "../component_base.h"
#include "../node/node_component.h"
#include "engine/runtime/platform/rhi/rhi_window.h"
#include <memory>

namespace rtr {

enum class Camera_type {
    PERSPECTIVE,
    ORTHOGRAPHIC, 
};

class Camera_component : public Component_base {

protected:
        
    float m_near_bound{0.1f};
    float m_far_bound{100.0f};
    Camera_type m_camera_type{};
    std::shared_ptr<Node_component> m_node{};

public:
    Camera_component(
        Camera_type type 
    ) : Component_base(Component_type::CAMERA), m_camera_type(type) {}

    Camera_component(
        Camera_type type,
        float near_bound, 
        float far_bound
    ) : Component_base(Component_type::CAMERA),
        m_near_bound(near_bound), 
        m_far_bound(far_bound) {}

    virtual ~Camera_component() override = default;

    void on_add_to_game_object() override {
        set_node(component_list()->get_component<Node_component>()); 
    }

    float& near_bound() { return m_near_bound; }
    float& far_bound() { return m_far_bound; }

    float near_bound() const { return m_near_bound; }
    float far_bound() const { return m_far_bound; }

    void set_node(const std::shared_ptr<Node_component>& node) {
        m_node = node;
        set_priority(node->priority() + 1);
    }

    const std::shared_ptr<Node_component>& node() const {
        return m_node; 
    }

    glm::mat4 view_matrix() const {
        glm::vec3 center = node()->world_position() + node()->world_front();
        return glm::lookAt(node()->world_position(), center, node()->world_up());
    }

    Camera_type camera_type() const { return m_camera_type; }

    virtual glm::mat4 projection_matrix() const = 0;
    virtual void adjust_zoom(float delta_zoom) = 0;

    void tick(const Logic_tick_context& tick_context) override {
    
        auto& data = tick_context.logic_swap_data;

        data.camera = Swap_camera{
            .view_matrix = view_matrix(),
            .projection_matrix = projection_matrix(),
            .camera_position = node()->world_position(),
        };

    }

    virtual void add_resize_callback(const std::shared_ptr<RHI_window>& window) {}
    
};

class Perspective_camera_component : public Camera_component {

protected:
    float m_fov{45.0f};
    float m_aspect_ratio{16 / 9.0f};

public:
    Perspective_camera_component() : Camera_component(Camera_type::PERSPECTIVE) {}

    Perspective_camera_component(
        float fov, 
        float aspect_ratio, 
        float near_bound, 
        float far_bound
    ) : Camera_component( Camera_type::PERSPECTIVE, near_bound, far_bound),
        m_fov(fov),
        m_aspect_ratio(aspect_ratio) {}

    static std::shared_ptr<Perspective_camera_component> create(float fov, float aspect_ratio, float near_bound, float far_bound) {
        return std::make_shared<Perspective_camera_component>(fov, aspect_ratio, near_bound, far_bound); 
    }

    ~Perspective_camera_component() override = default;
    float& fov() { return m_fov; }
    float& aspect_ratio() { return m_aspect_ratio; }
    float fov() const { return m_fov; }
    float aspect_ratio() const { return m_aspect_ratio; }

    glm::mat4 projection_matrix() const override {
        return glm::perspective(glm::radians(m_fov), m_aspect_ratio, m_near_bound, m_far_bound);
    }

    void adjust_zoom(float delta_zoom) override {
        node()->translate(node()->front(), delta_zoom);
    }

    void add_resize_callback(const std::shared_ptr<RHI_window>& window) override {
        m_aspect_ratio = (float)window->width() / (float)window->height();
        window->window_resize_event().add([&](int width, int height) {
            m_aspect_ratio = (float)width / (float)height;
        }); 
    }

};

class Orthographic_camera_component : public Camera_component {

protected:
    float m_left_bound{-5.0f};
    float m_right_bound{5.0f};
    float m_top_bound{5.0f};
    float m_bottom_bound{-5.0f};

public:
    Orthographic_camera_component() : Camera_component(Camera_type::ORTHOGRAPHIC) {}

    Orthographic_camera_component(
        float left_bound, 
        float right_bound, 
        float top_bound, 
        float bottom_bound, 
        float near_bound, 
        float far_bound
    ) : Camera_component(Camera_type::ORTHOGRAPHIC, near_bound, far_bound),
        m_left_bound(left_bound),
        m_right_bound(right_bound),
        m_top_bound(top_bound),
        m_bottom_bound(bottom_bound) {}

    static std::shared_ptr<Orthographic_camera_component> create(float left_bound, float right_bound, float top_bound, float bottom_bound, float near_bound, float far_bound) {
        return std::make_shared<Orthographic_camera_component>(left_bound, right_bound, top_bound, bottom_bound, near_bound, far_bound);
    }

    ~Orthographic_camera_component() override = default;

    float& left_bound() { return m_left_bound; }
    float& right_bound() { return m_right_bound; }
    float& top_bound() { return m_top_bound; }
    float& bottom_bound() { return m_bottom_bound; }

    float left_bound() const { return m_left_bound; }
    float right_bound() const { return m_right_bound; }
    float top_bound() const { return m_top_bound; }
    float bottom_bound() const { return m_bottom_bound; }

    glm::mat4 projection_matrix() const override {
        return glm::ortho(m_left_bound, m_right_bound, m_bottom_bound, m_top_bound, m_near_bound, m_far_bound);
    }

    void adjust_zoom(float delta_zoom) override {
        m_left_bound -= delta_zoom;
        m_right_bound += delta_zoom;
        m_top_bound += delta_zoom;
        m_bottom_bound -= delta_zoom;
    }

};

};
