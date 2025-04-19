#pragma once

#include "engine/runtime/global/base.h" 
#include "../rhi_buffer.h"
#include "rhi_cast_opengl.h"

#include <memory>


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

    void bind() { 
        glBindBuffer(gl_buffer_type(m_type), m_buffer_id); 
    }
    void unbind() { 
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

    ~RHI_vertex_buffer_OpenGL() override {}
    static std::shared_ptr<RHI_vertex_buffer_OpenGL> create(
        Buffer_usage usage,
        Buffer_data_type attribute_type,
        Buffer_iterate_type iterate_type,
        unsigned int unit_data_count,
        unsigned int data_size,
        const void* data
    ) {
        return std::make_shared<RHI_vertex_buffer_OpenGL>(
            usage,
            attribute_type,
            iterate_type,
            unit_data_count,
            data_size,
            data
        );
    }
};

class RHI_element_buffer_OpenGL : public RHI_buffer_OpenGL, public IRHI_element_buffer {
public:

    RHI_element_buffer_OpenGL(
        Buffer_usage usage,
        unsigned int data_count,
        unsigned int data_size,
        const void* data
    ) : RHI_buffer_OpenGL(Buffer_type::ELEMENT, usage, data_size, data),
        IRHI_element_buffer(data_count) {}

    ~RHI_element_buffer_OpenGL() override {}

    static std::shared_ptr<RHI_element_buffer_OpenGL> create(
        Buffer_usage usage,
        unsigned int data_count,
        unsigned int data_size,
        const void* data
    ) {
        return std::make_shared<RHI_element_buffer_OpenGL>(
            usage,
            data_count,
            data_size,
            data
        );
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
            glGetIntegerv(gl_memory_buffer_alignment_type(type), &m_alignment);
        }

    ~RHI_memory_buffer_OpenGL() override {}

    void bind_memory(unsigned int position) override {
        glBindBufferBase(gl_buffer_type(m_type), position, m_buffer_id);
    }

    void bind_partial_memory(unsigned int position, unsigned int offset, unsigned int size) override {
        glBindBufferRange(gl_buffer_type(m_type), position, m_buffer_id, offset, size);
    }

    static std::shared_ptr<RHI_memory_buffer_OpenGL> create(
        Buffer_type type,
        Buffer_usage usage,
        unsigned int data_size,
        const void* data
    ) {
        return std::make_shared<RHI_memory_buffer_OpenGL>(
            type,
            usage,
            data_size,
            data
        );
    }

};

class RHI_memory_binder_OpenGL : public RHI_memory_buffer_binder {
public:
    RHI_memory_binder_OpenGL() : RHI_memory_buffer_binder() {}
    virtual ~RHI_memory_binder_OpenGL() {}
    void bind_memory_buffer(
        const std::shared_ptr<RHI_buffer>& buffer, 
        unsigned int binding_point
    ) override {
        if (buffer->type() != Buffer_type::STORAGE && buffer->type()!= Buffer_type::UNIFORM) {
            std::cout << "buffer type is not storage or uniform" << std::endl;
            return;
        }

        if (auto memory_buffer = std::dynamic_pointer_cast<RHI_memory_buffer_OpenGL>(buffer)) {
            memory_buffer->bind_memory(binding_point);
        }

    }
   
    virtual void bind_memory_bufer_partial(
        const std::shared_ptr<RHI_buffer>& buffer, 
        unsigned int binding_point, 
        unsigned int offset, 
        unsigned int size
    ) override {
        if (buffer->type() != Buffer_type::STORAGE && buffer->type()!= Buffer_type::UNIFORM) {
            std::cout << "buffer type is not storage or uniform" << std::endl;
            return;
        }

        if (auto memory_buffer = std::dynamic_pointer_cast<RHI_memory_buffer_OpenGL>(buffer)) {
            memory_buffer->bind_partial_memory(binding_point, offset, size);
        }
    }

    static std::shared_ptr<RHI_memory_binder_OpenGL> create() {
        return std::make_shared<RHI_memory_binder_OpenGL>();
    }
};

};
