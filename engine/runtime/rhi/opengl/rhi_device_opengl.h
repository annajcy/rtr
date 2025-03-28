#pragma once

#include "engine/global/base.h" 

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

namespace rtr {

class RHI_device_OpenGL : public RHI_device {
public:

    RHI_device_OpenGL() : RHI_device(API_type::OPENGL) { }
    virtual ~RHI_device_OpenGL() override = default;
    
    // 新增虚函数实现
    void check_error() override;

    RHI_window* create_window(int width, int height, const std::string& title) override {
        return new RHI_window_OpenGL(width, height, title);
    }

    RHI_buffer* create_buffer(Buffer_type type, Buffer_usage usage, 
                            unsigned int data_size, const void* data) override {
        return new RHI_buffer_OpenGL(type, usage, data_size, data);
    }

    RHI_geometry* create_geometry(
        const std::unordered_map<unsigned int, Vertex_buffer_descriptor>& vertex_buffers,
        const Element_buffer_descriptor& element_buffer) override {
        return new RHI_geometry_OpenGL(vertex_buffers, element_buffer);
    }

    RHI_shader_code* create_shader_code(Shader_type type, const std::string& code) override {
        return new RHI_shader_code_OpenGL(type, code);
    }

    RHI_shader_program* create_shader_program(
        const std::unordered_map<Shader_type, unsigned int>& shader_codes,
        const std::unordered_map<std::string, RHI_uniform_entry>& uniforms,
        const std::unordered_map<std::string, RHI_uniform_array_entry>& uniform_arrays) override {
        return new RHI_shader_program_OpenGL(shader_codes, uniforms, uniform_arrays);
    }

    RHI_texture* create_texture_2D(
        int width, int height,
        unsigned int mipmap_levels,
        Texture_format internal_format,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filters,
        const Image_data& image) override {
        return new RHI_texture_2D_OpenGL(width, height, mipmap_levels, internal_format, 
                                    wraps, filters, image);
    }

    RHI_texture* create_texture_cubemap(
        int width, int height,
        unsigned int mipmap_levels,
        Texture_format internal_format,
        const std::unordered_map<Texture_wrap_target, Texture_wrap>& wraps,
        const std::unordered_map<Texture_filter_target, Texture_filter>& filters,
        const std::unordered_map<Texture_cubemap_face, Image_data>& images) override {
        return new RHI_texture_cubemap_OpenGL(width, height, mipmap_levels, internal_format, 
                                    wraps, filters, images);
    }

    RHI_frame_buffer* create_frame_buffer(
        int width, int height,
        const std::vector<unsigned int>& color_attachments,
        unsigned int depth_attachment) override {
        return new RHI_frame_buffer_OpenGL(width, height, color_attachments, depth_attachment);
    }

    RHI_pipeline_state* create_pipeline_state() override {
        return new RHI_pipeline_state_OpenGL();
    }
};


};



