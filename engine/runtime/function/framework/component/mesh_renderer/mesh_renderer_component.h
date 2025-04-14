#pragma once
#include "engine/runtime/function/framework/component/component_base.h"
#include "engine/runtime/function/framework/component/node/node_component.h"
#include "engine/runtime/global/base.h"

namespace rtr {
class Mesh_renderer_component : public Component_base {

protected:
    std::weak_ptr<Node_component> m_node{};
public:
    Mesh_renderer_component() : Component_base(Component_type::MESH_RENDERER) {}
    virtual ~Mesh_renderer_component() = default;
    void set_node(const std::shared_ptr<Node_component>& node) { m_node = node; }
    std::shared_ptr<Node_component> node() const {
        if (m_node.expired()) {
            return nullptr;
        }
        return m_node.lock();
    }
    void tick(float delta_time) override {
        // TODO: implement
        std::cout << "Mesh renderer component tick" << std::endl;
    }
    
};
}