#pragma once
#include "engine/global/base.h"
#include "camera.h"
#include "frame_buffer.h"
#include "geometry.h"
#include "light.h"
#include "material.h"
#include "mesh.h"
#include "node.h"

#include "scene.h"
#include "texture.h"


namespace rtr {

class Renderer {
protected:

    std::shared_ptr<Camera_setting> m_camera_setting{};
    std::shared_ptr<Light_setting> m_light_setting{};

    std::shared_ptr<RHI_device> m_device{};
    std::shared_ptr<Scene> m_scene{};
    
    std::shared_ptr<Frame_buffer> m_frame_buffer{};
    std::shared_ptr<Material> m_override_material{};

    std::vector<std::shared_ptr<Mesh>> m_opaque_meshes{};
    std::vector<std::shared_ptr<Mesh>> m_transparent_meshes{};

public:
    Renderer(const std::shared_ptr<RHI_device>& backend) : 
    m_device(backend),
    m_light_setting(std::make_shared<Light_setting>()),
    m_camera_setting(std::make_shared<Camera_setting>()) {}

    std::shared_ptr<RHI_device>& device() { return m_device; }
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
        

        //std::cout << "render mesh " << mesh->id() << std::endl;
        
        // mesh->upload_uniform(material->shader());
        // material->upload_uniform(material->shader());
        // m_light_setting.upload_uniform(material->shader());
        // m_camera_setting.upload_uniform(material->shader());

        // m_device->binding_state()->geometry() = geometry->create_rhi_geometry(m_device);
        // m_device->binding_state()->shader_program() = material->shader()->create_rhi_shader_program(m_device);
        
        // for (auto& [_, binded_texture] : material->textures()) {
        //     auto [id, texture] = binded_texture;
        //     if (texture->type() == Texture_type::TEXTURE_2D) {
        //         auto texture_2d = std::dynamic_pointer_cast<Texture_2D>(texture);
        //         m_device->binding_state()->textures_2D()[id] = texture_2d->create_rhi_texture_2D(m_device);
        //     } else if (texture->type() == Texture_type::TEXTURE_CUBE_MAP) {
        //         auto cube_map = std::dynamic_pointer_cast<Texture_cube_map>(texture);
        //         m_device->binding_state()->textures_cube_map()[id] = cube_map->create_rhi_texture_cube_map(m_device);
        //     }
            
        // }

        // if (m_frame_buffer)
        //     m_device->binding_state()->frame_buffer() = m_frame_buffer->create_rhi_frame_buffer(m_device);

        // material->update_pipeline_state(m_device);
        // m_device->pipeline_state()->apply();

        // m_device->binding_state()->bind();

        // if (auto instanced_material = std::dynamic_pointer_cast<Instanced_Material_base>(material)) {
        //     m_device->instanced_draw(instanced_material->instance_count());
        // } else {
        //     m_device->draw();
        // }

        // m_device->binding_state()->unbind();
        // m_device->binding_state()->clear();

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