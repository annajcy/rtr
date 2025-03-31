#pragma once

#include "engine/global/base.h" 

#include "engine/runtime/rhi/opengl/rhi_error_opengl.h"
#include "engine/runtime/rhi/rhi_device.h"

#include "engine/runtime/rhi/opengl/rhi_buffer_opengl.h"
#include "engine/runtime/rhi/opengl/rhi_geometry_opengl.h"
#include "engine/runtime/rhi/opengl/rhi_window_opengl.h"
#include "engine/runtime/rhi/opengl/rhi_shader_code_opengl.h"
#include "engine/runtime/rhi/opengl/rhi_shader_program_opengl.h"
#include "engine/runtime/rhi/opengl/rhi_pipeline_state_opengl.h"
#include "engine/runtime/rhi/opengl/rhi_texture_opengl.h"
#include "engine/runtime/rhi/opengl/rhi_frame_buffer_opengl.h"
#include "engine/runtime/rhi/rhi_frame_buffer.h"
#include "engine/runtime/rhi/rhi_pipeline_state.h"
#include <cassert>
#include <memory>

namespace rtr {

class RHI_device_OpenGL : public RHI_device {
public:

    RHI_device_OpenGL() : RHI_device(API_type::OPENGL) { }
    virtual ~RHI_device_OpenGL() override = default;
    
    // 新增虚函数实现
    void check_error() override {
        gl_check_error();
    }

    static RHI_device::Ptr create() {
        return std::make_shared<RHI_device_OpenGL>();
    }

    RHI_window::Ptr create_window(
        int width,
        int height,
        const std::string& title,
        const Clear_state& clear_state = Clear_state::enabled()
    ) override {
        return std::make_shared<RHI_window_OpenGL>(
            width,
            height,
            title,
            clear_state
        );
    }

    RHI_buffer::Ptr create_vertex_buffer(
        Buffer_usage usage,
        Buffer_data_type attribute_type,
        Buffer_iterate_type iterate_type,
        unsigned int unit_data_count,
        unsigned int data_size,
        const void* data
    ) override {
        return std::make_shared<RHI_vertex_buffer_OpenGL>(
            usage,
            attribute_type,
            iterate_type,
            unit_data_count,
            data_size,
            data
        );
    }

    RHI_buffer::Ptr create_element_buffer(
        Buffer_usage usage,
        unsigned int data_count,
        unsigned int data_size,
        const void* data
    ) override {
        return std::make_shared<RHI_element_buffer_OpenGL>(
            usage,
            data_count,
            data_size,
            data
        );
    }

    RHI_buffer::Ptr create_memory_buffer(
        Buffer_type type,
        Buffer_usage usage,
        unsigned int data_size,
        const void* data
    ) override {
        return std::make_shared<RHI_memory_buffer_OpenGL>(
            type,
            usage,
            data_size,
            data
        );
    }

    RHI_geometry::Ptr create_geometry(
        const std::unordered_map<unsigned int, RHI_buffer::Ptr> &vertex_buffers,
        const RHI_buffer::Ptr& element_buffer
    ) override {
        return std::make_shared<RHI_geometry_OpenGL>(
            vertex_buffers,
            element_buffer
        );
    }

    RHI_shader_code::Ptr create_shader_code(
        Shader_type type, 
        const std::string& code
    ) override {
        return std::make_shared<RHI_shader_code_OpenGL>(
            type,
            code
        );
    }

    RHI_shader_program::Ptr create_shader_program(
        const std::unordered_map<Shader_type, RHI_shader_code::Ptr>& shader_codes,
        const std::unordered_map<std::string, RHI_uniform_entry>& uniforms, 
        const std::unordered_map<std::string, RHI_uniform_array_entry>& uniform_arrays
    ) override {
        return std::make_shared<RHI_shader_program_OpenGL>(
            shader_codes,
            uniforms,
            uniform_arrays
        );
    }

    RHI_texture::Ptr create_texture_2D(
        int width,
        int height,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filters,
        const Image_data& image
    ) override {
        return std::make_shared<RHI_texture_2D_OpenGL>(
            width,
            height,
            mipmap_levels,
            internal_format,
            wraps,
            filters,
            image
        );
    }

    RHI_texture::Ptr create_texture_cubemap(
        int width,
        int height,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filters,
        const std::unordered_map<Texture_cubemap_face, Image_data>& images
    ) override {
        return std::make_shared<RHI_texture_cubemap_OpenGL>(
            width,
            height,
            mipmap_levels,
            internal_format,
            wraps,
            filters,
            images
        );
    }

    RHI_frame_buffer::Ptr create_frame_buffer(
        int width, 
        int height,
        const std::vector<RHI_texture::Ptr>& color_attachments,
        const RHI_texture::Ptr& depth_attachment
    ) override {
        return std::make_shared<RHI_frame_buffer_OpenGL>(
            width,
            height,
            color_attachments,
            depth_attachment
        );
    }

    RHI_pipeline_state::Ptr create_pipeline_state(const Pipeline_state& pipeline_state) override {
        return std::make_shared<RHI_pipeline_state_OpenGL>(pipeline_state);
    }

    RHI_pipeline_state::Ptr create_pipeline_state() override {
        return std::make_shared<RHI_pipeline_state_OpenGL>();
    }
};


};



