#pragma once

#include "engine/runtime/context/swap_struct.h"
#include "engine/runtime/function/render/core/geometry.h"
#include "engine/runtime/function/render/core/material.h"
#include "engine/runtime/function/render/core/texture.h"
#include "engine/runtime/framework/component/component_base.h"
#include "engine/runtime/framework/component/node/node_component.h"
#include "engine/runtime/global/enum.h"
#include <memory>
#include <unordered_map>

namespace rtr {
class Mesh_renderer_component : public Component_base {

protected:
    std::shared_ptr<Node_component> m_node{};
    std::shared_ptr<Geometry> m_geometry{};
    std::shared_ptr<Material> m_material{};

public:
    Mesh_renderer_component() : Component_base(Component_type::MESH_RENDERER) {}

    Mesh_renderer_component(
        const std::shared_ptr<Geometry>& geometry,
        const std::shared_ptr<Material>& material
    ) : Component_base(Component_type::MESH_RENDERER),
        m_geometry(geometry),
        m_material(material) {}

    virtual ~Mesh_renderer_component() = default;

    void on_add_to_game_object() override {
        set_node(component_list()->get_component<Node_component>());
    }
    
    static std::shared_ptr<Mesh_renderer_component> create(
        const std::shared_ptr<Geometry>& geometry,
        const std::shared_ptr<Material>& material
    ) {
        return std::make_shared<Mesh_renderer_component>(
            geometry, 
            material
        );
    }

    static std::shared_ptr<Mesh_renderer_component> create(
    ) {
        return std::make_shared<Mesh_renderer_component>();
    }

    void set_node(const std::shared_ptr<Node_component>& node) { 
        m_node = node; 
        set_priority(m_node->priority() + 1);
    }

    const std::shared_ptr<Node_component>& node() const {
        return m_node;
    }

    void set_geometry(const std::shared_ptr<Geometry>& geometry) { m_geometry = geometry; }
    void set_material(const std::shared_ptr<Material>& material) { m_material = material; }
    const std::shared_ptr<Geometry> geometry() const { return m_geometry; }
    const std::shared_ptr<Material> material() const { return m_material; }

    void tick(const Logic_tick_context& tick_context) override {
        auto& data = tick_context.logic_swap_data;
        data.render_objects.push_back(Swap_object{
            .material = m_material,
            .geometry = m_geometry,
            .model_matrix = node()->model_matrix()
        });
    }
    
};
}