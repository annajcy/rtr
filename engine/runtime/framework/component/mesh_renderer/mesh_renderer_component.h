#pragma once

#include "engine/runtime/context/swap/renderable_object.h"
#include "engine/runtime/context/tick_context/logic_tick_context.h"
#include "engine/runtime/framework/component/component.h"
#include "engine/runtime/framework/component/mesh_renderer/mesh_renderer.h"
#include "engine/runtime/framework/component/node/node_component.h"
#include <memory>

namespace rtr {
class Mesh_renderer_component : public Base_component {

protected:
    std::shared_ptr<Mesh_renderer> m_mesh_renderer{};
    bool m_is_cast_shadow{true};

public:

    Mesh_renderer_component() : Base_component(Component_type::MESH_RENDERER) {}

    virtual ~Mesh_renderer_component() = default;

    void on_add_to_game_object() override {
        auto node = component_list()->get_component<Node_component>()->node();
        m_mesh_renderer = Mesh_renderer::create(node);
    }
    
    static std::shared_ptr<Mesh_renderer_component> create() {
        return std::make_shared<Mesh_renderer_component>();
    }

    const std::shared_ptr<Mesh_renderer>& mesh_renderer() const { return m_mesh_renderer; }
    std::shared_ptr<Mesh_renderer>& mesh_renderer() { return m_mesh_renderer; }

    bool is_cast_shadow() const { return m_is_cast_shadow; }
    bool& is_cast_shadow() { return m_is_cast_shadow; }

    void tick(const Logic_tick_context& tick_context) override {
        auto& data = tick_context.logic_swap_data;
        data.render_objects.push_back(Swap_renderable_object{
            .material = m_mesh_renderer->material(),
            .geometry = m_mesh_renderer->geometry(),
            .model_matrix = m_mesh_renderer->node()->model_matrix(),
            .is_cast_shadow = m_is_cast_shadow
        });
    }
    
};
}