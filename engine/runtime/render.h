#pragma once
#include "engine/global/base.h"
#include "engine/runtime/camera.h"
#include "engine/runtime/frame_buffer.h"
#include "engine/runtime/light.h"
#include "engine/runtime/material.h"
#include "engine/runtime/mesh.h"
#include "engine/runtime/node.h"
#include "engine/runtime/rhi/device/rhi_device.h"
#include "engine/runtime/rhi/device/rhi_device_opengl.h"
#include "engine/runtime/scene.h"
#include <memory>

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
    std::shared_ptr<Camera> m_camera{};
    std::shared_ptr<Light_setting> m_light_setting{};
    std::shared_ptr<Frame_buffer> m_frame_buffer{};
    std::shared_ptr<Material> m_override_material{};

    std::vector<Mesh> m_opaque_meshes{};
    std::vector<Mesh> m_transparent_meshes{};

public:
    Renderer(const Renderer_descriptor& descriptor) {

        RHI_device_descriptor device_descriptor{};
        device_descriptor.width = descriptor.width;
        device_descriptor.height = descriptor.height;
        device_descriptor.title = descriptor.title;

        m_device = std::make_shared<RHI_device_OpenGL>(device_descriptor);

    }

    std::shared_ptr<Scene>& scene() { return m_scene; }
    std::shared_ptr<Camera>& camera() { return m_camera; }
    std::shared_ptr<Light_setting>& light_setting() { return m_light_setting; }
    std::shared_ptr<Frame_buffer>& frame_buffer() { return m_frame_buffer; }
    std::shared_ptr<Material>& override_material() { return m_override_material; }


    ~Renderer() = default;
    void render() {

    }

    void parse_scene() {
        m_opaque_meshes.clear();
        m_transparent_meshes.clear();

        for (auto& child : m_scene->children()) {
            if (child == nullptr) {
                continue;
            }

            if (child->type() == Node_type::MESH) {
                auto mesh = std::dynamic_pointer_cast<Mesh>(child);
            } else if (child->type() == Node_type::CAMERA) {
                auto camera = std::dynamic_pointer_cast<Camera>(child);
            } else if (child->type() == Node_type::LIGHT) {
                auto light = std::dynamic_pointer_cast<Light>(child);
            }

            
        }


    }

    void render_mesh(const std::shared_ptr<Mesh>& mesh) {

    }


};

}