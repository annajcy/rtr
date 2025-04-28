#pragma once

#include "engine/runtime/framework/object/node.h"
#include <memory>

namespace rtr {

enum class Camera_type {
    PERSPECTIVE,
    ORTHOGRAPHIC, 
};

class Camera {

protected:
        
    float m_near_bound{0.1f};
    float m_far_bound{100.0f};
    Camera_type m_camera_type{};
    std::shared_ptr<Node> m_node{};

public:
    Camera(
        Camera_type type,
        const std::shared_ptr<Node>& node
    ) : m_camera_type(type), m_node(node) {}

    virtual ~Camera() = default;

    float& near_bound() { return m_near_bound; }
    float& far_bound() { return m_far_bound; }

    const float& near_bound() const { return m_near_bound; }
    const float& far_bound() const { return m_far_bound; }

    const std::shared_ptr<Node>& node() const { return m_node; }
    std::shared_ptr<Node>& node() { return m_node; }

    glm::mat4 view_matrix() const {
        glm::vec3 center = node()->world_position() + node()->world_front();
        return glm::lookAt(node()->world_position(), center, node()->world_up());
    }

    Camera_type camera_type() const { return m_camera_type; }

    virtual glm::mat4 projection_matrix() const = 0;
    virtual void adjust_zoom(float delta_zoom) = 0;
    
};

class Perspective_camera : public Camera {

protected:
    float m_fov{45.0f};
    float m_aspect_ratio{16 / 9.0f};

public:
    Perspective_camera(
        const std::shared_ptr<Node>& node
    ) : Camera(Camera_type::PERSPECTIVE, node) {}

    static std::shared_ptr<Perspective_camera> create(const std::shared_ptr<Node>& node) {
        return std::make_shared<Perspective_camera>(node); 
    }

    ~Perspective_camera() override = default;

    float& fov() { return m_fov; }
    float& aspect_ratio() { return m_aspect_ratio; }

    const float& fov() const { return m_fov; }
    const float& aspect_ratio() const { return m_aspect_ratio; }

    glm::mat4 projection_matrix() const override {
        return glm::perspective(
            glm::radians(m_fov), 
            m_aspect_ratio, 
            m_near_bound, 
            m_far_bound
        );
    }

    void adjust_zoom(float delta_zoom) override {
        node()->translate(
            node()->front(), 
            delta_zoom
        );
    }

};

class Orthographic_camera : public Camera {

protected:

    float m_left_bound{-5.0f};
    float m_right_bound{5.0f};
    float m_top_bound{5.0f};
    float m_bottom_bound{-5.0f};

public:
    Orthographic_camera(
        const std::shared_ptr<Node>& node
    ) : Camera(Camera_type::ORTHOGRAPHIC, node) {
        m_far_bound = 5.0f;
        m_near_bound = -5.0f;
    }

    static std::shared_ptr<Orthographic_camera> create(const std::shared_ptr<Node>& node) {
        return std::make_shared<Orthographic_camera>(node);
    }

    ~Orthographic_camera() override = default;

    float& left_bound() { return m_left_bound; }
    float& right_bound() { return m_right_bound; }
    float& top_bound() { return m_top_bound; }
    float& bottom_bound() { return m_bottom_bound; }

    const float& left_bound() const { return m_left_bound; }
    const float& right_bound() const { return m_right_bound; }
    const float& top_bound() const { return m_top_bound; }
    const float& bottom_bound() const { return m_bottom_bound; }

    glm::mat4 projection_matrix() const override {
        return glm::ortho(
            m_left_bound, 
            m_right_bound, 
            m_bottom_bound, 
            m_top_bound, 
            m_near_bound, 
            m_far_bound
        );
    }

    void adjust_zoom(float delta_zoom) override {
        m_left_bound -= delta_zoom;
        m_right_bound += delta_zoom;
        m_top_bound += delta_zoom;
        m_bottom_bound -= delta_zoom;
    }

};

};
