#pragma once

#include "engine/runtime/framework/component/component_base.h"
#include "engine/runtime/framework/component/node/node.h"
#include "engine/runtime/framework/component/node/node_component.h"
#include <cmath>

namespace rtr {
class Ping_pong_component : public Component_base {
protected:
    glm::vec3 m_position{};
    float m_speed{1.0f};
    float m_amplitude{1.0f};
    std::shared_ptr<Node> m_node{};
public:
    Ping_pong_component() : Component_base(Component_type::CUSTOM) {}
    ~Ping_pong_component() = default;
    void on_add_to_game_object() override {
        m_node = get_component<Node_component>()->node();
    }
 
    float& speed() { return m_speed; }
    const float& speed() const { return m_speed; }
    const std::shared_ptr<Node>& node() const { return m_node; }
    std::shared_ptr<Node>& node() { return m_node; }

    const glm::vec3& position() const {
        return m_position;
    }

    glm::vec3& position() {
        return m_position;
    }

    const float& amplitude() const {
        return m_amplitude;
    }

    float& amplitude() {
        return m_amplitude;
    }
    
    void tick(const Logic_tick_context& tick_context) override {
        static double t = 0; 
        t += m_speed * tick_context.delta_time;
        auto position = m_position + glm::vec3(0.0f,  m_amplitude * sin(t), 0.0f);
        m_node->set_position(position);
    }

};
}