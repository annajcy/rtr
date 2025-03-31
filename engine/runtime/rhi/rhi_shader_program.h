#pragma once
#include "engine/global/base.h"
#include "engine/runtime/enum.h"
#include "engine/runtime/rhi/opengl/rhi_error_opengl.h"
#include "engine/runtime/rhi/rhi_resource.h"
#include "engine/runtime/rhi/rhi_shader_code.h"
#include <unordered_map>

namespace rtr {

struct RHI_uniform_entry {
    Uniform_type type{};
    const void* data{};
    bool need_update{};

    RHI_uniform_entry() {}
    RHI_uniform_entry(Uniform_type type, const void* data) : 
    type(type), data(data), need_update(true) {}
};

struct RHI_uniform_array_entry {
    Uniform_type type{};
    const void* data{};
    unsigned int count{};
    bool need_update{};

    RHI_uniform_array_entry() {}
    RHI_uniform_array_entry(Uniform_type type, const void* data, unsigned int count) :
    type(type), data(data), count(count), need_update(true) {}
};

class RHI_shader_program : public RHI_resource { 
protected:
    std::unordered_map<Shader_type, RHI_shader_code::Ptr> m_codes{};
    std::unordered_map<std::string, RHI_uniform_entry> m_uniforms{};
    std::unordered_map<std::string, RHI_uniform_array_entry> m_uniform_arrays{};

public:
    
    RHI_shader_program(
        const std::unordered_map<Shader_type, RHI_shader_code::Ptr>& shaders, 
        const std::unordered_map<std::string, RHI_uniform_entry>& uniforms, 
        const std::unordered_map<std::string, RHI_uniform_array_entry>& uniform_arrays) : 
        RHI_resource(RHI_resource_type::SHADER_PROGRAM),
        m_codes(shaders), 
        m_uniforms(uniforms), 
        m_uniform_arrays(uniform_arrays) {}

    using Ptr = std::shared_ptr<RHI_shader_program>;

    virtual ~RHI_shader_program() {}

    virtual void bind() = 0;
    virtual void unbind() = 0;
    virtual void attach_code(const RHI_shader_code::Ptr& code) = 0;
    virtual void detach_code(const RHI_shader_code::Ptr& code) = 0;
    virtual bool link() = 0;
    virtual void set_uniform(const std::string& name, Uniform_type type, const void* data) = 0;
    virtual void set_uniform_array(const std::string& name, Uniform_type type, const void* data, unsigned int count) = 0;

    const std::unordered_map<std::string, RHI_uniform_entry>& uniforms() const { return m_uniforms; }
    const std::unordered_map<std::string, RHI_uniform_array_entry>& uniform_arrays() const { return m_uniform_arrays; }
    
    void update_uniforms() {  
        bind();
        for (auto& [name, uniform] : m_uniforms) {
            if (uniform.need_update) {
                set_uniform(name, uniform.type, uniform.data);
                uniform.need_update = false;
            }  
        }

        for (auto& [name, uniform_array] : m_uniform_arrays) {
            if (uniform_array.need_update) {
                set_uniform_array(name, uniform_array.type, uniform_array.data, uniform_array.count);
                uniform_array.need_update = false;
            }  
        }
        unbind();
    }

    void modify_uniform(const std::string& name, Uniform_type type, const void* data) {
        if (m_uniforms.find(name) != m_uniforms.end()) {
            m_uniforms[name].data = data;
            m_uniforms[name].type = type;
            m_uniforms[name].need_update = true;
        } else {
            std::cerr << "Uniform " << name << " not found." << std::endl;
        }
    }

};

};