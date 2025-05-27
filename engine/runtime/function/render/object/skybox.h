#pragma once

#include "engine/runtime/function/render/object/geometry.h"
#include "engine/runtime/function/render/object/material.h"
#include "engine/runtime/function/render/object/shader.h"
#include "engine/runtime/function/render/object/texture.h"

namespace rtr {

class Skybox {
protected:
    std::shared_ptr<Geometry> m_skybox_geometry{};
    std::shared_ptr<Material> m_skybox_material{};

public:
    Skybox() {}
    Skybox(const std::shared_ptr<Texture>& texture) {
        m_skybox_geometry = Geometry::create_box();
        if (texture->texture_type() == Texture_type::TEXTURE_2D) {
            auto texture_2d = std::dynamic_pointer_cast<Texture_2D>(texture);
            auto mat = Skybox_spherical_material::create();
            mat->spherical_map = texture_2d;
            m_skybox_material = mat;
        } else {
            auto texture_cubemap = std::dynamic_pointer_cast<Texture_cubemap>(texture);
            auto mat = Skybox_cubemap_material::create();
            mat->cube_map = texture_cubemap;
            m_skybox_material = mat;
        }
    }

    std::shared_ptr<Geometry>& geometry() { return m_skybox_geometry; }
    std::shared_ptr<Material>& material() { return m_skybox_material; }

    const std::shared_ptr<Geometry>& geometry() const { return m_skybox_geometry; }
    const std::shared_ptr<Material>& material() const { return m_skybox_material; }

    static std::shared_ptr<Skybox> create(const std::shared_ptr<Texture>& texture) {
        return std::make_shared<Skybox>(texture);
    }
};


}