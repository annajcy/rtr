#pragma once

#include "engine/global/base.h" 
#include "engine/runtime/rhi/buffer/rhi_buffer.h"
#include "engine/runtime/rhi/geometry/rhi_geometry.h"
#include "engine/runtime/rhi/window/rhi_window.h"
#include "engine/runtime/rhi/shader/rhi_shader_code.h"
#include "engine/runtime/rhi/shader/rhi_shader_program.h"
#include "engine/runtime/rhi/texture/rhi_texture.h"
#include "engine/runtime/rhi/state/rhi_pipeline_state.h"
#include "engine/runtime/rhi/state/rhi_binding_state.h"

namespace rtr {
    
enum class API_type {
    OPENGL,
    DIRECTX,
    VULKAN
};

struct RHI_device_descriptor {
    unsigned int width{800};
    unsigned int height{600};
    std::string title = "RTR Engine";
};

class RHI_device {

protected:
    API_type m_api_type{};
    RHI_device_descriptor m_device_descriptor{};

    std::shared_ptr<RHI_window> m_window{};
    std::shared_ptr<RHI_pipeline_state> m_pipeline_state{};
    std::shared_ptr<RHI_binding_state> m_binding_state{};


public:
    
    RHI_device(
        API_type api_type, 
        const RHI_device_descriptor& device_descriptor
    ) : m_api_type(api_type), 
        m_device_descriptor(device_descriptor) {}

    virtual ~RHI_device() = default;

    const API_type& api_type() { return m_api_type; }
    std::shared_ptr<RHI_window>& window() { return m_window; }
    std::shared_ptr<RHI_pipeline_state>& pipeline_state() { return m_pipeline_state; }
    std::shared_ptr<RHI_binding_state>& binding_state() { return m_binding_state; }
    const RHI_device_descriptor& device_descriptor() { return m_device_descriptor; }

    virtual void init() = 0;
    virtual void destroy() = 0;
    virtual void check_error() = 0;
    
    virtual std::shared_ptr<RHI_vertex_buffer> create_vertex_buffer(
        Buffer_usage usage,
        Buffer_attribute_type attribute_type,
        unsigned int unit_count,
        unsigned int data_count,
        void* data
    ) = 0;

    virtual std::shared_ptr<RHI_element_buffer> create_element_buffer(
        Buffer_usage usage,
        unsigned int data_count,
        void* data
    ) = 0;

    virtual std::shared_ptr<RHI_geometry> create_geometry(
        const std::unordered_map<unsigned int, std::shared_ptr<RHI_vertex_buffer>>& vertex_buffers,
        const std::shared_ptr<RHI_element_buffer>& element_buffer
    ) = 0;

    virtual std::shared_ptr<RHI_shader_code> create_shader_code(
        Shader_type type,
        const std::string& code
    ) = 0;

    virtual std::shared_ptr<RHI_shader_program> create_shader_program(
        const std::unordered_map<Shader_type, std::shared_ptr<RHI_shader_code>>& shaders
    ) = 0;

    virtual std::shared_ptr<RHI_texture_2D> create_texture_2D(
        Texture_format internal_format,
        Texture_format external_format,
        Texture_buffer_type buffer_type,
        int width,
        int height,
        unsigned char* data
    ) = 0;

    virtual std::shared_ptr<RHI_texture_2D> create_texture_2D(
        int width,
        int height,
        unsigned char* data
    ) = 0;

    virtual std::shared_ptr<RHI_texture_cube_map> create_texture_cube_map(
        Texture_format internal_format,
        Texture_format external_format,
        Texture_buffer_type buffer_type,
        std::unordered_map<Texture_cube_map_face, RHI_texture_cube_map::Face_data> face_textures
    ) = 0;

    virtual std::shared_ptr<RHI_texture_cube_map> create_texture_cube_map(
        std::unordered_map<Texture_cube_map_face, RHI_texture_cube_map::Face_data> face_textures
    ) = 0;

    virtual std::shared_ptr<RHI_texture_2D> create_color_attachment(
        int width,
        int height
    ) = 0;

    virtual std::shared_ptr<RHI_texture_2D> create_depth_attachment(
        int width,
        int height
    ) = 0;

    virtual std::shared_ptr<RHI_frame_buffer> create_frame_buffer(
        int width,
        int height,
        int color_attachment_count
    ) = 0;

    void draw() {
        m_pipeline_state->apply();

        m_binding_state->bind();
        m_binding_state->geometry()->draw();
        m_binding_state->unbind();
    }

    void instanced_draw(int instance_count) {
        m_pipeline_state->apply();

        m_binding_state->bind();
        m_binding_state->geometry()->instanced_draw(instance_count);
        m_binding_state->unbind();
    }

    virtual void clear() = 0;

    
};


};



