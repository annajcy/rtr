#pragma once

#include "../rhi_device.h"
#include "../rhi_frame_buffer.h"
#include "../rhi_pipeline_state.h"
#include "../rhi_renderer.h"
#include "../rhi_window.h"

#include "engine/runtime/platform/rhi/rhi_texture.h"
#include "rhi_compute_opengl.h"
#include "rhi_error_opengl.h"
#include "rhi_renderer_opengl.h"
#include "rhi_buffer_opengl.h"
#include "rhi_geometry_opengl.h"
#include "rhi_window_opengl.h"
#include "rhi_shader_code_opengl.h"
#include "rhi_shader_program_opengl.h"
#include "rhi_pipeline_state_opengl.h"
#include "rhi_texture_opengl.h"
#include "rhi_frame_buffer_opengl.h"
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

    static std::shared_ptr<RHI_device> create() {
        return std::make_shared<RHI_device_OpenGL>();
    }

    std::shared_ptr<RHI_window> create_window(
        int width,
        int height,
        const std::string& title
    ) override {
        return std::make_shared<RHI_window_OpenGL>(
            width,
            height,
            title
        );
    }

    std::shared_ptr<RHI_buffer> create_vertex_buffer(
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

    std::shared_ptr<RHI_buffer> create_element_buffer(
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

    std::shared_ptr<RHI_buffer> create_memory_buffer(
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

    std::shared_ptr<RHI_geometry> create_geometry(
        const std::unordered_map<unsigned int, std::shared_ptr<RHI_buffer>> &vertex_buffers,
        const std::shared_ptr<RHI_buffer>& element_buffer
    ) override {
        return std::make_shared<RHI_geometry_OpenGL>(
            vertex_buffers,
            element_buffer
        );
    }

    std::shared_ptr<RHI_shader_code> create_shader_code(
        Shader_type type, 
        const std::string& code
    ) override {
        return std::make_shared<RHI_shader_code_OpenGL>(
            type,
            code
        );
    }

    std::shared_ptr<RHI_shader_program> create_shader_program(
        const std::unordered_map<Shader_type, std::shared_ptr<RHI_shader_code>>& shader_codes,
        const std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>& uniforms
    ) override {
        return std::make_shared<RHI_shader_program_OpenGL>(
            shader_codes,
            uniforms
        );
    }

    std::shared_ptr<RHI_texture> create_texture_2D(
        int width,
        int height,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filters
    ) override {
        return std::make_shared<RHI_texture_2D_OpenGL>(
            width,
            height,
            mipmap_levels,
            internal_format,
            wraps,
            filters
        );
    }

    std::shared_ptr<RHI_texture> create_texture_cubemap(
        int width,
        int height,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filters
    ) override {
        return std::make_shared<RHI_texture_cubemap_OpenGL>(
            width,
            height,
            mipmap_levels,
            internal_format,
            wraps,
            filters
        );
    }

    std::shared_ptr<RHI_texture> create_texture_2D_array(
        int width,
        int height,
        unsigned int mipmap_levels,
        Texture_internal_format internal_format,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filters,
        unsigned int layer_count
    ) override {
        return std::make_shared<RHI_texture_2D_array_OpenGL>(
            width,
            height,
            mipmap_levels,
            internal_format,
            wraps,
            filters,
            layer_count
        );
    }

    std::shared_ptr<RHI_screen_buffer> create_screen_buffer(
        const std::shared_ptr<RHI_window>& window
    ) override {
        return std::make_shared<RHI_screen_buffer_OpenGL>(window);
    }

    std::shared_ptr<RHI_frame_buffer> create_frame_buffer(
        int width, 
        int height,
        const std::vector<std::shared_ptr<RHI_texture>>& color_attachments,
        const std::shared_ptr<RHI_texture>& depth_attachment
    ) override {
        return std::make_shared<RHI_frame_buffer_OpenGL>(
            width,
            height,
            color_attachments,
            depth_attachment
        );
    }

    std::shared_ptr<RHI_compute_task> create_compute_task(
        const std::shared_ptr<RHI_shader_program>& shader_program
    ) override {
        return std::make_shared<RHI_compute_task_OpenGL>(shader_program);
    }

    std::shared_ptr<RHI_memory_buffer_binder> create_memory_buffer_binder() override {
        return std::make_shared<RHI_memory_binder_OpenGL>();
    }

    std::shared_ptr<RHI_texture_builder> create_texture_builder() override {
        return std::make_shared<RHI_texture_builder_OpenGL>();
    }

    std::shared_ptr<RHI_renderer> create_renderer(const Clear_state& clear_state) override {
        return std::make_shared<RHI_renderer_OpenGL>(clear_state);
    }

    std::shared_ptr<RHI_pipeline_state> create_pipeline_state(const Pipeline_state& pipeline_state) override {
        return std::make_shared<RHI_pipeline_state_OpenGL>(pipeline_state);
    }

    std::shared_ptr<RHI_pipeline_state> create_pipeline_state() override {
        return std::make_shared<RHI_pipeline_state_OpenGL>();
    }

};


};



