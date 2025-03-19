#pragma once

#include "engine/global/base.h" 

#include "engine/runtime/rhi/device/rhi_device.h"

#include "engine/runtime/rhi/buffer/rhi_buffer_opengl.h"
#include "engine/runtime/rhi/geometry/rhi_geometry_opengl.h"

#include "engine/runtime/rhi/window/rhi_window_opengl.h"

#include "engine/runtime/rhi/shader/rhi_shader_code_opengl.h"
#include "engine/runtime/rhi/shader/rhi_shader_program_opengl.h"

#include "engine/runtime/rhi/state/rhi_pipeline_state_opengl.h"

#include "engine/runtime/rhi/texture/rhi_texture_opengl.h"
#include "engine/runtime/rhi/frame_buffer/rhi_frame_buffer_opengl.h"

namespace rtr {
    

class RHI_device_OpenGL : public RHI_device {
public:

    RHI_device_OpenGL(const RHI_device_descriptor& device_descriptor) : RHI_device(API_type::OPENGL, device_descriptor) { init(); }
    virtual ~RHI_device_OpenGL() override { destroy(); }

    virtual void init() override {

        m_window = std::make_shared<RHI_window_OpenGL>(
            m_device_descriptor.width,
            m_device_descriptor.height,
            m_device_descriptor.title
        );

        m_binding_state = std::make_shared<RHI_binding_state>();
        m_pipeline_state = std::make_shared<RHI_pipeline_state_OpenGL>();
    }

    virtual void destroy() override { }

    virtual void check_error() override {
        GLenum error_code = glGetError();
        std::string error;
        if (error_code != GL_NO_ERROR) {
            switch (error_code)
            {
                case GL_INVALID_ENUM: error = "INVALID_ENUM"; break;
                case GL_INVALID_VALUE:  error = "INVALID_VALUE"; break;
                case GL_INVALID_OPERATION: error = "INVALID_OPERATION"; break;
                case GL_OUT_OF_MEMORY: error = "OUT OF MEMORY"; break;
                default:
                    error = "UNKNOWN";
                    break;
            }
            std::cout << error << std::endl;

            assert(false);
        }
    }

    virtual std::shared_ptr<RHI_vertex_buffer> create_vertex_buffer(
        Buffer_usage usage,
        Buffer_attribute_type attribute_type,
        Buffer_iterate_type iterate_type,
        unsigned int unit_count,
        unsigned int data_count,
        void* data
    ) override {
        return std::make_shared<RHI_vertex_buffer_OpenGL>(
            usage,
            attribute_type,
            iterate_type,
            unit_count,
            data_count,
            data
        );
    }
    
    virtual std::shared_ptr<RHI_element_buffer> create_element_buffer(
        Buffer_usage usage,
        unsigned int data_count,
        void* data
    ) override {
        return std::make_shared<RHI_element_buffer_OpenGL>(
            usage,
            data_count,
            data
        );
    }

    virtual std::shared_ptr<RHI_geometry> create_geometry(
        const std::unordered_map<unsigned int, std::shared_ptr<RHI_vertex_buffer>>& vertex_buffers,
        const std::shared_ptr<RHI_element_buffer>& element_buffer
    ) override {
        return std::make_shared<RHI_geometry_OpenGL>(
            vertex_buffers,
            element_buffer
        );
    }

    virtual std::shared_ptr<RHI_shader_code> create_shader_code(
        Shader_type type, 
        const std::string& code
    ) override {
        return std::make_shared<RHI_shader_code_OpenGL>(type, code);
    }

    virtual std::shared_ptr<RHI_shader_program> create_shader_program(
        const std::unordered_map<Shader_type, std::shared_ptr<RHI_shader_code>>& shaders
    ) override {
        return std::make_shared<RHI_shader_program_OpenGL>(shaders);
    }

    virtual std::shared_ptr<RHI_texture_2D> create_texture_2D(
        Texture_format internal_format,
        Texture_format external_format,
        Texture_buffer_type buffer_type,
        int width,
        int height,
        unsigned char* data
    ) override {
        return std::make_shared<RHI_texture_2D_OpenGL>(
            internal_format,
            external_format,
            buffer_type,
            width,
            height,
            data
        );
    }

    virtual std::shared_ptr<RHI_texture_2D> create_texture_2D(
        int width,
        int height,
        unsigned char* data
    ) override {
        return std::make_shared<RHI_texture_2D_OpenGL>(
            width,
            height,
            data
        );
    }

    virtual std::shared_ptr<RHI_texture_cube_map> create_texture_cube_map(
        Texture_format internal_format,
        Texture_format external_format,
        Texture_buffer_type buffer_type,
        std::unordered_map<Texture_cube_map_face, RHI_texture_cube_map::Face_data> face_data
    ) override {
        return std::make_shared<RHI_texture_cube_map_OpenGL>(
            internal_format,
            external_format,
            buffer_type,
            face_data
        );
    }

    virtual std::shared_ptr<RHI_texture_cube_map> create_texture_cube_map(
        std::unordered_map<Texture_cube_map_face, RHI_texture_cube_map::Face_data> face_data
    ) override {
        return std::make_shared<RHI_texture_cube_map_OpenGL>(
            face_data
        );
    }

    virtual std::shared_ptr<RHI_texture_2D> create_color_attachment(
        int width,
        int height
    ) override {
        return std::make_shared<RHI_texture_2D_OpenGL>(
            width,
            height,
            nullptr
        );
    }

    virtual std::shared_ptr<RHI_texture_2D> create_depth_attachment(
        int width,
        int height
    ) override {
        return std::make_shared<RHI_texture_2D_OpenGL>(
            Texture_format::DEPTH_STENCIL_24_8,
            Texture_format::DEPTH_STENCIL,
            Texture_buffer_type::UNSIGNED_INT_24_8,
            width,
            height,
            nullptr
        );
    }

    virtual std::shared_ptr<RHI_frame_buffer> create_frame_buffer(
        int width,
        int height,
        int color_attachment_count
    ) override {
        auto frame_buffer = std::make_shared<RHI_frame_buffer_OpenGL>(
            width,
            height
        );

        for (int i = 0; i < color_attachment_count; i++) {
            frame_buffer->add_color_attachment(create_color_attachment(width, height));
        }

        frame_buffer->depth_attachment() = create_depth_attachment(width, height);
        frame_buffer->attach();
        if (!frame_buffer->is_valid()) {
            std::cout << "Frame buffer is not valid" << std::endl;
            return nullptr;
        } else {
            std::cout << "Frame buffer is valid" << std::endl;
        }
        return frame_buffer;
    }

    virtual void clear() override {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glClear(m_pipeline_state->clear_mask());
    }
    
};


};



