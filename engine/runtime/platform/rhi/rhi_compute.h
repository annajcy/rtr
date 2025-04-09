#pragma once

#include "engine/runtime/global/base.h" 
#include "rhi_buffer.h"
#include "rhi_resource.h"
#include "rhi_shader_code.h"
#include "rhi_shader_program.h"


namespace rtr {

class RHI_compute_task : public RHI_resource {
protected:
    RHI_shader_program::Ptr m_shader_program{};
public:
    using Ptr = std::shared_ptr<RHI_compute_task>;
    RHI_compute_task(const RHI_shader_program::Ptr& shader_program) : 
    RHI_resource(RHI_resource_type::COMPUTE_TASK),
    m_shader_program(shader_program) {
        if (!m_shader_program->codes().contains(Shader_type::COMPUTE)) {
            std::cout << "compute shader not found" << std::endl;
        }
    }

    virtual ~RHI_compute_task() {}
    virtual void dispatch(unsigned int x, unsigned int y, unsigned int z) = 0;
    virtual void wait() = 0;
};

};