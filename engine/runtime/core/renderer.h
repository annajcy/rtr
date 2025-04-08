#pragma once
#include "engine/global/base.h"

#include "engine/runtime/core/camera.h"
#include "engine/runtime/core/frame_buffer.h"
#include "engine/runtime/core/geometry.h"
#include "engine/runtime/core/light.h"
#include "engine/runtime/core/material.h"
#include "engine/runtime/core/mesh.h"
#include "engine/runtime/core/node.h"
#include "engine/runtime/core/scene.h"

namespace rtr {

class Renderer {
protected:

    std::shared_ptr<Camera_setting> m_camera_setting{};
    std::shared_ptr<Light_setting> m_light_setting{};
    std::shared_ptr<Scene> m_scene{};
    
    std::shared_ptr<Frame_buffer> m_frame_buffer{};
    std::shared_ptr<Material> m_override_material{};

    std::vector<std::shared_ptr<Mesh>> m_opaque_meshes{};
    std::vector<std::shared_ptr<Mesh>> m_transparent_meshes{};

public:
    Renderer() :
    m_light_setting(std::make_shared<Light_setting>()),
    m_camera_setting(std::make_shared<Camera_setting>()) {}
    std::shared_ptr<Scene>& scene() { return m_scene; }
    std::shared_ptr<Frame_buffer>& frame_buffer() { return m_frame_buffer; }
    std::shared_ptr<Material>& override_material() { return m_override_material; }
    std::shared_ptr<Camera_setting>& camera() { return m_camera_setting; }
    std::shared_ptr<Light_setting>& light_setting() { return m_light_setting; }

    ~Renderer() = default;

    void render() {
        parse_scene();

        for (auto& mesh : m_opaque_meshes) {
            render_mesh(mesh);
        }

        for (auto& mesh : m_transparent_meshes) {
            render_mesh(mesh);
        }

    }

    void parse_scene() {
        if (m_scene == nullptr) {
            std::cout << "scene is nullptr" << std::endl;
            return;
        }

        parse_node(m_scene);

    }

    void render_mesh(const std::shared_ptr<Mesh>& mesh) {
        if (mesh == nullptr) {
            std::cout << "mesh is nullptr" << std::endl;
            return;
        }

        std::shared_ptr<Geometry> geometry = mesh->geometry();

        std::shared_ptr<Material> material{};
        if (m_override_material != nullptr) {
            material = m_override_material;
        } else {
            material = mesh->material();
        }

        material->upload_uniform(mesh, m_camera_setting, m_light_setting);
        

    }

    void parse_node(const std::shared_ptr<Node>& node) {
        if (node == nullptr) {
            std::cout << "scene is nullptr" << std::endl;
            return;
        }

        if (node->type() == Node_type::MESH) {
            auto mesh = std::dynamic_pointer_cast<Mesh>(node);
            if (mesh->material()->is_translucent()) {
                m_transparent_meshes.push_back(mesh);
            } else {
                m_opaque_meshes.push_back(mesh);
            }

        } else if (node->type() == Node_type::CAMERA) {
            auto camera = std::dynamic_pointer_cast<Camera>(node);
            m_camera_setting->add_camera(camera);
        } else if (node->type() == Node_type::LIGHT) {
            auto light = std::dynamic_pointer_cast<Light>(node);
            m_light_setting->add_light(light);
        } 

        for (auto& child : node->children()) {
            parse_node(child);
        }
    }

};

}