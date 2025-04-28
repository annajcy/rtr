#pragma once

#include "engine/runtime/framework/object/camera/camera.h"
#include "engine/runtime/function/input/input_system.h"

#include <memory>

namespace rtr {

enum class Camera_control_type {
    TRACKBALL,
    GAME, 
};

class Camera_control  {

protected:
    std::shared_ptr<Camera> m_camera{};
    Camera_control_type m_camera_control_type{};
    
    float m_move_speed{0.001f};
    float m_rotate_speed{0.05f};
    float m_zoom_speed{0.01f};

public:

    Camera_control(
        Camera_control_type camera_control_type,
        const std::shared_ptr<Camera>& camera
    ) : m_camera(camera) {}

    virtual ~Camera_control() = default;

    Camera_control_type camera_control_type() const { return m_camera_control_type; }

    float& move_speed() { return m_move_speed; }
    float& rotate_speed() { return m_rotate_speed; }
    float& zoom_speed() { return m_zoom_speed; }
    const float& move_speed() const { return m_move_speed; }
    const float& rotate_speed() const { return m_rotate_speed; }
    const float& zoom_speed() const { return m_zoom_speed; }

    const std::shared_ptr<Camera>& camera() const { return m_camera; }
    std::shared_ptr<Camera>& camera() { return m_camera; }

    virtual void pitch(float angle) = 0;
    virtual void yaw(float angle) = 0;
    virtual void tick(const Input_state& input_state) = 0;
};

class Trackball_Camera_control : public Camera_control {

public:
    Trackball_Camera_control(
        const std::shared_ptr<Camera>& camera
    ) : Camera_control(
        Camera_control_type::TRACKBALL,
        camera
    ) {}
    virtual ~Trackball_Camera_control() override = default;

    static std::shared_ptr<Trackball_Camera_control> create(
        const std::shared_ptr<Camera>& camera
    ) {
        return std::make_shared<Trackball_Camera_control>(camera);
    }

    void tick(const Input_state& input_state) override {

        if (input_state.mouse_button(Mouse_button::LEFT) != Key_action::RELEASE) {
            yaw(input_state.mouse_dx * m_rotate_speed);
            pitch(input_state.mouse_dy * m_rotate_speed);
        } else if (input_state.mouse_button(Mouse_button::MIDDLE) != Key_action::RELEASE) {
            camera()->node()->translate(camera()->node()->up(), input_state.mouse_dy * m_move_speed);
            camera()->node()->translate(camera()->node()->right(), input_state.mouse_dx * m_move_speed);
        }

        auto delta_scale = input_state.mouse_scroll_dy;
        if (std::abs(delta_scale) > EPSILON) {
            camera()->adjust_zoom(delta_scale * m_zoom_speed);
        }
    }

private:
    void pitch(float angle) override {
        auto mat = glm::rotate(glm::identity<glm::mat4>(), glm::radians(angle), camera()->node()->right());
        camera()->node()->rotate(angle, camera()->node()->right());
        camera()->node()->set_position(glm::vec3(mat * glm::vec4(camera()->node()->position(), 1.0f)));
    }

    void yaw(float angle) override {
        auto mat = glm::rotate(glm::identity<glm::mat4>(), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        camera()->node()->rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f));
        camera()->node()->set_position(glm::vec3(mat * glm::vec4(camera()->node()->position(), 1.0f)));
    }
};


class Game_Camera_control : public Camera_control {

public:
    Game_Camera_control(
        const std::shared_ptr<Camera>& camera
    ) : Camera_control(
        Camera_control_type::GAME,
        camera
    ) {}

    virtual ~Game_Camera_control() override = default;

    static std::shared_ptr<Game_Camera_control> create(
        const std::shared_ptr<Camera>& camera
    ) {
        return std::make_shared<Game_Camera_control>(camera);
    }

    void tick(const Input_state& input_state) override {

        if (input_state.mouse_button(Mouse_button::LEFT)!= Key_action::RELEASE) {
            yaw(input_state.mouse_dx * m_rotate_speed);
            pitch(input_state.mouse_dy * m_rotate_speed);
        }

        glm::vec3 direction = glm::zero<glm::vec3>();

        if (input_state.key(Key_code::W)!= Key_action::RELEASE) {
            direction += camera()->node()->front();
        }

        if (input_state.key(Key_code::S)!= Key_action::RELEASE) {
            direction += camera()->node()->back();
        }

        if (input_state.key(Key_code::A)!= Key_action::RELEASE) {
            direction += camera()->node()->left();
        }

        if (input_state.key(Key_code::D)!= Key_action::RELEASE) {
            direction += camera()->node()->right();
        }

        if (input_state.key(Key_code::Q)!= Key_action::RELEASE) {
            direction += camera()->node()->up();
        }

        if (input_state.key(Key_code::E)!= Key_action::RELEASE) {
            direction += camera()->node()->down();
        }

        if (std::abs(glm::length(direction)) > EPSILON) {
            camera()->node()->translate(glm::normalize(direction), m_move_speed);
        }

        auto delta_scale = input_state.mouse_scroll_dy;

        if (std::abs(delta_scale) > DBL_EPSILON) {
            camera()->adjust_zoom(delta_scale * m_zoom_speed);
        }
    }
    

private:
    void pitch(float angle) override {
        camera()->node()->rotate(angle, camera()->node()->right());
    }

    void yaw(float angle) override {
        camera()->node()->rotate(angle, camera()->node()->up());
    }
};



};
