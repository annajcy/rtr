#pragma once

#include "engine/runtime/global/base.h"
#include "engine/runtime/function/input/input_system.h"
#include "engine/runtime/function/context/global_context.h"
#include "../component_base.h"

#include "camera_component.h"

namespace rtr {

class Camera_control_component : public Component_base {

protected:
    std::shared_ptr<Camera_component> m_camera{};
    
    float m_move_speed{0.001f};
    float m_rotate_speed{0.1f};
    float m_zoom_speed{0.01f};

public:

    Camera_control_component(
        const std::shared_ptr<Camera_component>& camera
    ) : Component_base(Component_type::CAMERA_CONTROL),
        m_camera(camera) {}

    virtual ~Camera_control_component() = default;

    float& move_speed() { return m_move_speed; }
    float& rotate_speed() { return m_rotate_speed; }
    float& zoom_speed() { return m_zoom_speed; }
    float move_speed() const { return m_move_speed; }
    float rotate_speed() const { return m_rotate_speed; }
    float zoom_speed() const { return m_zoom_speed; }

    void set_camera(const std::shared_ptr<Camera_component>& camera) { m_camera = camera; }
    const std::shared_ptr<Camera_component>& camera() const { return m_camera; }

    virtual void pitch(float angle) = 0;
    virtual void yaw(float angle) = 0;

    static Component_type c_type() { return Component_type::CAMERA_CONTROL; }
    
};

class Trackball_camera_control_component : public Camera_control_component {

public:
    Trackball_camera_control_component(
        const std::shared_ptr<Camera_component>& camera, 
        const std::shared_ptr<Input_system>& input
    ) : Camera_control_component(camera) {}

    virtual ~Trackball_camera_control_component() override = default;

    static std::shared_ptr<Trackball_camera_control_component> create(
        const std::shared_ptr<Camera_component>& camera, 
        const std::shared_ptr<Input_system>& input
    ) {
        return std::make_shared<Trackball_camera_control_component>(camera, input);
    }

    void tick(float delta_time) override {
        auto input = Context::get_instance()->input_system;
        if (input->mouse_button(Mouse_button::LEFT) != Key_action::RELEASE) {
            yaw(-input->mouse_dx() * m_rotate_speed);
            pitch(input->mouse_dy() * m_rotate_speed);
        } else if (input->mouse_button(Mouse_button::MIDDLE) != Key_action::RELEASE) {
            
            m_camera->node()->translate(m_camera->node()->up(), input->mouse_dy() * m_move_speed);
            m_camera->node()->translate(m_camera->node()->right(), input->mouse_dx() * m_move_speed);
        }

        auto delta_scale = input->mouse_scroll_dy();
        if (std::abs(delta_scale) > EPSILON) {
            m_camera->adjust_zoom(delta_scale * m_zoom_speed);
        }
        
    }

    
    static Component_type c_type() { return Component_type::CAMERA_CONTROL; }

private:
    void pitch(float angle) override {
        auto mat = glm::rotate(glm::identity<glm::mat4>(), glm::radians(angle), m_camera->node()->right());
        m_camera->node()->rotate(angle, m_camera->node()->right());
        m_camera->node()->position() = glm::vec3(mat * glm::vec4(m_camera->node()->position(), 1.0f));
    }

    void yaw(float angle) override {
        auto mat = glm::rotate(glm::identity<glm::mat4>(), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        m_camera->node()->rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f));
        m_camera->node()->position() = glm::vec3(mat * glm::vec4(m_camera->node()->position(), 1.0f));
    }
};


class Game_camera_control_component : public Camera_control_component {

public:
    Game_camera_control_component(
        const std::shared_ptr<Camera_component>& camera
    ) : Camera_control_component(camera) {}
    virtual ~Game_camera_control_component() override = default;

    static std::shared_ptr<Game_camera_control_component> create(
        const std::shared_ptr<Camera_component>& camera, 
        const std::shared_ptr<Input_system>& input
    ) {
        return std::make_shared<Game_camera_control_component>(camera, input);
    }

    void tick(float delta_time) override {
        
        auto input = Context::get_instance()->input_system;

        if (input->mouse_button(Mouse_button::LEFT)!= Key_action::RELEASE) {
            yaw(input->mouse_dx() * m_rotate_speed);
            pitch(input->mouse_dy() * m_rotate_speed);
        }

        glm::vec3 direction = glm::zero<glm::vec3>();

        if (input->key(Key_code::W)!= Key_action::RELEASE) {
            direction += m_camera->node()->front();
        }

        if (input->key(Key_code::S)!= Key_action::RELEASE) {
            direction += m_camera->node()->back();
        }

        if (input->key(Key_code::A)!= Key_action::RELEASE) {
            direction += m_camera->node()->left();
        }

        if (input->key(Key_code::D)!= Key_action::RELEASE) {
            direction += m_camera->node()->right();
        }

        if (input->key(Key_code::Q)!= Key_action::RELEASE) {
            direction += m_camera->node()->up();
        }

        if (input->key(Key_code::E)!= Key_action::RELEASE) {
            direction += m_camera->node()->down();
        }

        if (std::abs(glm::length(direction)) > EPSILON) {
            m_camera->node()->translate(glm::normalize(direction), m_move_speed);
        }

        auto delta_scale = input->mouse_scroll_dy();

        if (std::abs(delta_scale) > DBL_EPSILON) {
            m_camera->adjust_zoom(delta_scale * m_zoom_speed);
        }
    }
    
    static Component_type c_type() { return Component_type::CAMERA_CONTROL; }

private:
    void pitch(float angle) override {
        m_camera->node()->rotate(angle, m_camera->node()->right());
    }

    void yaw(float angle) override {
        m_camera->node()->rotate(angle, m_camera->node()->up());
    }
};



};
