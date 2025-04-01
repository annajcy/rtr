#pragma once
#include "engine/global/base.h"
#include "engine/runtime/rhi/opengl/rhi_frame_buffer_opengl.h"
#include "engine/runtime/rhi/opengl/rhi_geometry_opengl.h"
#include "engine/runtime/rhi/opengl/rhi_shader_program_opengl.h"
#include "engine/runtime/rhi/rhi_renderer.h"
#include "engine/runtime/rhi/rhi_resource.h"

namespace rtr {
class RHI_renderer_OpenGL : public RHI_renderer {

public:
    RHI_renderer_OpenGL(
        const RHI_shader_program::Ptr& shader_program,
        const RHI_geometry::Ptr& geometry,
        const RHI_frame_buffer::Ptr& frame_buffer
    ) : RHI_renderer(
        shader_program,
        geometry,
        frame_buffer
    ) {}
    ~RHI_renderer_OpenGL() override {}
    void draw() override {
        if (m_frame_buffer) {
            if (auto gl_frame_buffer = std::dynamic_pointer_cast<RHI_frame_buffer_OpenGL>(m_frame_buffer)) {
                gl_frame_buffer->bind();
            }
        }

        if (m_shader_program) {
            if (auto gl_shader_program = std::dynamic_pointer_cast<RHI_shader_program_OpenGL>(m_shader_program)) {
                gl_shader_program->bind();
            }
        }

        if (m_geometry) {
            if (auto gl_geometry = std::dynamic_pointer_cast<RHI_geometry_OpenGL>(m_geometry)) {
                gl_geometry->bind();
                gl_geometry->draw();
                gl_geometry->unbind();
            }
        }

        if (m_shader_program) {
            if (auto gl_shader_program = std::dynamic_pointer_cast<RHI_shader_program_OpenGL>(m_shader_program)) {
                gl_shader_program->unbind();
            }
        }

        if (m_frame_buffer) {
            if (auto gl_frame_buffer = std::dynamic_pointer_cast<RHI_frame_buffer_OpenGL>(m_frame_buffer)) {
                gl_frame_buffer->unbind();
            }
        }
    }

    void draw_instanced(unsigned int instance_count) override {
        if (m_frame_buffer) {
            if (auto gl_frame_buffer = std::dynamic_pointer_cast<RHI_frame_buffer_OpenGL>(m_frame_buffer)) {
                gl_frame_buffer->bind();
            }
        }

        if (m_shader_program) {
            if (auto gl_shader_program = std::dynamic_pointer_cast<RHI_shader_program_OpenGL>(m_shader_program)) {
                gl_shader_program->bind();
            }
        }

        if (m_geometry) {
            if (auto gl_geometry = std::dynamic_pointer_cast<RHI_geometry_OpenGL>(m_geometry)) {
                gl_geometry->bind();
                gl_geometry->draw_instanced(instance_count);
                gl_geometry->unbind();
            }
        }

        if (m_shader_program) {
            if (auto gl_shader_program = std::dynamic_pointer_cast<RHI_shader_program_OpenGL>(m_shader_program)) {
                gl_shader_program->unbind();
            }
        }

        if (m_frame_buffer) {
            if (auto gl_frame_buffer = std::dynamic_pointer_cast<RHI_frame_buffer_OpenGL>(m_frame_buffer)) {
                gl_frame_buffer->unbind();
            }
        }
        
    }
};
};