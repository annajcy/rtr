#pragma once
#include "engine/global/base.h"
#include "engine/runtime/enum.h"

#include "engine/runtime/core/geometry.h"
#include "engine/runtime/core/material.h"
#include "engine/runtime/core/node.h"


namespace rtr {

class Mesh : public Node {
private:
    
    std::shared_ptr<Geometry> m_geometry{};
    std::shared_ptr<Material> m_material{};

public:
    Mesh(
        const std::shared_ptr<Geometry>& geometry,
        const std::shared_ptr<Material>& material
    ) : Node(Node_type::MESH),
        m_geometry(geometry),
        m_material(material) {}
    virtual ~Mesh() override = default;

    std::shared_ptr<Geometry>& geometry() { return m_geometry; }
    std::shared_ptr<Material>& material() { return m_material; }


};

}