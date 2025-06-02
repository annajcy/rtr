#pragma once


#include "engine/runtime/platform/rhi/rhi_shader_program.h"
#include <memory>

namespace rtr {

// Global shading settings

struct Base_setting {
    virtual ~Base_setting() = default;
    virtual void modify_shader_uniform(
        const std::shared_ptr<RHI_shader_program>& shader_program
    ) = 0;
};


}