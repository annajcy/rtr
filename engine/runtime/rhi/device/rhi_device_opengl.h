#pragma once

#include "engine/global/base.h" 

#include "engine/runtime/rhi/device/rhi_device.h"

#include "engine/runtime/rhi/buffer/rhi_buffer_opengl.h"
#include "engine/runtime/rhi/geometry/rhi_geometry_opengl.h"

#include "engine/runtime/rhi/window/rhi_window_opengl.h"

#include "engine/runtime/rhi/shader/rhi_shader_code_opengl.h"
#include "engine/runtime/rhi/shader/rhi_shader_program_opengl.h"

#include "engine/runtime/rhi/state/rhi_binding_state_opengl.h"
#include "engine/runtime/rhi/state/rhi_pipeline_state_opengl.h"

namespace rtr {
    

class RHI_device_OpenGL : public RHI_device {
public:
    RHI_device_OpenGL(const RHI_device_descriptor& device_descriptor) : RHI_device(API_type::OPENGL, device_descriptor) { init(); }
    virtual ~RHI_device_OpenGL() override { destroy(); }

    virtual void init() override {

        m_window = std::make_shared<RHI_window_OpenGL>(
            m_device_descriptor.m_width,
            m_device_descriptor.m_height,
            m_device_descriptor.m_title
        );

        m_pipeline_state = std::make_shared<RHI_pipeline_state_OpenGL>();

    }

    virtual std::shared_ptr<RHI_vertex_buffer> create_vertex_buffer(
        Buffer_usage usage,
        Buffer_attribute_type attribute_type,
        unsigned int unit_count,
        unsigned int data_count,
        void* data
    ) override {
        return std::make_shared<RHI_vertex_buffer_OpenGL>(
            usage,
            attribute_type,
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

    virtual std::shared_ptr<RHI_geometry> create_geometry() override {
        return std::make_shared<RHI_geometry_OpenGL>();
    }

    virtual std::shared_ptr<RHI_shader_code> create_shader_code(Shader_type type, const std::string& code) override {
        return std::make_shared<RHI_shader_code_OpenGL>(type, code);
    }

    virtual std::shared_ptr<RHI_shader_program> create_shader_program(const std::vector<std::shared_ptr<RHI_shader_code>>& shaders) override {
        return std::make_shared<RHI_shader_program_OpenGL>(shaders);
    }

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
    
};


};



