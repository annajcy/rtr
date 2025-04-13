#pragma once

#include "engine/runtime/global/base.h"
#include "engine/runtime/function/input/input_system.h"
#include "../component_base.h"

#include "camera_component.h"
#include <memory>

namespace rtr {

enum class Camera_control_type {
    TRACKBALL,
    GAME, 
};

class Camera_control_component : public Component_base {

protected:
    std::weak_ptr<Camera_component> m_camera{};
    Camera_control_type m_camera_control_type{};
    Input_system_state m_input_system_state{};
    
    float m_move_speed{0.001f};
    float m_rotate_speed{0.1f};
    float m_zoom_speed{0.01f};

public:

    Camera_control_component(
        Camera_control_type camera_control_type,
        const std::shared_ptr<Camera_component>& camera
    ) : Component_base(Component_type::CAMERA_CONTROL),
        m_camera(camera) {}

    virtual ~Camera_control_component() = default;

    Camera_control_type camera_control_type() const { return m_camera_control_type; }

    float& move_speed() { return m_move_speed; }
    float& rotate_speed() { return m_rotate_speed; }
    float& zoom_speed() { return m_zoom_speed; }
    float move_speed() const { return m_move_speed; }
    float rotate_speed() const { return m_rotate_speed; }
    float zoom_speed() const { return m_zoom_speed; }

    void set_camera(const std::shared_ptr<Camera_component>& camera) { m_camera = camera; }
    const std::shared_ptr<Camera_component> camera() const { 
        if (m_camera.expired()) {
            return nullptr; 
        }
        return m_camera.lock();
    }

    void set_input_system_state(const Input_system_state& input_system_state) { m_input_system_state = input_system_state; }
    const Input_system_state& input_system_state() const { return m_input_system_state; }

    virtual void pitch(float angle) = 0;
    virtual void yaw(float angle) = 0;
    
};

class Trackball_camera_control_component : public Camera_control_component {

public:
    Trackball_camera_control_component(
        const std::shared_ptr<Camera_component>& camera
    ) : Camera_control_component(Camera_control_type::TRACKBALL, camera) {}

    virtual ~Trackball_camera_control_component() override = default;

    static std::shared_ptr<Trackball_camera_control_component> create(
        const std::shared_ptr<Camera_component>& camera
    ) {
        return std::make_shared<Trackball_camera_control_component>(camera);
    }

    void tick(float delta_time) override {

        if (m_input_system_state.mouse_button(Mouse_button::LEFT) != Key_action::RELEASE) {
            yaw(-m_input_system_state.mouse_dx * m_rotate_speed);
            pitch(m_input_system_state.mouse_dy * m_rotate_speed);
        } else if (m_input_system_state.mouse_button(Mouse_button::MIDDLE) != Key_action::RELEASE) {
            
            camera()->node()->translate(camera()->node()->up(), m_input_system_state.mouse_dy * m_move_speed);
            camera()->node()->translate(camera()->node()->right(), m_input_system_state.mouse_dx * m_move_speed);
        }

        auto delta_scale = m_input_system_state.mouse_scroll_dy;
        if (std::abs(delta_scale) > EPSILON) {
            camera()->adjust_zoom(delta_scale * m_zoom_speed);
        }
    }

private:
    void pitch(float angle) override {
        auto mat = glm::rotate(glm::identity<glm::mat4>(), glm::radians(angle), camera()->node()->right());
        camera()->node()->rotate(angle, camera()->node()->right());
        camera()->node()->position() = glm::vec3(mat * glm::vec4(camera()->node()->position(), 1.0f));
    }

    void yaw(float angle) override {
        auto mat = glm::rotate(glm::identity<glm::mat4>(), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        camera()->node()->rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f));
        camera()->node()->position() = glm::vec3(mat * glm::vec4(camera()->node()->position(), 1.0f));
    }
};


class Game_camera_control_component : public Camera_control_component {

public:
    Game_camera_control_component(
        const std::shared_ptr<Camera_component>& camera
    ) : Camera_control_component(Camera_control_type::GAME, camera) {}
    virtual ~Game_camera_control_component() override = default;

    static std::shared_ptr<Game_camera_control_component> create(
        const std::shared_ptr<Camera_component>& camera
    ) {
        return std::make_shared<Game_camera_control_component>(camera);
    }

    void tick(float delta_time) override {

        if (m_input_system_state.mouse_button(Mouse_button::LEFT)!= Key_action::RELEASE) {
            yaw(m_input_system_state.mouse_dx * m_rotate_speed);
            pitch(m_input_system_state.mouse_dy * m_rotate_speed);
        }

        glm::vec3 direction = glm::zero<glm::vec3>();

        if (m_input_system_state.key(Key_code::W)!= Key_action::RELEASE) {
            direction += camera()->node()->front();
        }

        if (m_input_system_state.key(Key_code::S)!= Key_action::RELEASE) {
            direction += camera()->node()->back();
        }

        if (m_input_system_state.key(Key_code::A)!= Key_action::RELEASE) {
            direction += camera()->node()->left();
        }

        if (m_input_system_state.key(Key_code::D)!= Key_action::RELEASE) {
            direction += camera()->node()->right();
        }

        if (m_input_system_state.key(Key_code::Q)!= Key_action::RELEASE) {
            direction += camera()->node()->up();
        }

        if (m_input_system_state.key(Key_code::E)!= Key_action::RELEASE) {
            direction += camera()->node()->down();
        }

        if (std::abs(glm::length(direction)) > EPSILON) {
            camera()->node()->translate(glm::normalize(direction), m_move_speed);
        }

        auto delta_scale = m_input_system_state.mouse_scroll_dy;

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
