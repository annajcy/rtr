#pragma once

#include "engine/global/base.h" 
#include "engine/runtime/enum.h"
#include "engine/runtime/rhi/rhi_buffer.h"
#include "engine/runtime/rhi/rhi_frame_buffer.h"
#include "engine/runtime/rhi/rhi_geometry.h"
#include "engine/runtime/rhi/rhi_pipeline_state.h"
#include "engine/runtime/rhi/rhi_shader_code.h"
#include "engine/runtime/rhi/rhi_shader_program.h"
#include "engine/runtime/rhi/rhi_texture.h"
#include "engine/runtime/rhi/rhi_window.h"

namespace rtr {

class RHI_device {

protected:
    API_type m_api_type{};

public:
    
    RHI_device(API_type api_type) : m_api_type(api_type) {}
    virtual ~RHI_device() = default;
    const API_type& api_type() { return m_api_type; }

    virtual void check_error() = 0;

    virtual RHI_window* create_window(
        int width,
        int height,
        const std::string& title
    ) = 0;

    virtual RHI_buffer* create_buffer(
        Buffer_type type,
        Buffer_usage usage,
        unsigned int data_size,
        const void* data
    ) = 0;

    virtual RHI_geometry* create_geometry(
        const std::unordered_map<unsigned int, Vertex_buffer_descriptor> &vertex_buffers,
        const Element_buffer_descriptor& element_buffer
    ) = 0;

    virtual RHI_shader_code* create_shader_code(
        Shader_type type, 
        const std::string& code
    ) = 0;

    virtual RHI_shader_program* create_shader_program(
        const std::unordered_map<Shader_type, unsigned int>& shader_codes,
        const std::unordered_map<std::string, RHI_uniform_entry>& uniforms, 
        const std::unordered_map<std::string, RHI_uniform_array_entry>& uniform_arrays
    ) = 0;

    virtual RHI_texture* create_texture_2D(
        int width,
        int height,
        unsigned int mipmap_levels,
        Texture_format internal_format,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filters,
        const Image_data& image
    ) = 0;

    virtual RHI_texture* create_texture_cubemap(
        int width,
        int height,
        unsigned int mipmap_levels,
        Texture_format internal_format,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filters,
        const std::unordered_map<Texture_cubemap_face, Image_data>& images
    ) = 0;

    virtual RHI_frame_buffer* create_frame_buffer(
        int width, 
        int height,
        const std::vector<unsigned int>& color_attachments,
        unsigned int depth_attachment
    ) = 0;

    virtual RHI_pipeline_state* create_pipeline_state() = 0;

};

};



