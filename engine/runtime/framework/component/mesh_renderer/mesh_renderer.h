#pragma once

#include "engine/runtime/framework/component/node/node.h"
#include "engine/runtime/function/render/object/geometry.h"
#include "engine/runtime/function/render/object/material.h"

namespace rtr {

class Mesh_renderer {

protected:
    std::shared_ptr<Node> m_node{};
    std::shared_ptr<Geometry> m_geometry{};
    std::shared_ptr<Material> m_material{};

public:
    Mesh_renderer() {}
    Mesh_renderer(
        const std::shared_ptr<Node>& node
    ) : m_node(node) {}

    virtual ~Mesh_renderer() = default;
    
    static std::shared_ptr<Mesh_renderer> create(
        const std::shared_ptr<Node>& node
    ) {
        return std::make_shared<Mesh_renderer>(node);
    }

    const std::shared_ptr<Node>& node() const {return m_node; }
    std::shared_ptr<Node>& node() { return m_node; }
    const std::shared_ptr<Geometry>& geometry() const { return m_geometry; }
    std::shared_ptr<Geometry>& geometry() { return m_geometry; }
    const std::shared_ptr<Material>& material() const { return m_material; }
    std::shared_ptr<Material>& material() { return m_material; }
    
};
}