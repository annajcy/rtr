#pragma once
#include "engine/global/base.h"
#include "engine/runtime/camera.h"
#include "engine/runtime/frame_buffer.h"
#include "engine/runtime/light.h"
#include "engine/runtime/mesh.h"
#include "engine/runtime/rhi/device/rhi_device.h"
#include "engine/runtime/rhi/device/rhi_device_opengl.h"
#include "engine/runtime/scene.h"

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

    virtual ~Renderer() = default;
    virtual void render() {

    }
};

}