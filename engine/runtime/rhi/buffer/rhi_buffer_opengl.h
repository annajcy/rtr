#pragma once

#include "engine/global/base.h" 
#include "engine/runtime/rhi/buffer/rhi_buffer.h"

namespace rtr {

inline constexpr unsigned int gl_usage(Buffer_usage usage) {
    switch (usage) {
    case Buffer_usage::STATIC:
        return GL_STATIC_DRAW;
    case Buffer_usage::DYNAMIC:
        return GL_DYNAMIC_DRAW;
    default:
        return GL_STATIC_DRAW;
    }
}

class RHI_vertex_buffer_OpenGL : public RHI_vertex_buffer {
protected:
    unsigned int m_vbo{};
    bool m_is_data_loaded{false};

public:

    RHI_vertex_buffer_OpenGL(
        Buffer_usage usage,
        Buffer_attribute_type attribute_type,
        Buffer_iterate_type iterate_type,
        unsigned int unit_data_count,
        unsigned int data_count,
        const void* data
    ) : RHI_vertex_buffer(usage, 
        attribute_type, 
        iterate_type ,
        unit_data_count, 
        data_count, 
        data) {
        init();
    }

    virtual ~RHI_vertex_buffer_OpenGL() override { destroy(); }
    virtual void init() override {
        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        if (this->data() == nullptr) {
            glBufferData(GL_ARRAY_BUFFER, this->size(), nullptr, gl_usage(this->usage()));
            m_is_data_loaded = false;
            std::cout << "WARNING::RHI_VERTEX_BUFFER::DATA_IS_NULL: " << m_vbo << std::endl;
        } else {
            glBufferData(GL_ARRAY_BUFFER, this->size(), this->data(), gl_usage(this->usage()));
            m_is_data_loaded = true;
            std::cout << "INFO::RHI_VERTEX_BUFFER::DATA_IS_NOT_NULL: " << m_vbo << std::endl;
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    virtual void bind() override {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    }

    virtual void unbind() override {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    virtual void destroy() override {
        if (m_vbo) {
            glDeleteBuffers(1, &m_vbo);
            m_vbo = 0;
        }
    }

    virtual void update_buffer() override {
        if (this->data() == nullptr) {
            if (m_is_data_loaded) {
                glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
                glBufferData(GL_ARRAY_BUFFER, this->size(), nullptr, gl_usage(this->usage()));
            }
            m_is_data_loaded = false;
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, this->size(), this->data());
            m_is_data_loaded = true;
        }
    }

    unsigned int vbo() const { return m_vbo; }
};


class RHI_element_buffer_OpenGL : public RHI_element_buffer {
protected:
    unsigned int m_ebo{};
    bool m_is_data_loaded{false};
public:
    RHI_element_buffer_OpenGL(
        Buffer_usage usage,
        unsigned int data_count,
        const void* data
    ) : RHI_element_buffer(
        usage, 
        data_count, 
        data) {
        init();
    }

    virtual ~RHI_element_buffer_OpenGL() override { destroy(); }

    virtual void init() override {
        glGenBuffers(1, &m_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        if (this->data() == nullptr) {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->size(), nullptr, gl_usage(this->usage()));
            m_is_data_loaded = false;
            std::cout << "WARNING::RHI_ELEMENT_BUFFER::DATA_IS_NULL: " << m_ebo << std::endl;
        } else {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->size(), this->data(), gl_usage(this->usage()));
            m_is_data_loaded = true;
            std::cout << "INFO::RHI_ELEMENT_BUFFER::DATA_IS_NOT_NULL: " << m_ebo << std::endl;
        }
    }

    virtual void bind() override {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    }

    virtual void unbind() override {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    virtual void destroy() override {
        if (m_ebo) {
            glDeleteBuffers(1, &m_ebo);
            m_ebo = 0;
        }
    }

    virtual void update_buffer() override {
        if (this->data() == nullptr) {
            if (m_is_data_loaded) {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->size(), nullptr, gl_usage(this->usage()));
            }
            m_is_data_loaded = false;
        }
        else {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, this->size(), this->data());
            m_is_data_loaded = true;
        }
    }

    unsigned int ebo() const { return m_ebo; }
};

};
