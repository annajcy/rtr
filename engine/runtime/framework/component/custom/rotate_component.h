#pragma once
#include "../component_base.h"
#include "../node/node_component.h"
#include "engine/runtime/framework/object/node/node.h"


namespace rtr {

class Rotate_component : public Component_base {
protected:
    glm::vec3 m_axis{0.0f, 1.0f, 0.0f};
    float m_speed{1.0f};
    std::shared_ptr<Node> m_node{};

public:
    Rotate_component() : Component_base(Component_type::CUSTOM) {}
    ~Rotate_component() = default;

    void on_add_to_game_object() override {
        m_node = get_component<Node_component>()->node();
    }

    glm::vec3& axis() { return m_axis; }
    float& speed() { return m_speed; }
    const glm::vec3& axis() const { return m_axis; }
    const float& speed() const { return m_speed; }
    
    const std::shared_ptr<Node>& node() const { return m_node; }
    std::shared_ptr<Node>& node() { return m_node; }

    void tick(const Logic_tick_context& tick_context) override {
        m_node->rotate(m_speed * tick_context.delta_time, m_axis);
    }

    static std::shared_ptr<Rotate_component> create() {
        return std::make_shared<Rotate_component>();
    }
};



};


