
#include "engine/global/base.h"
#include "engine/runtime/geometry.h"
#include "engine/runtime/node.h"
#include <memory>

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

class Ambient_light : public Light {
public:
    Ambient_light() :
    Light(Light_type::AMBIENT) {}
    ~Ambient_light() = default;
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

class Light_setting
{
private:
    std::shared_ptr<Light> m_main_light{};
    std::vector<std::shared_ptr<Directional_light>> m_directional_lights{}; // 定向光源
    std::vector<std::shared_ptr<Ambient_light>> m_ambient_lights{};         // 环境光源
    std::vector<std::shared_ptr<Spot_light>> m_spot_lights{};               // 聚光灯
    std::vector<std::shared_ptr<Point_light>> m_point_lights{};             // 点光源

public:
    Light_setting() = default;
    Light_setting(const std::vector<std::shared_ptr<Light>>& lights) {
        for (auto& light : lights) {
            add(light);
        }
    }

    // 获取主光源
    std::shared_ptr<Light>& main_light() {
        return m_main_light;
    }

    // 获取光源列表
    const std::vector<std::shared_ptr<Directional_light>>& directional_lights() {
        return m_directional_lights;
    }

    const std::vector<std::shared_ptr<Ambient_light>> ambient_lights() {
        return m_ambient_lights;
    }

    const std::vector<std::shared_ptr<Spot_light>> spot_lights() {
        return m_spot_lights;
    }

    const std::vector<std::shared_ptr<Point_light>> point_lights() {
        return m_point_lights;
    }

    // 添加光源
    void add(const std::shared_ptr<Light>& light) {
        switch (light->type()) {
            case Light_type::DIRECTIONAL:
                m_directional_lights.push_back(std::dynamic_pointer_cast<Directional_light>(light));
                break;
            case Light_type::AMBIENT:
                m_ambient_lights.push_back(std::dynamic_pointer_cast<Ambient_light>(light));
                break;
            case Light_type::SPOT:
                m_spot_lights.push_back(std::dynamic_pointer_cast<Spot_light>(light));
                break;
            case Light_type::POINT:
                m_point_lights.push_back(std::dynamic_pointer_cast<Point_light>(light));
                break;
            default:
                break;
        }
    }

    // 移除光源
    void erase(const std::shared_ptr<Light>& light) {
        switch (light->type()) {
            case Light_type::DIRECTIONAL:
                m_directional_lights.erase(std::remove(m_directional_lights.begin(), m_directional_lights.end(), light), m_directional_lights.end());
                break;
            case Light_type::AMBIENT:
                m_ambient_lights.erase(std::remove(m_ambient_lights.begin(), m_ambient_lights.end(), light), m_ambient_lights.end());
                break;
            case Light_type::SPOT:
                m_spot_lights.erase(std::remove(m_spot_lights.begin(), m_spot_lights.end(), light), m_spot_lights.end());
                break;
            case Light_type::POINT:
                m_point_lights.erase(std::remove(m_point_lights.begin(), m_point_lights.end(), light), m_point_lights.end());
                break;
            default:
                break;
        }
    }

    // 清空光源
    void clear() {
        m_directional_lights.clear();
        m_ambient_lights.clear();
        m_spot_lights.clear();
        m_point_lights.clear();
        m_main_light = nullptr;
    }

    ~Light_setting() = default;
};

};