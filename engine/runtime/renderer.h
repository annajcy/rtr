#pragma once
#include "engine/global/base.h"
#include "engine/runtime/camera.h"
#include "engine/runtime/frame_buffer.h"
#include "engine/runtime/geometry.h"
#include "engine/runtime/light.h"
#include "engine/runtime/material.h"
#include "engine/runtime/mesh.h"
#include "engine/runtime/node.h"
#include "engine/runtime/rhi/device/rhi_device.h"
#include "engine/runtime/rhi/device/rhi_device_opengl.h"
#include "engine/runtime/rhi/texture/rhi_texture.h"
#include "engine/runtime/scene.h"
#include "glm/fwd.hpp"
#include <array>
#include <cstddef>
#include <memory>
#include <vector>

namespace rtr {

class Renderer_descriptor {
public:
    unsigned int width{800};
    unsigned int height{600};
    std::string title{"RTR Engine"};
};

class Renderer {
protected:
    std::shared_ptr<RHI_device> m_device{};
    std::shared_ptr<Scene> m_scene{};
    std::shared_ptr<Camera_setting> m_camera_setting{};
    std::shared_ptr<Light_setting> m_light_setting{};
    std::shared_ptr<Frame_buffer> m_frame_buffer{};
    std::shared_ptr<Material> m_override_material{};

    std::vector<std::shared_ptr<Mesh>> m_opaque_meshes{};
    std::vector<std::shared_ptr<Mesh>> m_transparent_meshes{};

public:
    Renderer(const Renderer_descriptor& descriptor) {

        RHI_device_descriptor device_descriptor{};
        device_descriptor.width = descriptor.width;
        device_descriptor.height = descriptor.height;
        device_descriptor.title = descriptor.title;

        m_device = std::make_shared<RHI_device_OpenGL>(device_descriptor);
        m_light_setting = std::make_shared<Light_setting>();
        m_camera_setting = std::make_shared<Camera_setting>();

    }

    std::shared_ptr<Scene>& scene() { return m_scene; }
    std::shared_ptr<Camera_setting>& camera() { return m_camera_setting; }
    std::shared_ptr<Light_setting>& light_setting() { return m_light_setting; }
    std::shared_ptr<Frame_buffer>& frame_buffer() { return m_frame_buffer; }
    std::shared_ptr<Material>& override_material() { return m_override_material; }

    ~Renderer() = default;


    void render() {
        parse_scene();
        sort_meshes();

        if (m_frame_buffer == nullptr) {
            std::cout << "frame buffer is nullptr, use screen" << std::endl;
            return;
        } else {
            m_device->binding_state()->frame_buffer() = m_frame_buffer->create_rhi_frame_buffer(m_device);
            m_device->binding_state()->frame_buffer()->bind();
        }

        
        for (auto& mesh : m_opaque_meshes) {
            render_mesh(mesh);
        }

        for (auto& mesh : m_transparent_meshes) {
            render_mesh(mesh);
        }

        if (m_frame_buffer != nullptr) {
            m_device->binding_state()->frame_buffer()->unbind();
        }

    }

    void parse_scene() {
        if (m_scene == nullptr) {
            std::cout << "scene is nullptr" << std::endl;
            return;
        }

        parse_node(m_scene);

    }

    void sort_meshes() {

        //TODO: sort meshes by material

        // std::sort(m_opaque_meshes.begin(), m_opaque_meshes.end(), [](const std::shared_ptr<Mesh>& a, const std::shared_ptr<Mesh>& b) {

        //     return true;
        // });

        // std::sort(m_transparent_meshes.begin(), m_transparent_meshes.end(), [](const std::shared_ptr<Mesh>& a, const std::shared_ptr<Mesh>& b) {

        //     return true;
        // });

    }

    void render_mesh(const std::shared_ptr<Mesh>& mesh) {
        if (mesh == nullptr) {
            std::cout << "mesh is nullptr" << std::endl;
            return;
        }

        auto geometry = mesh->geometry();
        auto material = mesh->material();  
        
        material->upload_uniform(material->shader());
        m_light_setting->upload_uniform(material->shader());
        m_camera_setting->upload_uniform(material->shader());
        

        if (m_override_material != nullptr) {
            material = m_override_material;
        }

        m_device->binding_state()->geometry() = geometry->create_rhi_geometry(m_device);
        m_device->binding_state()->geometry()->bind();

        m_device->binding_state()->shader_program() = material->shader()->create_rhi_shader_program(m_device);
        

        
       



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

        for (auto& child : m_scene->children()) {
            parse_node(child);
        }
    }

};

}