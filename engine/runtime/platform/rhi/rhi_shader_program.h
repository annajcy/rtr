#pragma once

#include "engine/runtime/global/base.h" 

#include "rhi_cast.h"
#include "rhi_shader_code.h"
#include <memory>

namespace rtr {

enum class Uniform_entry_type {
    SINGLE,
    ARRAY
};

class Uniform_entry_base {
protected:
    Uniform_entry_type m_entry_type{};
    Uniform_data_type m_data_type{};
    bool m_is_need_update{};

public:
    Uniform_entry_base(
        Uniform_data_type data_type, 
        Uniform_entry_type entry_type
    ) : m_data_type(data_type), 
        m_entry_type(entry_type), 
        m_is_need_update(true) {}

    virtual ~Uniform_entry_base() {}
    Uniform_data_type data_type() const { return m_data_type; }
    Uniform_entry_type entry_type() const { return m_entry_type; }
    bool is_need_update() const { return m_is_need_update; }
    bool& is_need_update() { return m_is_need_update; }
    virtual const void* data_ptr() const = 0;
    virtual unsigned int data_count() const = 0;
};

template<typename T>
class Uniform_entry : public Uniform_entry_base {
protected:
    std::unique_ptr<T> m_data{};

public:
    using Ptr = std::shared_ptr<Uniform_entry<T>>;

    ~Uniform_entry() override {}

    Uniform_entry(const T& data) : Uniform_entry_base(get_uniform_data_type<T>(), Uniform_entry_type::SINGLE), m_data(std::make_unique<T>(data)) {}
    const void* data_ptr() const override { return reinterpret_cast<const void*>(m_data.get()); }
    const std::unique_ptr<T>& data_ptr_typed() const { return m_data; }

    const T data() const {
        if (m_data == nullptr) {
            std::cout << "RHI_uniform_entry::data: m_data is nullptr" << std::endl;
            return T{};
        }
        return *m_data;
    }

    void modify(const T& data) {
        if (m_data == nullptr) {
            std::cout << "RHI_uniform_entry::modify: m_data is nullptr" << std::endl;
            return;
        }
        *m_data = data;
        m_is_need_update = true;
    }

    unsigned int data_count() const override { return 1; }

    static Ptr create(const T& data) {
        return std::make_shared<Uniform_entry<T>>(data);
    }

};

template<typename T>
class Uniform_entry_array : public Uniform_entry_base {
protected:
    std::unique_ptr<T[]> m_data{};
    unsigned int m_count{};
public:
    ~Uniform_entry_array() override {}
    Uniform_entry_array(const T* data, unsigned int count) : Uniform_entry_base(get_uniform_data_type<T>(), Uniform_entry_type::ARRAY), m_count(count) {
        m_data = std::make_unique<T[]>(count);
        for (unsigned int i = 0; i < count; ++i) {
            m_data[i] = data[i];
        }
    }
    unsigned int data_count() const override { return m_count; }
    const void* data_ptr() const override { return reinterpret_cast<const void*>(m_data.get()); }
    const std::unique_ptr<T[]>& data_ptr_typed() const { return m_data; }
    const T data(unsigned int index) const {
        if (index >= m_count) {
            std::cout << "RHI_uniform_entry_array::data: index out of range" << std::endl;
        }
        return m_data[index];
    }

    void modify(const T& data, unsigned int index) {
        if (index >= m_count) {
            std::cout << "RHI_uniform_entry_array::modify: index out of range" << std::endl;
        } else {
            m_data[index] = data;
            m_is_need_update = true;
        }
    }

    void modify(const T* data, unsigned int count, unsigned int offset = 0) {
        if (offset + count > m_count) {
            std::cout << "RHI_uniform_entry_array::modify: index out of range" << std::endl;
        } else {
            for (unsigned int i = 0; i < count; ++i) {
                m_data[offset + i] = data[i];
            }
            m_is_need_update = true;
        }
    }

    static std::shared_ptr<Uniform_entry_array<T>> create(const T* data, unsigned int count) {
        return std::make_shared<Uniform_entry_array<T>>(data, count);
    }
};

class RHI_shader_program { 
protected:
    std::unordered_map<Shader_type, std::shared_ptr<RHI_shader_code>> m_codes{};
    std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> m_uniforms{};

public:
    
    RHI_shader_program(
        const std::unordered_map<Shader_type, std::shared_ptr<RHI_shader_code>> & shaders, 
        const std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>& uniforms
    ) : m_codes(shaders), 
        m_uniforms(uniforms) {}

    const std::unordered_map<Shader_type, std::shared_ptr<RHI_shader_code>>& codes() const { return m_codes; }
    const std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>& uniforms() const { return m_uniforms; }

    virtual ~RHI_shader_program() {}

    virtual void attach_code(const std::shared_ptr<RHI_shader_code>& code) = 0;
    virtual void detach_code(const std::shared_ptr<RHI_shader_code>& code) = 0;
    virtual bool link() = 0;
    virtual void set_uniform(const std::string& name, Uniform_data_type type, const void* data) = 0;
    virtual void set_uniform_array(const std::string& name, Uniform_data_type type, const void* data, unsigned int count) = 0;
    
    virtual void update_uniforms() = 0;

    template<typename T>
    void modify_uniform(const std::string& name, const T& data) {
        if (auto it = m_uniforms.find(name); it != m_uniforms.end()) {
            if (auto uniform = std::dynamic_pointer_cast<Uniform_entry<T>>(it->second)) {
                uniform->modify(data);
            }
        } else {
            std::cout << "RHI_shader_program::modify_uniform: uniform " << name << " not found" << std::endl;
        }
    }

    template<typename T>
    void modify_uniform_array(const std::string& name, const T* data, unsigned int count, unsigned int offset = 0) {
        if (auto it = m_uniforms.find(name); it!= m_uniforms.end()) {
            if (auto uniform = std::dynamic_pointer_cast<Uniform_entry_array<T>>(it->second)) {
                uniform->modify(data, count, offset);
            }
        } else {
            std::cout << "RHI_shader_program::modify_uniform_array: uniform " << name << " not found" << std::endl;
        }
    }

    template<typename T>
    T get_uniform(const std::string& name) {
        if (auto it = m_uniforms.find(name); it!= m_uniforms.end()) {
            if (auto uniform = std::dynamic_pointer_cast<Uniform_entry<T>>(it->second)) {
                return uniform->data();
            }
        } else {
            std::cout << "RHI_shader_program::get_uniform: uniform " << name << " not found" << std::endl;
        }
    }


    template<typename T>
    T get_uniform_array(const std::string& name, unsigned int index) {
        if (auto it = m_uniforms.find(name); it!= m_uniforms.end()) {
            if (auto uniform = std::dynamic_pointer_cast<Uniform_entry_array<T>>(it->second)) {
                return uniform->data(index);
            }
        }
    }

    template<typename T>
    Uniform_entry_array<T>::Ptr get_uniform_entry_array(const std::string& name) {
        if (auto it = m_uniforms.find(name); it!= m_uniforms.end()) {
            if (auto uniform = std::dynamic_pointer_cast<Uniform_entry_array<T>>(it->second)) {
                return uniform;
            }
        } else {
            std::cout << "RHI_shader_program::get_uniform_entry_array: uniform " << name << " not found" << std::endl;
        }
    }

    template<typename T>
    Uniform_entry<T>::Ptr get_uniform_entry(const std::string& name) {
        if (auto it = m_uniforms.find(name); it!= m_uniforms.end()) {
            if (auto uniform = std::dynamic_pointer_cast<Uniform_entry<T>>(it->second)) {
                return uniform;
            }
        } else {
            std::cout << "RHI_shader_program::get_uniform_entry: uniform " << name << " not found" << std::endl;
        }
    }

    

};

};