#pragma once

#include "engine/global/base.h" 
#include "engine/runtime/enum.h"
#include "engine/runtime/rhi/opengl/gl_cast.h"
#include "engine/runtime/rhi/rhi_buffer.h"


namespace rtr {


class RHI_buffer_OpenGL : public RHI_buffer {
    unsigned int m_buffer_id{0};
    
public:
    RHI_buffer_OpenGL(
        Buffer_type type,
        Buffer_usage usage,
        unsigned int data_size,
        const void* data
    ) : RHI_buffer(type, usage, data_size, data) {
        glGenBuffers(1, &m_buffer_id);
        glBindBuffer(gl_buffer_type(m_type), m_buffer_id);
        glBufferData(gl_buffer_type(m_type), m_data_size, data, gl_usage(m_usage));
    }

    ~RHI_buffer_OpenGL() override {
        if (m_buffer_id) {
            glDeleteBuffers(1, &m_buffer_id);
        }
    }

    void bind() override { 
        glBindBuffer(gl_buffer_type(m_type), m_buffer_id); 
    }
    void unbind() override { 
        glBindBuffer(gl_buffer_type(m_type), 0); 
    }

    unsigned int id() override { return m_buffer_id; }

    void reallocate_data(const void* data, unsigned int data_size) override {
        m_data_size = data_size;
        glBufferData(gl_buffer_type(m_type), data_size, data, gl_usage(m_usage));
    }

    void subsitute_data(const void* data, unsigned int data_size, unsigned int offset) override {
        glBufferSubData(gl_buffer_type(m_type), offset, data_size, data);
    }

    void access_gpu_buffer(std::function<void(void*)> accessor, GPU_access_flags flags) override {
        GLenum access = 0;
        if(flags.is_read) access |= GL_MAP_READ_BIT;
        if(flags.is_write) access |= GL_MAP_WRITE_BIT;
        if(flags.is_buffer_discard) access |= GL_MAP_INVALIDATE_BUFFER_BIT;

        bind();
        auto m_mapped_pointer = glMapBufferRange(gl_buffer_type(m_type), 0, m_data_size, access);
        if (m_mapped_pointer == nullptr) {
            std::cout << "[RHI] glMapBufferRange failed" << std::endl;
            return;
        }
        accessor(m_mapped_pointer);
        glUnmapBuffer(gl_buffer_type(m_type));
    }
};

};
