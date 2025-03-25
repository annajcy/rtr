#pragma once
#include "engine/global/base.h"
#include "engine/runtime/enum.h"

#include "geometry.h"
#include "node.h"
#include "shader.h"


namespace rtr {


class Light : public Node {
protected:
    bool m_is_main{false};
    glm::vec3 m_color{};
    float m_intensity{};
    Light_type m_type{};
public:
    Light(Light_type type) : Node(Node_type::LIGHT) , m_type(type) {}
    virtual ~Light() = default;
    
    Light_type type() const { return m_type; }
    glm::vec3& color() { return m_color; }
    [[nodiscard]] float intensity() const { return m_intensity; }
    float& intensity() { return m_intensity; }
    [[nodiscard]] bool is_main() const { return m_is_main; }
    bool& is_main() { return m_is_main; }
    
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

class Light_setting  {
protected:
    std::vector<std::shared_ptr<Light>> m_lights{};
    int m_main_light_index{-1};

public:
    Light_setting() {}
    ~Light_setting() = default;
    void add_light(const std::shared_ptr<Light>& light) {
        m_lights.push_back(light);
        if (light->is_main()) {
            m_main_light_index = m_lights.size() - 1;
        }
    }

    void remove_light(unsigned int id) {
        m_lights.erase(m_lights.begin() + id);
        if (m_main_light_index == id) {
            m_main_light_index = -1;
        } else if (m_main_light_index > id) {
            m_main_light_index--;
        }
    }

    const std::shared_ptr<Light>& main_light() const { 
        return m_lights[m_main_light_index];
    }

    void set_main_light(std::shared_ptr<Light>& light) {
        m_lights[m_main_light_index]->is_main() = false;
        for (unsigned int i = 0; i < m_lights.size(); i++) {
            if (m_lights[i] == light) {
                m_main_light_index = i;
                m_lights[i]->is_main() = true;
                break;
            }
        }
    }

    void set_main_light(unsigned int id) { 
        m_lights[m_main_light_index]->is_main() = false;
        m_main_light_index = id;
        m_lights[id]->is_main() = true;
    }

    void clear_lights() {
        m_lights.clear();
        m_main_light_index = -1;
    }

};

};