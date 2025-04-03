#pragma once

#include "engine/runtime/enum.h"
#include "engine/runtime/rhi/opengl/rhi_buffer_opengl.h"
#include "engine/runtime/rhi/opengl/rhi_shader_program_opengl.h"
#include "engine/runtime/rhi/rhi_compute.h"
#include "engine/runtime/rhi/rhi_shader_program.h"
#include <memory>

namespace rtr {

class RHI_compute_task_OpenGL : public RHI_compute_task {
public:
    using Ptr = std::shared_ptr<RHI_compute_task_OpenGL>;
    RHI_compute_task_OpenGL(
        const RHI_shader_program::Ptr& shader_program
    ) : RHI_compute_task(shader_program) {}
    ~RHI_compute_task_OpenGL() override {}
    void dispatch(unsigned int x, unsigned int y, unsigned int z) override {
        if (auto gl_shader_program = std::dynamic_pointer_cast<RHI_shader_program_OpenGL>(m_shader_program)) {
            gl_shader_program->bind();
            glDispatchCompute(x, y, z);
            gl_shader_program->unbind();
        }
    }

    void wait() override {
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    void bind_memory(const RHI_buffer::Ptr& buffer, unsigned int binding_point) override {
        if (auto gl_memory_buffer = std::dynamic_pointer_cast<RHI_memory_buffer_OpenGL>(buffer)) {
            if (gl_memory_buffer->type() != Buffer_type::STORAGE) {
                std::cout << "not a SSBO" << std::endl;
            } 
            gl_memory_buffer->bind_memory(binding_point);
        }
        
    }

    void bind_partial_memory(const RHI_buffer::Ptr& buffer, unsigned int binding_point, unsigned int offset, unsigned int size) override {
        if (auto gl_memory_buffer = std::dynamic_pointer_cast<RHI_memory_buffer_OpenGL>(buffer)) {
            if (gl_memory_buffer->type() != Buffer_type::STORAGE) {
                std::cout << "not a SSBO" << std::endl;
            } 
            gl_memory_buffer->bind_partial_memory(binding_point, offset, size);
        }
    }

    


};

};