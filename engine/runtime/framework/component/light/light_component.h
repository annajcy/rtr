#pragma once
#include "../node/node_component.h"
#include "engine/runtime/framework/component/component_base.h"
#include "engine/runtime/framework/object/light.h"

#include "glm/fwd.hpp"
#include <memory>

namespace rtr {

class Light_component : public Component_base {

protected:
    std::shared_ptr<Light> m_light{};

public:
    Light_component() : Component_base(Component_type::LIGHT) {}

    virtual ~Light_component() = default;
    const std::shared_ptr<Light>& light() const { return m_light; }
    std::shared_ptr<Light>& light() { return m_light; }

};

class Directional_light_component : public Light_component {

public:
    Directional_light_component() {}
    ~Directional_light_component() = default;

    static std::shared_ptr<Directional_light_component> create() {
        return std::make_shared<Directional_light_component>();
    }

    std::shared_ptr<Directional_light> directional_light() { 
        return std::dynamic_pointer_cast<Directional_light>(m_light); 
    }

    void tick(const Logic_tick_context& tick_context) override {
        auto& data = tick_context.logic_swap_data;
        Swap_directional_light dl{};
        dl.color = directional_light()->color();
        dl.direction = directional_light()->node()->world_front();
        dl.intensity = directional_light()->intensity();
        data.directional_lights.push_back(dl);
    }

    void on_add_to_game_object() override {
        auto node = get_component<Node_component>()->node();
        m_light = Directional_light::create(node);
    }
};

class Spot_light_component : public Light_component {

public:
    Spot_light_component() : Light_component() {}
    ~Spot_light_component() = default;
    
    static std::shared_ptr<Spot_light_component> create() {
        return std::make_shared<Spot_light_component>();
    }

    std::shared_ptr<Spot_light> spot_light() {
        return std::dynamic_pointer_cast<Spot_light>(m_light);
    }

    void tick(const Logic_tick_context& tick_context) override {
        auto& data = tick_context.logic_swap_data;
        Swap_spot_light spl{};
        spl.color = spot_light()->color();
        spl.direction = spot_light()->node()->world_front();
        spl.intensity = spot_light()->intensity();
        spl.position = spot_light()->node()->world_position();
        spl.inner_angle_cos = spot_light()->inner_angle_cos();
        spl.outer_angle_cos = spot_light()->outer_angle_cos();
        data.spot_lights.push_back(spl);
    }

    void on_add_to_game_object() override {
        auto node = get_component<Node_component>()->node();
        m_light = Spot_light::create(node);
    }

};

class Point_light_component : public Light_component {

public:
    Point_light_component() {}
    ~Point_light_component() = default;
   
    static std::shared_ptr<Point_light_component> create() {
        return std::make_shared<Point_light_component>();
    }

    std::shared_ptr<Point_light> point_light() {
        return std::dynamic_pointer_cast<Point_light>(m_light);
    }

    void tick(const Logic_tick_context& tick_context) override {
        auto& data = tick_context.logic_swap_data;
        Swap_point_light pl{};
        pl.attenuation = glm::vec3(point_light()->kc(), point_light()->k1(), point_light()->k2());
        pl.color = point_light()->color();
        pl.intensity = point_light()->intensity();
        pl.position = point_light()->node()->world_position();
        data.point_lights.push_back(pl);
    }

    void on_add_to_game_object() override {
        auto node = get_component<Node_component>()->node();
        m_light = Point_light::create(node);
    }

};

}