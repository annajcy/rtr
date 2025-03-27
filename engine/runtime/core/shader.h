#pragma once
#include "engine/global/base.h"
#include "texture.h"


namespace rtr {

template< typename T>
inline constexpr Uniform_type get_uniform_type() {
    if constexpr (std::is_same_v<T, int>) {
        return Uniform_type::INT;
    } else if constexpr (std::is_same_v<T, float>) {
        return Uniform_type::FLOAT;
    } else if constexpr (std::is_same_v<T, glm::vec2>) {
        return Uniform_type::VEC2;
    } else if constexpr (std::is_same_v<T, glm::vec3>) {
        return Uniform_type::VEC3;
    } else if constexpr (std::is_same_v<T, glm::vec4>) {
        return Uniform_type::VEC4;
    } else if constexpr (std::is_same_v<T, glm::mat2>) {
        return Uniform_type::MAT2;
    } else if constexpr (std::is_same_v<T, glm::mat3>) {
        return Uniform_type::MAT3;
    } else if constexpr (std::is_same_v<T, glm::mat4>) {
        return Uniform_type::MAT4;
    } else if constexpr (std::is_same_v<T, Texture>) {
        return Uniform_type::SAMPLER;
    } else {
        return Uniform_type::UNKNOWN;
    }
}

class Uniform_entry_base {
protected:
    Uniform_type m_type{};

public:
    Uniform_entry_base(Uniform_type type) : m_type(type) {}
    virtual ~Uniform_entry_base() = default;
    virtual const void* data_ptr() const = 0;
    Uniform_type type() const { return m_type; }
};


template <typename T>
class Uniform_entry : public Uniform_entry_base {
protected:
    T m_data{};
public:
    Uniform_entry(const T& data) : 
    Uniform_entry_base(get_uniform_type<T>()), 
    m_data(data) {}

    T& data() { return m_data; }
    const void* data_ptr() const override { return static_cast<const void *>(&m_data); }
};

class Uniform_array_entry_base {
protected:
    Uniform_type m_type{};

public:
    Uniform_array_entry_base(Uniform_type type, unsigned int count) : m_type(type) {}
    virtual ~Uniform_array_entry_base() = default;
    virtual const void* data_ptr() const = 0;
    virtual unsigned int count() const = 0;
    Uniform_type type() const { return m_type; }
};

template <typename T>
class Uniform_array_entry : public Uniform_array_entry_base {
protected:
    std::vector<T> m_data{};
public:
    Uniform_array_entry(const std::vector<T>& data) :
    Uniform_array_entry_base(get_uniform_type<T>(), data.size()),
    m_data(data) {}

    std::vector<T>& data() { return m_data; }
    const void* data_ptr() const override { return static_cast<const void *>(m_data.data()); }
    unsigned int count() const override { return m_data.size(); }

};

class Shader_code : public GUID {
protected:
    Shader_type m_type{};
    std::string m_code{};
public:
    Shader_code(Shader_type type, const std::string& code) : GUID(), m_type(type), m_code(code) {}
    virtual ~Shader_code() = default;
    std::string& code() { return m_code; }
    Shader_type type() const { return m_type; }
};


class Shader : public GUID {
protected:
    std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> m_shader_code_map{};
    std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> m_uniform_map{};
    std::unordered_map<std::string, std::shared_ptr<Uniform_array_entry_base>> m_uniform_array_map{};

public:
    Shader(
        const std::unordered_map<Shader_type, std::shared_ptr<Shader_code>>& shader_code_map 
    ) : GUID(), m_shader_code_map(shader_code_map) {}

    virtual ~Shader() = default;

    std::unordered_map<Shader_type, std::shared_ptr<Shader_code>>& shader_code_map() { return m_shader_code_map; }
    std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>& uniform_map () { return m_uniform_map; }
    std::unordered_map<std::string, std::shared_ptr<Uniform_array_entry_base>>& uniform_array_map() { return m_uniform_array_map; }
    void add_uniform(const std::string& name, std::shared_ptr<Uniform_entry_base> entry) { m_uniform_map[name] = entry; }
    void add_uniform_array(const std::string& name, std::shared_ptr<Uniform_array_entry_base> entry) { m_uniform_array_map[name] = entry; }
};


};