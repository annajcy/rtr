#pragma once
#include "engine/global/base.h"
#include "engine/runtime/enum.h"
#include "engine/runtime/rhi/opengl/rhi_error_opengl.h"
#include "engine/runtime/rhi/rhi_resource.h"
#include "engine/runtime/rhi/rhi_cast.h"
#include "engine/runtime/rhi/rhi_shader_code.h"
#include <functional>
#include <unordered_map>

namespace rtr {

class RHI_uniform_entry_base {
public:
    using Ptr = std::shared_ptr<RHI_uniform_entry_base>;
protected:
    Uniform_type m_type{};
    bool m_is_need_update{};
    bool m_is_external_reference{};
public:
    RHI_uniform_entry_base(Uniform_type type, bool is_external_reference) : 
    m_type(type), m_is_need_update(true) {}
    virtual ~RHI_uniform_entry_base() {}
    Uniform_type type() const { return m_type; }
    bool is_external_reference() const { return m_is_external_reference; }
    bool is_need_update() const { return m_is_need_update; }
    bool& is_need_update() { return m_is_need_update; }

    virtual const void* data_ptr() const = 0;
};

template<typename T>
class RHI_uniform_entry : public RHI_uniform_entry_base {
protected:
    T* m_data{};

public:
    using Ptr = std::shared_ptr<RHI_uniform_entry<T>>;

    ~RHI_uniform_entry() override {
        if (m_data != nullptr && !m_is_external_reference) {
            delete m_data;
        }
    }

    RHI_uniform_entry(T data) : RHI_uniform_entry_base(get_type<T>(), false), m_data(new T(data)) {}
    RHI_uniform_entry(T* data) : RHI_uniform_entry_base(get_type<T>(), true), m_data(data) {}
    const void* data_ptr() const override { return reinterpret_cast<const void*>(m_data); }
    T* data_ptr_typed() { return m_data; }

    const T& data() const {
        if (m_data == nullptr) {
            std::cout << "RHI_uniform_entry::data: m_data is nullptr" << std::endl;
            return *m_data;
        }
    }

    void modify(const T& data) {
        if (m_data == nullptr) {
            std::cout << "RHI_uniform_entry::modify: m_data is nullptr" << std::endl;
            return;
        }
        *m_data = data;
        m_is_need_update = true;
    }

    static Ptr create(T* data) {
        return std::make_shared<RHI_uniform_entry<T>>(data);
    }

    static Ptr create(const T& data) {
        return std::make_shared<RHI_uniform_entry<T>>(data);
    }

};
class RHI_shader_program : public RHI_resource { 
protected:
    std::unordered_map<Shader_type, RHI_shader_code::Ptr> m_codes{};
    std::unordered_map<std::string, RHI_uniform_entry_base::Ptr> m_uniforms{};

public:
    
    RHI_shader_program(
        const std::unordered_map<Shader_type, RHI_shader_code::Ptr>& shaders, 
        const std::unordered_map<std::string, RHI_uniform_entry_base::Ptr>& uniforms
    ) : RHI_resource(RHI_resource_type::SHADER_PROGRAM),
        m_codes(shaders), 
        m_uniforms(uniforms) {}

    using Ptr = std::shared_ptr<RHI_shader_program>;

    const std::unordered_map<Shader_type, RHI_shader_code::Ptr>& codes() const { return m_codes; }
    const std::unordered_map<std::string, RHI_uniform_entry_base::Ptr>& uniforms() const { return m_uniforms; }

    virtual ~RHI_shader_program() {}

    virtual void attach_code(const RHI_shader_code::Ptr& code) = 0;
    virtual void detach_code(const RHI_shader_code::Ptr& code) = 0;
    virtual bool link() = 0;
    virtual void set_uniform(const std::string& name, Uniform_type type, const void* data) = 0;
    virtual void set_uniform_array(const std::string& name, Uniform_type type, const void* data, unsigned int count) = 0;
    
    virtual void update_uniforms() = 0;

    template<typename T>
    void modify_uniform(const std::string& name, const T& data) {
        if (auto it = m_uniforms.find(name); it != m_uniforms.end()) {
            if (auto uniform = std::dynamic_pointer_cast<RHI_uniform_entry<T>>(it->second)) {
                uniform->modify(data);
            }
        } else {
            std::cout << "RHI_shader_program::modify_uniform: uniform " << name << " not found" << std::endl;
        }
    }

    template<typename T>
    T get_uniform(const std::string& name) {
        if (auto it = m_uniforms.find(name); it!= m_uniforms.end()) {
            if (auto uniform = std::dynamic_pointer_cast<RHI_uniform_entry<T>>(it->second)) {
                return uniform->data();
            }
        } else {
            std::cout << "RHI_shader_program::get_uniform: uniform " << name << " not found" << std::endl;
        }
    }

    template<typename T>
    RHI_uniform_entry<T>::Ptr get_uniform_entry(const std::string& name) {
        if (auto it = m_uniforms.find(name); it!= m_uniforms.end()) {
            if (auto uniform = std::dynamic_pointer_cast<RHI_uniform_entry<T>>(it->second)) {
                return uniform;
            }
        } else {
            std::cout << "RHI_shader_program::get_uniform_entry: uniform " << name << " not found" << std::endl;
        }
    }

    

};

};