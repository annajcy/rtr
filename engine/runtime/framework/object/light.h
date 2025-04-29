#pragma once

#include "engine/runtime/framework/object/node.h"
#include "engine/runtime/global/enum.h"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"
#include <memory>

namespace rtr {

class Light {

protected:
    glm::vec3 m_color{1.0f};
    float m_intensity{1.0f};
    Light_type m_light_type{};
    std::shared_ptr<Node> m_node{};

public:
    Light(
        Light_type light_type,
        const std::shared_ptr<Node>& node
    ) : m_light_type(light_type),
        m_node(node) {}

    virtual ~Light() = default;

    Light_type light_type() const { return m_light_type; }
    glm::vec3& color() { return m_color; }
    float& intensity() { return m_intensity; }

    const glm::vec3& color() const { return m_color; }
    const float& intensity() const { return m_intensity; }
    const std::shared_ptr<Node>& node() const { return m_node; }
    std::shared_ptr<Node>& node() { return m_node; }

};

class Directional_light : public Light {

public:
    Directional_light(
        const std::shared_ptr<Node>& node
    ) : Light(Light_type::DIRECTIONAL, node) { }
    ~Directional_light() = default;

    static std::shared_ptr<Directional_light> create(
        const std::shared_ptr<Node>& node
    ) {
        return std::make_shared<Directional_light>(node);
    }

};

class Point_light : public Light {
protected:
    float m_k1{1.0f};
    float m_k2{1.0f};
    float m_kc{1.0f};

public:
    Point_light(
        const std::shared_ptr<Node>& node
    ) : Light(Light_type::POINT, node) {}
    ~Point_light() = default;

    static std::shared_ptr<Point_light> create(
        const std::shared_ptr<Node>& node
    ) {
        return std::make_shared<Point_light>(node);
    }

    float& k1() { return m_k1; }
    float& k2() { return m_k2; }
    float& kc() { return m_kc; }

    const float& k1() const { return m_k1; }
    const float& k2() const { return m_k2; }
    const float& kc() const { return m_kc; }
};

class Spot_light : public Light {
protected:
    float m_inner_angle{10.0f};
    float m_outer_angle{20.0f};

public:
    Spot_light(
        const std::shared_ptr<Node>& node
    ) : Light(Light_type::SPOT, node) {}
    ~Spot_light() = default;

    float& inner_angle() { return m_inner_angle; }
    float& outer_angle() { return m_outer_angle; }
    const float& inner_angle() const { return m_inner_angle; }
    const float& outer_angle() const { return m_outer_angle; }
    float inner_angle_cos() const { return glm::cos(glm::radians(m_inner_angle)); }
    float outer_angle_cos() const { return glm::cos(glm::radians(m_outer_angle)); }

    static std::shared_ptr<Spot_light> create(
        const std::shared_ptr<Node>& node
    ) {
        return std::make_shared<Spot_light>(node);
    }

};

}