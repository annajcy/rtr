#pragma once
#include "engine/global/base.h"
#include "engine/runtime/geometry.h"
#include "engine/runtime/node.h"

namespace rtr {

enum class Light_type {
    POINT,
    DIRECTIONAL,
    SPOT,
    AREA,
    AMBIENT
};

class Light : public Node {
protected:
    glm::vec3 m_color{};
    float m_intensity{};
    Light_type m_type{};
public:
    Light(Light_type type) : Node(Node_type::LIGHT) , m_type(type) {}
    virtual ~Light() = default;
    
    Light_type type() const { return m_type; }
    glm::vec3& color() { return m_color; }
    float intensity() { return m_intensity; }
};

class Point_light : public Light {
protected:
    float m_k1{};
    float m_k2{};
    float m_kc{}; 

public:

    Point_light(Light_type type) :
    Light(type) {}

    Point_light() : 
    Light(Light_type::POINT) {}
    ~Point_light() = default;

    float& k1() { return m_k1; }
    float& k2() { return m_k2; }
    float& kc() { return m_kc; }

    [[nodiscard]] float k1() const { return m_k1; }
    [[nodiscard]] float k2() const { return m_k2; }
    [[nodiscard]] float kc() const { return m_kc; }

};

class Spot_light : public Point_light {
protected:
    float m_inner_angle{30.0f};
    float m_outer_angle{40.0f};

public:
    Spot_light() :
    Point_light(Light_type::SPOT) {}
    ~Spot_light() = default;
    float& inner_angle() { return m_inner_angle; }
    float& outer_angle() { return m_outer_angle; }
    [[nodiscard]] float inner_angle() const { return m_inner_angle; }
    [[nodiscard]] float outer_angle() const { return m_outer_angle; }

};

class Directional_light : public Light {

public:
    Directional_light() :
    Light(Light_type::DIRECTIONAL) {}
    ~Directional_light() = default;

};

class Ambient_Light : public Light {
public:
    Ambient_Light() :
    Light(Light_type::AMBIENT) {}
    ~Ambient_Light() = default;
};

class Area_light : public Light {
protected:
    std::shared_ptr<Geometry> m_geometry{};
public:
    Area_light(const std::shared_ptr<Geometry>& geometry) :
    Light(Light_type::AREA),
    m_geometry(geometry) {}
    
    ~Area_light() = default;
    std::shared_ptr<Geometry>& geometry() { return m_geometry; }
};

};