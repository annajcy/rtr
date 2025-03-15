#pragma once
#include "engine/global/base.h"
#include "engine/runtime/rhi/shader/rhi_shader_code.h"

namespace rtr {

enum class Uniform_type {
    FLOAT, INT, BOOL,
    VEC2, VEC3, VEC4,
    IVEC2, IVEC3, IVEC4,
    MAT2, MAT3, MAT4,
    SAMPLER_2D, SAMPLER_CUBE,
    UNKNOWN,
};

class RHI_shader_program { 
protected:
    std::vector<std::shared_ptr<RHI_shader_code>> m_shaders;

public:
    RHI_shader_program(const std::vector<std::shared_ptr<RHI_shader_code>>& shaders) : m_shaders(shaders) { }
    virtual ~RHI_shader_program() = default;

    virtual void init() = 0;
    virtual void destroy() = 0;
    virtual void bind() = 0;
    virtual void unbind() = 0;
    virtual void attach_shader_code(const std::shared_ptr<RHI_shader_code>& shader) = 0;
    virtual void detach_shader_code(const std::shared_ptr<RHI_shader_code>& shader) = 0;
    virtual bool link() = 0;
    virtual void set_uniform(const std::string& name, Uniform_type type, const void* data) = 0;
    virtual void set_uniform_array(const std::string& name, Uniform_type type, const void* data, unsigned int count) = 0;
    
};

};