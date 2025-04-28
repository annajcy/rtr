#pragma once

#include "engine/runtime/context/swap_struct.h"
#include "engine/runtime/framework/object/mesh_renderer.h"
#include "engine/runtime/framework/component/component_base.h"
#include "engine/runtime/framework/component/node/node_component.h"
#include <memory>

namespace rtr {
class Mesh_renderer_component : public Component_base {

protected:
    std::shared_ptr<Mesh_renderer> m_mesh_renderer{};

public:

    Mesh_renderer_component(
    ) : Component_base(Component_type::MESH_RENDERER) {}

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

    
    void tick(const Logic_tick_context& tick_context) override {
        auto& data = tick_context.logic_swap_data;
        data.render_objects.push_back(Swap_object{
            .material = m_mesh_renderer->material(),
            .geometry = m_mesh_renderer->geometry(),
            .model_matrix = m_mesh_renderer->node()->model_matrix(),
            .shadow_setting = Swap_shadow_setting{
                .is_cast_shadow = m_mesh_renderer->shadow_setting().is_cast_shadow,
                .is_receive_shadow = m_mesh_renderer->shadow_setting().is_receive_shadow,
                .bias = m_mesh_renderer->shadow_setting().bias
            }
        });
    }
    
};
}