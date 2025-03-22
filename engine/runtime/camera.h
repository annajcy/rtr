#pragma once
#include "engine/global/base.h"
#include "engine/runtime/node.h"
#include "engine/runtime/shader.h"
#include "engine/runtime/input.h"
#include "engine/runtime/rhi/window/rhi_window.h"
#include <memory>
#include <unordered_map>
#include <vector>

namespace rtr {

class Camera : public Node {
protected:
    bool m_is_main = false;
    float m_near_bound{};
    float m_far_bound{};

public:

    Camera(float near_bound, float far_bound) : 
    Node(Node_type::CAMERA), 
    m_near_bound(near_bound), 
    m_far_bound(far_bound) {}

    bool is_main() const {
        return m_is_main;
    }

    bool& is_main() {
        return m_is_main;
    }

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


class Camera_setting : public ISet_shader_uniform {
protected:
    std::unordered_map<unsigned int, std::shared_ptr<Camera>> m_camera_map{};
    unsigned int m_main_camera_id{0};

    void update_main_camera(unsigned int new_id) {
        if (auto old_cam = m_camera_map.find(m_main_camera_id); 
            old_cam != m_camera_map.end()) {
            old_cam->second->is_main() = false;
        }
        if (auto new_cam = m_camera_map.find(new_id);
            new_cam != m_camera_map.end()) {
            new_cam->second->is_main() = true;
            m_main_camera_id = new_id;
        }
    }

public:
    Camera_setting() : ISet_shader_uniform() {}
    ~Camera_setting() = default;

    void add_camera(const std::shared_ptr<Camera>& camera) {
        if (camera->is_main()) {
            update_main_camera(camera->id());
        }
        m_camera_map.try_emplace(camera->id(), camera);
    }

    void remove_camera(unsigned int id) {
        if (auto it = m_camera_map.find(id); it != m_camera_map.end()) {
            if (id == m_main_camera_id) {
                m_main_camera_id = 0; // 需要手动设置新的主相机
            }
            m_camera_map.erase(it);
        }
    }

    void clear_cameras() {
        m_camera_map.clear();
        m_main_camera_id = 0;
    }

    std::shared_ptr<Camera> main_camera() const { 
        return m_camera_map.count(m_main_camera_id) 
            ? m_camera_map.at(m_main_camera_id) 
            : nullptr;
    }

    void set_main_camera(std::shared_ptr<Camera>& camera) {
        if (m_camera_map.contains(camera->id())) {
            update_main_camera(camera->id());
        }
    }

    void set_main_camera(unsigned int id) {
        if (m_camera_map.contains(id)) {
            update_main_camera(id);
        } else {
            std::cout << "Camera ID:" << id << " not exist\n";
        }
    }

    [[nodiscard]] unsigned int main_camera_id() const { 
        return m_main_camera_id; 
    }
    
    [[nodiscard]] std::shared_ptr<Camera> camera(unsigned int id) const { 
        return m_camera_map.count(id) ? m_camera_map.at(id) : nullptr;
    }
    
    const std::unordered_map<unsigned int, std::shared_ptr<Camera>>& camera_map() const { 
        return m_camera_map; 
    }
};

}