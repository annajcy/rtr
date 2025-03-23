#pragma once
#include "engine/global/base.h"
#include "engine/runtime/rhi/shader/rhi_shader_code.h"

namespace rtr {

enum class Uniform_type {
    FLOAT, INT, BOOL,
    VEC2, VEC3, VEC4,
    IVEC2, IVEC3, IVEC4,
    MAT2, MAT3, MAT4,
    SAMPLER,
    UNKNOWN,
};

struct RHI_uniform_entry {
    Uniform_type type;
    const void* data;
};

struct RHI_uniform_array_entry {
    Uniform_type type;
    const void* data;
    unsigned int count;
};


class RHI_shader_program { 
protected:
    std::unordered_map<Shader_type, std::shared_ptr<RHI_shader_code>> m_shaders{};

    std::unordered_map<std::string, RHI_uniform_entry> m_uniforms{};
    std::unordered_map<std::string, RHI_uniform_array_entry> m_uniform_arrays{};

public:
    RHI_shader_program(const std::unordered_map<Shader_type, std::shared_ptr<RHI_shader_code>>& shaders) : m_shaders(shaders) { }
    RHI_shader_program(
        const std::unordered_map<Shader_type, std::shared_ptr<RHI_shader_code>>& shaders, 
        const std::unordered_map<std::string, RHI_uniform_entry>& uniforms, 
        const std::unordered_map<std::string, RHI_uniform_array_entry>& uniform_arrays) : 
        m_shaders(shaders), 
        m_uniforms(uniforms), 
        m_uniform_arrays(uniform_arrays) {}
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

    const std::unordered_map<Shader_type, std::shared_ptr<RHI_shader_code>>& shaders() const { return m_shaders; }
    const std::shared_ptr<RHI_shader_code>& shader(Shader_type type) const { return m_shaders.at(type); }
    bool has_shader(Shader_type type) const { return m_shaders.find(type) != m_shaders.end(); }

    void set_uniforms(const std::unordered_map<std::string, RHI_uniform_entry>& uniforms) { m_uniforms = uniforms; }
    void set_uniform_arrays(const std::unordered_map<std::string, RHI_uniform_array_entry>& uniform_arrays) { m_uniform_arrays = uniform_arrays; }

    const std::unordered_map<std::string, RHI_uniform_entry>& uniforms() const { return m_uniforms; }
    const std::unordered_map<std::string, RHI_uniform_array_entry>& uniform_arrays() const { return m_uniform_arrays; }
    bool has_uniform(const std::string& name) const { return m_uniforms.find(name) != m_uniforms.end(); }
    bool has_uniform_array(const std::string& name) const { return m_uniform_arrays.find(name)!= m_uniform_arrays.end(); }

    RHI_uniform_entry& uniform(const std::string& name) { return m_uniforms.at(name); }
    RHI_uniform_array_entry& uniform_array(const std::string& name) { return m_uniform_arrays.at(name); }
    
    void update_uniforms() {  
        for (auto& [name, uniform] : m_uniforms) {
            set_uniform(name, uniform.type, uniform.data);
        }

        for (auto& [name, uniform_array] : m_uniform_arrays) {
            set_uniform_array(name, uniform_array.type, uniform_array.data, uniform_array.count);
        }
    }
    
};

};