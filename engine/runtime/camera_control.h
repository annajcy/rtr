#pragma once
#include "engine/global/base.h"
#include "engine/runtime/camera.h"
#include "engine/runtime/input.h"
#include "engine/runtime/rhi/window/rhi_window.h"

namespace rtr {
class Camera_control {
protected:
    std::shared_ptr<Input> m_input{};
    std::shared_ptr<Camera> m_camera{};

    float m_move_speed{0.001f};
    float m_rotate_speed{0.1f};
    float m_zoom_speed{0.01f};

public:

    Camera_control(
        const std::shared_ptr<Camera>& camera, 
        const std::shared_ptr<Input>& input
    ) : m_camera(camera), 
        m_input(input) {}

    virtual ~Camera_control() = default;

    float& move_speed() { return m_move_speed; }
    float& rotate_speed() { return m_rotate_speed; }
    float& zoom_speed() { return m_zoom_speed; }
    [[nodiscard]] float move_speed() const { return m_move_speed; }
    [[nodiscard]] float rotate_speed() const { return m_rotate_speed; }
    [[nodiscard]] float zoom_speed() const { return m_zoom_speed; }

    virtual void update() = 0;
    virtual void pitch(float angle) = 0;
    virtual void yaw(float angle) = 0;

};


class Trackball_camera_control : public Camera_control {

private:
    void pitch(float angle) override {
        auto mat = glm::rotate(glm::identity<glm::mat4>(), glm::radians(angle), m_camera->right());
        m_camera->rotate(angle, m_camera->right());
        m_camera->position() = glm::vec3(mat * glm::vec4(m_camera->position(), 1.0f));
    }

    void yaw(float angle) override {
        auto mat = glm::rotate(glm::identity<glm::mat4>(), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        m_camera->rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f));
        m_camera->position() = glm::vec3(mat * glm::vec4(m_camera->position(), 1.0f));
    }

public:
    Trackball_camera_control(
        const std::shared_ptr<Camera>& camera,
        const std::shared_ptr<Input>& input
    ) : Camera_control(camera, input) {}
    virtual ~Trackball_camera_control() override = default;
    void update() override {

        if (m_input->mouse_button(Mouse_button::LEFT) != Key_action::RELEASE) {
            yaw(m_input->mouse_dx() * m_rotate_speed);
            pitch(m_input->mouse_dx() * m_rotate_speed);
        } else if (m_input->mouse_button(Mouse_button::MIDDLE)!= Key_action::RELEASE) {
            m_camera->translate(m_camera->up(), m_input->mouse_dy() * m_move_speed);
            m_camera->translate(m_camera->right(), m_input->mouse_dx() * m_move_speed);
        }

        auto delta_scale = m_input->mouse_scroll_dy();
        if (std::abs(delta_scale) > EPSILON) {
            m_camera->adjust_zoom(delta_scale * m_zoom_speed);
        }
    }
};


class Game_camera_control : public Camera_control {
private:
    void pitch(float angle) override {
        m_camera->rotate(angle, m_camera->right());
    }

    void yaw(float angle) override {
        m_camera->rotate(angle, m_camera->up());
    }

public:
    Game_camera_control(
        const std::shared_ptr<Camera>& camera,
        const std::shared_ptr<Input>& input
    ) : Camera_control(camera, input) {}
    virtual ~Game_camera_control() override = default;
    void update() override {

        if (m_input->mouse_button(Mouse_button::LEFT)!= Key_action::RELEASE) {
            yaw(m_input->mouse_dx() * m_rotate_speed);
            pitch(m_input->mouse_dy() * m_rotate_speed);
        }

        glm::vec3 direction = glm::zero<glm::vec3>();

        if (m_input->key(Key_code::W)!= Key_action::RELEASE) {
            direction += m_camera->front();
        }

        if (m_input->key(Key_code::S)!= Key_action::RELEASE) {
            direction += m_camera->back();
        }

        if (m_input->key(Key_code::A)!= Key_action::RELEASE) {
            direction += m_camera->left();
        }

        if (m_input->key(Key_code::D)!= Key_action::RELEASE) {
            direction += m_camera->right();
        }

        if (m_input->key(Key_code::Q)!= Key_action::RELEASE) {
            direction += m_camera->up();
        }

        if (m_input->key(Key_code::E)!= Key_action::RELEASE) {
            direction += m_camera->down();
        }

        if (std::abs(glm::length(direction)) > EPSILON) {
            m_camera->translate(glm::normalize(direction), m_move_speed);
        }

        auto delta_scale = m_input->mouse_scroll_dy();

        if (std::abs(delta_scale) > DBL_EPSILON) {
            m_camera->adjust_zoom(delta_scale * m_zoom_speed);
        }
        
    }
};



};