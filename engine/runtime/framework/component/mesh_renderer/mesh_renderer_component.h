#pragma once

#include "engine/runtime/core/geometry.h"
#include "engine/runtime/core/material.h"
#include "engine/runtime/framework/component/component_base.h"
#include "engine/runtime/framework/component/node/node_component.h"
#include <memory>

namespace rtr {
class Mesh_renderer_component : public Component_base {

protected:
    std::weak_ptr<Node_component> m_node{};
    std::shared_ptr<Geometry> m_geometry{};
    std::shared_ptr<Material> m_material{};

public:
    Mesh_renderer_component(
        const std::shared_ptr<Geometry>& geometry,
        const std::shared_ptr<Material>& material
    ) : Component_base(Component_type::MESH_RENDERER),
        m_geometry(geometry),
        m_material(material) {}
    virtual ~Mesh_renderer_component() = default;
    void set_node(const std::shared_ptr<Node_component>& node) { m_node = node; }

    static std::shared_ptr<Mesh_renderer_component> create(
        const std::shared_ptr<Geometry>& geometry,
        const std::shared_ptr<Material>& material
    ) {
        return std::make_shared<Mesh_renderer_component>(
            geometry, 
            material
        );
    }

    std::shared_ptr<Node_component> node() const {
        if (m_node.expired()) {
            return nullptr;
        }
        return m_node.lock();
    }

    void set_geometry(const std::shared_ptr<Geometry>& geometry) { m_geometry = geometry; }
    void set_material(const std::shared_ptr<Material>& material) { m_material = material; }
    const std::shared_ptr<Geometry> geometry() const { return m_geometry; }
    const std::shared_ptr<Material> material() const { return m_material; }

    void tick(const Logic_tick_context& tick_context) override {
        auto shader_program = m_material->get_shader_program();
        auto& data = tick_context.logic_swap_data;
        data.render_objects.push_back({});
    }
    
};
}