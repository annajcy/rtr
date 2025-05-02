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
#include <memory>

namespace rtr {

class RHI_device {

protected:
    API_type m_api_type{};

public:
    
    RHI_device(API_type api_type) : m_api_type(api_type) {}
    virtual ~RHI_device() = default;
    const API_type& api_type() { return m_api_type; }

    virtual void check_error() = 0;

    virtual std::shared_ptr<RHI_window> create_window(
        int width,
        int height,
        const std::string& title
    ) = 0;

    virtual std::shared_ptr<RHI_buffer> create_vertex_buffer(
        Buffer_usage usage,
        Buffer_data_type attribute_type,
        Buffer_iterate_type iterate_type,
        unsigned int unit_data_count,
        unsigned int data_size,
        const void* data
    ) = 0;

    virtual std::shared_ptr<RHI_buffer> create_element_buffer(
        Buffer_usage usage,
        unsigned int data_count,
        unsigned int data_size,
        const void* data
    ) = 0;

    virtual std::shared_ptr<RHI_buffer> create_memory_buffer(
        Buffer_type type,
        Buffer_usage usage,
        unsigned int data_size,
        const void* data
    ) = 0;

    virtual std::shared_ptr<RHI_geometry> create_geometry(
        const std::unordered_map<unsigned int, std::shared_ptr<RHI_buffer>> &vertex_buffers,
        const std::shared_ptr<RHI_buffer>& element_buffer
    ) = 0;

    virtual std::shared_ptr<RHI_shader_code> create_shader_code(
        Shader_type type, 
        const std::string& code
    ) = 0;

    virtual std::shared_ptr<RHI_shader_program> create_shader_program(
        const std::unordered_map<Shader_type, std::shared_ptr<RHI_shader_code>>& shader_codes,
        const std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>& uniforms
    ) = 0;

    virtual std::shared_ptr<RHI_texture> create_texture_2D(
        int width,
        int height,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filters,
        const Image_data& image
    ) = 0;

    virtual std::shared_ptr<RHI_texture> create_texture_2D_array(
        int width,
        int height,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filters,
        const std::vector<Image_data>& images
    ) = 0;

    std::shared_ptr<RHI_texture> create_texture_color_attachment(
        int width,
        int height
    ) {
        return create_texture_2D(
            width,
            height,
            1,
            Texture_internal_format::RGB_ALPHA,
            {
                {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_EDGE},
                {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_EDGE}
            },
            {
                {Texture_filter_target::MIN, Texture_filter::LINEAR},
                {Texture_filter_target::MAG, Texture_filter::LINEAR}
            },
            Image_data{}
        );
    }

    std::shared_ptr<RHI_texture> create_texture_depth_stencil_attachment(
        int width,
        int height
    ) {
        return create_texture_2D(
            width,
            height,
            1,
            Texture_internal_format::DEPTH_STENCIL_24_8,
            std::unordered_map<Texture_wrap_target, Texture_wrap>{
                {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_EDGE},
                {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_EDGE}
            },
            std::unordered_map<Texture_filter_target, Texture_filter>{
                // 修改过滤模式为NEAREST
                {Texture_filter_target::MIN, Texture_filter::NEAREST},
                {Texture_filter_target::MAG, Texture_filter::NEAREST}
            },
            Image_data{}
        );
    }

    std::shared_ptr<RHI_texture> create_texture_depth_attachment(
        int width,
        int height
    ) {
        return create_texture_2D(
            width,
            height,
            1,
            Texture_internal_format::DEPTH_32F,
            std::unordered_map<Texture_wrap_target, Texture_wrap>{
                {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_BORDER},
                {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_BORDER}
            },
            std::unordered_map<Texture_filter_target, Texture_filter>{
                {Texture_filter_target::MIN, Texture_filter::NEAREST},
                {Texture_filter_target::MAG, Texture_filter::NEAREST}
            },
            Image_data{}
        );
    }

    std::shared_ptr<RHI_texture> create_texture_depth_attachment_array(
        int width,
        int height,
        int array_size
    ) {
        return create_texture_2D_array(
            width,
            height,
            1,
            Texture_internal_format::DEPTH_32F,
            std::unordered_map<Texture_wrap_target, Texture_wrap>{
                {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_BORDER},
                {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_BORDER}
            },
            std::unordered_map<Texture_filter_target, Texture_filter>{
                {Texture_filter_target::MIN, Texture_filter::NEAREST},
                {Texture_filter_target::MAG, Texture_filter::NEAREST}
            },
            std::vector<Image_data>(array_size)
        );
    }

    virtual std::shared_ptr<RHI_texture> create_texture_cubemap(
        int width,
        int height,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filters,
        const std::unordered_map<Texture_cubemap_face, Image_data>& images
    ) = 0;

    virtual std::shared_ptr<RHI_frame_buffer> create_frame_buffer(
        int width, 
        int height,
        const std::vector<std::shared_ptr<RHI_texture>>& color_attachments,
        const std::shared_ptr<RHI_texture>& depth_attachment
    ) = 0;

    virtual std::shared_ptr<RHI_screen_buffer> create_screen_buffer(
        const std::shared_ptr<RHI_window>& window
    ) = 0;
    
    virtual RHI_compute_task::Ptr create_compute_task(
        const std::shared_ptr<RHI_shader_program>& shader_program
    ) = 0;

    virtual std::shared_ptr<RHI_memory_buffer_binder> create_memory_buffer_binder() = 0;
   
    virtual std::shared_ptr<RHI_texture_builder> create_texture_builder() = 0;

    virtual std::shared_ptr<RHI_renderer> create_renderer(const Clear_state& clear_state) = 0;

    virtual std::shared_ptr<RHI_pipeline_state> create_pipeline_state() = 0;

    virtual std::shared_ptr<RHI_pipeline_state> create_pipeline_state(const Pipeline_state& pipeline_state) = 0;

    virtual std::shared_ptr<RHI_imgui> create_imgui(const std::shared_ptr<RHI_window>& window) = 0;
    


};

};



