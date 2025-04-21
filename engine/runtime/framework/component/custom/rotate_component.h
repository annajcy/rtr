#pragma once
#include "../component_base.h"
#include "../node/node_component.h"


namespace rtr {

class Rotate_component : public Component_base {
protected:
    glm::vec3 m_axis{0.0f, 1.0f, 0.0f};
    float m_speed{1.0f};
    std::shared_ptr<Node_component> m_node{};
public:
    Rotate_component() : Component_base(Component_type::CUSTOM) {}
    ~Rotate_component() = default;

    void on_add_to_game_object() override {
        set_node(component_list()->get_component<Node_component>());
    }

    glm::vec3& axis() { return m_axis; }
    float& speed() { return m_speed; }
    glm::vec3 axis() const { return m_axis; }
    float speed() const { return m_speed; }
    void set_node(const std::shared_ptr<Node_component>& node) { 
        m_node = node; 
        set_priority(node->priority() - 1);
    }
    std::shared_ptr<Node_component> node() const { return m_node; }

    void tick(const Logic_tick_context& tick_context) override {
        m_node->rotate(m_speed * tick_context.delta_time, m_axis);
    }

    static std::shared_ptr<Rotate_component> create() {
        return std::make_shared<Rotate_component>();
    }
};



};


