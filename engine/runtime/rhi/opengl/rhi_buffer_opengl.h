#pragma once

#include "engine/global/base.h" 
#include "engine/runtime/enum.h"
#include "engine/runtime/rhi/opengl/rhi_cast_opengl.h"
#include "engine/runtime/rhi/rhi_buffer.h"


namespace rtr {

class RHI_buffer_OpenGL : public RHI_buffer {
protected:
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

    unsigned int buffer_id() const { return m_buffer_id; }

    void bind() override { 
        glBindBuffer(gl_buffer_type(m_type), m_buffer_id); 
    }
    void unbind() override { 
        glBindBuffer(gl_buffer_type(m_type), 0); 
    }

    void reallocate_data(const void* data, unsigned int data_size) override {
        m_data_size = data_size;
        glBufferData(gl_buffer_type(m_type), data_size, data, gl_usage(m_usage));
    }

    void subsitute_data(const void* data, unsigned int data_size, unsigned int offset) override {
        glBufferSubData(gl_buffer_type(m_type), offset, data_size, data);
    }

    void map_buffer(std::function<void(void*)> access_function, const RHI_buffer_access_flags& flags) override {
        GLenum access = 0;
        if(flags.is_read) access |= GL_MAP_READ_BIT;
        if(flags.is_write) access |= GL_MAP_WRITE_BIT;
        if(flags.is_buffer_discard) access |= GL_MAP_INVALIDATE_BUFFER_BIT;

        bind();
        auto m_mapped_pointer = glMapBufferRange(gl_buffer_type(m_type), 0, m_data_size, access);
        if (m_mapped_pointer == nullptr) {
            std::cout << "glMapBufferRange failed" << std::endl;
            return;
        }
        access_function(m_mapped_pointer);
        glUnmapBuffer(gl_buffer_type(m_type));
        unbind();
    }
};

class RHI_vertex_buffer_OpenGL : public RHI_buffer_OpenGL, public IRHI_vertex_buffer {
public:
    RHI_vertex_buffer_OpenGL(
        Buffer_usage usage,
        Buffer_data_type attribute_type,
        Buffer_iterate_type iterate_type,
        unsigned int unit_data_count,
        unsigned int data_size,
        const void* data
    ) : RHI_buffer_OpenGL(Buffer_type::VERTEX, usage, data_size, data),
        IRHI_vertex_buffer(attribute_type, iterate_type, unit_data_count) {}

    ~RHI_vertex_buffer_OpenGL() override {
        RHI_buffer_OpenGL::~RHI_buffer_OpenGL();
    }
};

class RHI_element_buffer_OpenGL : public RHI_buffer_OpenGL, public IRHI_element_buffer {
public:
    RHI_element_buffer_OpenGL(
        Buffer_usage usage,
        unsigned int data_size,
        const void* data
    ) : RHI_buffer_OpenGL(Buffer_type::ELEMENT, usage, data_size, data),
        IRHI_element_buffer(data_size) {}

    ~RHI_element_buffer_OpenGL() override {
        RHI_buffer_OpenGL::~RHI_buffer_OpenGL();
    }
};

class RHI_memory_buffer_OpenGL : public RHI_buffer_OpenGL, public IRHI_memory_buffer {
public:

    RHI_memory_buffer_OpenGL(
        Buffer_type type,
        Buffer_usage usage,
        unsigned int data_size,
        const void* data
    ) : RHI_buffer_OpenGL(type, usage, data_size, data),
        IRHI_memory_buffer() {
            glGetIntegerv(gl_memory_buffer_alignment_type(Buffer_type::UNIFORM), &m_alignment);
        }

    ~RHI_memory_buffer_OpenGL() override {
        RHI_buffer_OpenGL::~RHI_buffer_OpenGL();
    }

    void bind_memory(unsigned int position) override {
        glBindBufferBase(gl_buffer_type(m_type), position, m_buffer_id);
    }

    void bind_partial_memory(unsigned int position, unsigned int offset, unsigned int size) override {
        glBindBufferRange(gl_buffer_type(m_type), position, m_buffer_id, offset, size);
    }

};

};
