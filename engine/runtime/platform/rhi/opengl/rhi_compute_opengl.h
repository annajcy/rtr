#pragma once

#include "engine/runtime/global/base.h" 

#include "../rhi_compute.h"
#include "../rhi_shader_program.h"
#include "rhi_buffer_opengl.h"
#include "rhi_shader_program_opengl.h"


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
    


};

};