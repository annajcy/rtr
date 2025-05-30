#pragma once

#include "engine/runtime/framework/component/camera/camera_component.h"
#include "engine/runtime/framework/component/camera/camera_control.h"
#include "engine/runtime/framework/component/component.h"
#include <memory>

namespace rtr {

class Camera_control_component : public Base_component {

protected:
    std::shared_ptr<Camera_control> m_camera_control{};

public:
    Camera_control_component() : Base_component(Component_type::CAMERA_CONTROL) {}
    virtual ~Camera_control_component() = default;

    void tick(const Logic_tick_context& tick_context) override {
        auto &input_state = tick_context.input_state;
        m_camera_control->tick(input_state);
    }

    const std::shared_ptr<Camera_control>& camera_control() const { return m_camera_control; }
    std::shared_ptr<Camera_control>& camera_control() { return m_camera_control; }
};

class Trackball_camera_control_component : public Camera_control_component {

public:
    Trackball_camera_control_component() {}
    ~Trackball_camera_control_component() override = default;

    static std::shared_ptr<Trackball_camera_control_component> create() {
        return std::make_shared<Trackball_camera_control_component>();
    }

    void on_add_to_game_object() override {
        auto camera = get_component<Camera_component>()->camera();
        m_camera_control = Trackball_Camera_control::create(camera); 
    }

    std::shared_ptr<Trackball_Camera_control> trackball_camera_control() {
        return std::dynamic_pointer_cast<Trackball_Camera_control>(m_camera_control);
    }
};


class Game_camera_control_component : public Camera_control_component {

public:
    Game_camera_control_component(){}
    ~Game_camera_control_component() override = default;

    static std::shared_ptr<Game_camera_control_component> create() {
        return std::make_shared<Game_camera_control_component>();
    }

    void on_add_to_game_object() override {
        auto camera = get_component<Camera_component>()->camera();
        m_camera_control = Game_Camera_control::create(camera);
    }

    std::shared_ptr<Game_Camera_control> game_camera_control() {
        return std::dynamic_pointer_cast<Game_Camera_control>(m_camera_control); 
    }

};



};
