#pragma once
#include "engine/global/base.h"
#include "engine/runtime/node.h"
#include "engine/runtime/shader.h"
#include "glm/fwd.hpp"
#include <memory>

namespace rtr {

class Camera : public Node {
protected:
    float m_near_bound{};
    float m_far_bound{};

public:

    Camera(float near_bound, float far_bound) : 
    Node(Node_type::CAMERA), 
    m_near_bound(near_bound), 
    m_far_bound(far_bound) {}

    virtual ~Camera() override = default;

    virtual void upload_uniform(std::shared_ptr<Shader>& shader) override {
        ISet_shader_uniform::upload_uniform(shader);

        shader->add_uniform(
            "view_matrix",
            std::make_shared<Uniform_entry<glm::mat4>>(this->view_matrix())
        );

        shader->add_uniform(
            "projection_matrix",
            std::make_shared<Uniform_entry<glm::mat4>>(this->projection_matrix())
        );

        shader->add_uniform(
            "camera_position",
            std::make_shared<Uniform_entry<glm::vec3>>(this->position())
        );

        shader->add_uniform(
            "near_bound",
            std::make_shared<Uniform_entry<float>>(this->near_bound())
        );

        shader->add_uniform(
            "far_bound",
            std::make_shared<Uniform_entry<float>>(this->far_bound())
        );
    }

    float& near_bound() { return m_near_bound; }
    float& far_bound() { return m_far_bound; }


    [[nodiscard]] float near_bound() const { return m_near_bound; }
    [[nodiscard]] float far_bound() const { return m_far_bound; }

    glm::mat4 view_matrix() {
        glm::vec3 center = m_position + front();
        return glm::lookAt(m_position, center, up());
    }

    virtual glm::mat4 projection_matrix() const = 0;
    virtual void adjust_zoom(float delta_zoom) = 0;
    
};

class Perspective_camera : public Camera {
protected:
    float m_fov{};
    float m_aspect_ratio{};
public:
    Perspective_camera(float fov, float aspect_ratio, float near_bound, float far_bound) :
    Camera(near_bound, far_bound),
    m_fov(fov),
    m_aspect_ratio(aspect_ratio) {}

    ~Perspective_camera() override = default;
    float& fov() { return m_fov; }
    float& aspect_ratio() { return m_aspect_ratio; }
    [[nodiscard]] float fov() const { return m_fov; }
    [[nodiscard]] float aspect_ratio() const { return m_aspect_ratio; }

    [[nodiscard]] glm::mat4 projection_matrix() const override {
        return glm::perspective(glm::radians(m_fov), m_aspect_ratio, m_near_bound, m_far_bound);
    }

    void adjust_zoom(float delta_zoom) override {
        translate(front(), delta_zoom);
    }

};

class Orthographic_camera : public Camera {
protected:
    float m_left_bound{};
    float m_right_bound{};
    float m_top_bound{};
    float m_bottom_bound{};

public:
    Orthographic_camera(float left_bound, float right_bound, float top_bound, float bottom_bound, float near_bound, float far_bound) :
    Camera(near_bound, far_bound),
    m_left_bound(left_bound),
    m_right_bound(right_bound),
    m_top_bound(top_bound),
    m_bottom_bound(bottom_bound) {}

    ~Orthographic_camera() override = default;
    float& left_bound() { return m_left_bound; }
    float& right_bound() { return m_right_bound; }
    float& top_bound() { return m_top_bound; }
    float& bottom_bound() { return m_bottom_bound; }
    [[nodiscard]] float left_bound() const { return m_left_bound; }
    [[nodiscard]] float right_bound() const { return m_right_bound; }
    [[nodiscard]] float top_bound() const { return m_top_bound; }
    [[nodiscard]] float bottom_bound() const { return m_bottom_bound; }

    [[nodiscard]] glm::mat4 projection_matrix() const override {
        return glm::ortho(m_left_bound, m_right_bound, m_bottom_bound, m_top_bound, m_near_bound, m_far_bound);
    }

    void adjust_zoom(float delta_zoom) override {
        m_left_bound -= delta_zoom;
        m_right_bound += delta_zoom;
        m_top_bound += delta_zoom;
        m_bottom_bound -= delta_zoom;
    }

};

}