#pragma once
#include "engine/global/base.h" 
#include "engine/runtime/enum.h"
#include "engine/runtime/rhi/opengl/gl_cast.h"
#include "engine/runtime/rhi/rhi_cast.h"
#include "engine/runtime/rhi/rhi_geometry.h"

namespace rtr {

class RHI_geometry_OpenGL : public RHI_geometry {
protected:
    unsigned int m_vao{};

public:
    RHI_geometry_OpenGL(
        const std::unordered_map<unsigned int, Vertex_buffer_descriptor> &vertex_buffers, 
        const Element_buffer_descriptor& element_buffer
    ) : RHI_geometry(vertex_buffers, element_buffer) { 
        glGenVertexArrays(1, &m_vao);
        bind_buffers();
    }

    virtual ~RHI_geometry_OpenGL() override { 
        if (m_vao) {
            glDeleteVertexArrays(1, &m_vao);
        }
    }

    virtual void bind() override {
        glBindVertexArray(m_vao);
    }

    virtual void unbind() override {
        glBindVertexArray(0);
    }

    virtual void bind_vertex_buffer(Vertex_buffer_descriptor vbo, unsigned int location) override {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, vbo.unit_data_count, gl_atribute_type(vbo.attribute_type), GL_FALSE, vbo.unit_data_count * get_buffer_attribute_size(vbo.attribute_type), (void*)0);
        glVertexAttribDivisor(location, vbo.iterate_type == Buffer_iterate_type::PER_INSTANCE ? 1 : 0);
    }

    virtual void bind_buffers() override {
        bind();
        for (auto& [location, vbo] : m_vertex_buffers) {
            bind_vertex_buffer(vbo, location);
        }
        if (m_element_buffer.ebo) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_element_buffer.ebo);
        }
        unbind();
    }
 
    virtual void draw(Draw_mode mode = Draw_mode::TRIANGLES) override {
        if (m_element_buffer.ebo) {
            bind();
            glDrawElements(gl_draw_mode(mode), m_element_buffer.data_count, GL_UNSIGNED_INT, 0);
            unbind();
        }
    }

    virtual void instanced_draw(unsigned int instance_count, Draw_mode mode = Draw_mode::TRIANGLES) override {
        if (m_element_buffer.ebo) {
            bind();
            glDrawElementsInstanced(gl_draw_mode(mode), m_element_buffer.data_count, GL_UNSIGNED_INT, 0, instance_count);
            unbind();
        }
    }

    virtual unsigned int id() override {
        return m_vao;
    }

};

}
