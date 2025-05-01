#pragma once

#include "../component_base.h"
#include "engine/runtime/framework/object/node/node.h"
#include <memory>

namespace rtr {

class Node_component : public std::enable_shared_from_this<Node_component>, public Component_base {

protected:
    std::shared_ptr<Node> m_node{};

public:
    Node_component() : Component_base(Component_type::NODE) {}

  	~Node_component() {}

	void tick(const Logic_tick_context& tick_context) override {}

    const std::shared_ptr<Node>& node() const { return m_node; }
    std::shared_ptr<Node> node() { return m_node; }

    void on_add_to_game_object() override { 
        m_node = Node::create();
    }
};

};


