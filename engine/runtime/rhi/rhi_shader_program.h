#pragma once
#include "engine/global/base.h"
#include "engine/runtime/enum.h"
#include "engine/runtime/rhi/rhi_resource.h"
#include <unordered_map>

namespace rtr {

struct RHI_uniform_entry {
    Uniform_type type;
    const void* data;
};

struct RHI_uniform_array_entry {
    Uniform_type type;
    const void* data;
    unsigned int count;
};


class RHI_shader_program : public RHI_resource { 
protected:
    std::unordered_map<Shader_type, unsigned int> m_codes{};
    std::unordered_map<std::string, RHI_uniform_entry> m_uniforms{};
    std::unordered_map<std::string, RHI_uniform_array_entry> m_uniform_arrays{};

public:
    
    RHI_shader_program(
        const std::unordered_map<Shader_type, unsigned int>& shaders, 
        const std::unordered_map<std::string, RHI_uniform_entry>& uniforms, 
        const std::unordered_map<std::string, RHI_uniform_array_entry>& uniform_arrays) : 
        RHI_resource(RHI_resource_type::SHADER_PROGRAM),
        m_codes(shaders), 
        m_uniforms(uniforms), 
        m_uniform_arrays(uniform_arrays) {
            RHI_resource_manager::add_resource(this);
            for (auto& [type, code] : shaders) {
                RHI_resource_manager::add_dependency(guid(), code);
            }
        }

    virtual ~RHI_shader_program() {
        RHI_resource_manager::remove_resource(guid());
    }

    virtual void bind() = 0;
    virtual void unbind() = 0;
    virtual void attach_code(unsigned int code) = 0;
    virtual void detach_code(unsigned int code) = 0;
    virtual bool link() = 0;
    virtual void set_uniform(const std::string& name, Uniform_type type, const void* data) = 0;
    virtual void set_uniform_array(const std::string& name, Uniform_type type, const void* data, unsigned int count) = 0;

    const std::unordered_map<Shader_type, unsigned int>& codes() const { return m_codes; }
    const unsigned int& code(Shader_type type) const { return m_codes.at(type); }

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