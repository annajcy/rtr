#pragma once

#include "engine/global/base.h" 
#include "engine/runtime/rhi/device/rhi_device.h"

#include "engine/runtime/rhi/buffer/rhi_buffer_opengl.h"
#include "engine/runtime/rhi/geometry/rhi_geometry_opengl.h"

namespace rtr {
    

class RHI_device_OpenGL : public RHI_device {
public:
    RHI_device_OpenGL() : RHI_device(API_type::OPENGL) { init(); }
    virtual ~RHI_device_OpenGL() override { destroy(); }

    virtual void init() override {

    }

    virtual void destroy() override {
        
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



