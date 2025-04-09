#pragma once

#include "engine/runtime/global/base.h" 
#include "rhi_imgui.h"
#include "rhi_buffer.h"
#include "rhi_compute.h"
#include "rhi_frame_buffer.h"
#include "rhi_geometry.h"
#include "rhi_pipeline_state.h"
#include "rhi_renderer.h"
#include "rhi_shader_code.h"
#include "rhi_shader_program.h"
#include "rhi_texture.h"
#include "rhi_window.h"

namespace rtr {

class RHI_device {

protected:
    API_type m_api_type{};

public:

    using Ptr = std::shared_ptr<RHI_device>;
    
    RHI_device(API_type api_type) : m_api_type(api_type) {}
    virtual ~RHI_device() = default;
    const API_type& api_type() { return m_api_type; }

    virtual void check_error() = 0;

    virtual RHI_window::Ptr create_window(
        int width,
        int height,
        const std::string& title
    ) = 0;

    virtual RHI_buffer::Ptr create_vertex_buffer(
        Buffer_usage usage,
        Buffer_data_type attribute_type,
        Buffer_iterate_type iterate_type,
        unsigned int unit_data_count,
        unsigned int data_size,
        const void* data
    )= 0;

    virtual RHI_buffer::Ptr create_element_buffer(
        Buffer_usage usage,
        unsigned int data_count,
        unsigned int data_size,
        const void* data
    )= 0;

    virtual RHI_buffer::Ptr create_memory_buffer(
        Buffer_type type,
        Buffer_usage usage,
        unsigned int data_size,
        const void* data
    ) = 0;

    virtual RHI_geometry::Ptr create_geometry(
        const std::unordered_map<unsigned int, RHI_buffer::Ptr> &vertex_buffers,
        const RHI_buffer::Ptr& element_buffer
    ) = 0;

    virtual RHI_shader_code::Ptr create_shader_code(
        Shader_type type, 
        const std::string& code
    ) = 0;

    virtual RHI_shader_program::Ptr create_shader_program(
        const std::unordered_map<Shader_type, RHI_shader_code::Ptr>& shader_codes,
        const std::unordered_map<std::string, RHI_uniform_entry_base::Ptr>& uniforms
    ) = 0;

    virtual RHI_texture::Ptr create_texture_2D(
        int width,
        int height,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filters,
        const Image_data& image
    ) = 0;

    virtual RHI_texture::Ptr create_texture_cubemap(
        int width,
        int height,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filters,
        const std::unordered_map<Texture_cubemap_face, Image_data>& images
    ) = 0;

    virtual RHI_frame_buffer::Ptr create_frame_buffer(
        int width, 
        int height,
        const std::vector<RHI_texture::Ptr>& color_attachments,
        const RHI_texture::Ptr& depth_attachment
    ) = 0;

    virtual RHI_frame_buffer::Ptr create_screen_frame_buffer(
        const RHI_window::Ptr& window
    ) = 0;
    virtual RHI_compute_task::Ptr create_compute_task(
        const RHI_shader_program::Ptr& shader_program
    ) = 0;

    virtual RHI_memory_buffer_binder::Ptr create_memory_buffer_binder() = 0;

    virtual RHI_renderer::Ptr create_renderer(const Clear_state& clear_state) = 0;

    virtual RHI_pipeline_state::Ptr create_pipeline_state() = 0;
    virtual RHI_pipeline_state::Ptr create_pipeline_state(const Pipeline_state& pipeline_state) = 0;

    virtual RHI_imgui::Ptr create_imgui(const RHI_window::Ptr& window) = 0;
    


};

};



