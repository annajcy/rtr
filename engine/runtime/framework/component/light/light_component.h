#pragma once
#include "../node/node_component.h"
#include "engine/runtime/function/render/render_struct.h"
#include "engine/runtime/function/render/render_system.h"

#include "engine/runtime/framework/component/component_base.h"
#include "engine/runtime/global/enum.h"

#include "glm/fwd.hpp"
#include <memory>

namespace rtr {

class Light_component : public Component_base {
protected:
    glm::vec3 m_color{1.0f};
    float m_intensity{1.0f};
    Light_type m_light_type{};
    std::weak_ptr<Node_component> m_node{};

public:
    Light_component(
        Light_type light_type
    ) : Component_base(Component_type::LIGHT), 
        m_light_type(light_type) {}

    virtual ~Light_component() = default;

    Light_type light_type() const { return m_light_type; }
    glm::vec3& color() { return m_color; }
    float& intensity() { return m_intensity; }

    glm::vec3 color() const { return m_color; }
    float intensity() const { return m_intensity; }

    void set_node(const std::shared_ptr<Node_component>& node) { 
        if (!node) {
            return;
        }

        if (m_node.lock() == node) {
            return; 
        }

        if (!m_node.expired()) {
            remove_dependency(m_node.lock());
        }

        add_dependency(node);
        m_node = node; 
    }

    std::shared_ptr<Node_component> node() const {
        if (m_node.expired()) {
            return nullptr;
        }
        return m_node.lock();
    }

};

class Directional_light_component : public Light_component {
public:
    Directional_light_component() : Light_component(Light_type::DIRECTIONAL) { }
    ~Directional_light_component() = default;

    glm::vec3 direction() const { return node()->world_rotation() * glm::vec3(0.0f, 0.0f, 1.0);  }

    static std::shared_ptr<Directional_light_component> create() {
        return std::make_shared<Directional_light_component>();
    }

    void tick(const Engine_tick_context& tick_context) override {
        std::cout << "tick directional light" << std::endl;
        auto& data = tick_context.global_context->render_system->logic_swap_data();

        Render_directional_light directional_light{};
        directional_light.color = color();
        directional_light.direction = direction();
        directional_light.intensity = intensity();

        data.directional_light = directional_light;
    }

};

class Spot_light_component : public Light_component {
protected:
    float m_inner_angle{30.0f};
    float m_outer_angle{40.0f};

public:
    Spot_light_component() : Light_component(Light_type::SPOT) {}
    ~Spot_light_component() = default;
    
    glm::vec3 direction() const { return node()->world_rotation() * glm::vec3(0.0f, 0.0f, 1.0); }
    glm::vec3 position() const { return node()->world_position(); }

    static std::shared_ptr<Spot_light_component> create() {
        return std::make_shared<Spot_light_component>();
    }

    float& inner_angle() { return m_inner_angle; }
    float& outer_angle() { return m_outer_angle; }

    float inner_angle() const { return m_inner_angle; }
    float outer_angle() const { return m_outer_angle; }

    float inner_angle_cos() const { return glm::cos(glm::radians(m_inner_angle)); }
    float outer_angle_cos() const { return glm::cos(glm::radians(m_outer_angle)); }

    void tick(const Engine_tick_context& tick_context) override {
        std::cout << "spot point light" << std::endl;
        auto& data = tick_context.global_context->render_system->logic_swap_data();
        Render_spot_light spot_light{};
        spot_light.color = color();
        spot_light.direction = direction();
        spot_light.intensity = intensity();
        spot_light.position = position();
        spot_light.inner_angle_cos = inner_angle_cos();
        spot_light.outer_angle_cos = outer_angle_cos();
        data.spot_lights.push_back(spot_light);
    }

};

class Point_light_component : public Light_component {
protected:
    float m_k1{1.0f};
    float m_k2{1.0f};
    float m_kc{1.0f};

public:
    Point_light_component() : Light_component(Light_type::POINT) {}
    ~Point_light_component() = default;
   
    glm::vec3 position() const { return node()->world_position(); }

    static std::shared_ptr<Point_light_component> create() {
        return std::make_shared<Point_light_component>();
    }

    float& k1() { return m_k1; }
    float& k2() { return m_k2; }
    float& kc() { return m_kc; }

    float k1() const { return m_k1; }
    float k2() const { return m_k2; }
    float kc() const { return m_kc; }

    void tick(const Engine_tick_context& tick_context) override {
        std::cout << "tick point light" << std::endl;
        auto& data = tick_context.global_context->render_system->logic_swap_data();
        Render_point_light point_light{};
        point_light.attenuation = glm::vec3(kc(), k1(), k2());
        point_light.color = color();
        point_light.intensity = intensity();
        point_light.position = position();
        data.point_lights.push_back(point_light);
    }

};

}