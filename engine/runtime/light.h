
#include "engine/global/base.h"
#include "engine/runtime/geometry.h"
#include "engine/runtime/node.h"
#include "engine/runtime/shader.h"
#include <memory>
#include <unordered_map>

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
    bool m_is_main{false};
    glm::vec3 m_color{};
    float m_intensity{};
    Light_type m_type{};
public:
    Light(Light_type type) : Node(Node_type::LIGHT) , m_type(type) {}
    virtual ~Light() = default;
    
    Light_type type() const { return m_type; }
    glm::vec3& color() { return m_color; }
    float intensity() { return m_intensity; }

    bool& is_main() { return m_is_main; }
    [[nodiscard]] bool is_main() const { return m_is_main; }
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

class Light_setting : public ISet_shader_uniform {
protected:
    std::unordered_map<unsigned int, std::shared_ptr<Light>> m_light_map{};
    unsigned int m_main_light_id{0};

    void update_main_light(unsigned int new_id) {
        if (auto old_light = m_light_map.find(m_main_light_id); old_light != m_light_map.end()) {
            old_light->second->is_main() = false;
        }
        if (auto new_light = m_light_map.find(new_id); new_light != m_light_map.end()) {
            new_light->second->is_main() = true;
            m_main_light_id = new_id;
        }
    }

public:
    Light_setting() : ISet_shader_uniform() {}
    ~Light_setting() = default;
    void add_light(const std::shared_ptr<Light>& light) {
        if (light->is_main()) {
            update_main_light(light->id());
        }
        m_light_map.try_emplace(light->id(), light);
    }

    void remove_light(unsigned int id) {
        if (auto it = m_light_map.find(id); it != m_light_map.end()) {
            if (id == m_main_light_id) {
                m_main_light_id = 0; // 需要手动设置新的主光源
            }
            m_light_map.erase(it);
        }
    }

    std::shared_ptr<Light> main_light() const { 
        return m_light_map.count(m_main_light_id) ? m_light_map.at(m_main_light_id) : nullptr;
    }

    void set_main_light(std::shared_ptr<Light>& light) {
        if (m_light_map.contains(light->id())) {
            update_main_light(light->id());
        }
    }

    void set_main_light(unsigned int id) { 
        if (m_light_map.contains(id)) {
            update_main_light(id);
        } else {
            std::cout << "Light ID:" << id << " not exist\n";
        }
    }

    void clear_lights() {
        m_light_map.clear();
        m_main_light_id = 0;
    }

    [[nodiscard]] unsigned int main_light_id() const { return m_main_light_id; }
    [[nodiscard]] std::shared_ptr<Light> light(unsigned int id) const { return m_light_map.at(id); }
    std::unordered_map<unsigned int, std::shared_ptr<Light>>& light_map() { return m_light_map; }
};

};