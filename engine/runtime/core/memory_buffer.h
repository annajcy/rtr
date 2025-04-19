#pragma once


#include "engine/runtime/global/enum.h"
#include "engine/runtime/global/guid.h"
#include "engine/runtime/platform/rhi/rhi_buffer.h"
#include "engine/runtime/platform/rhi/rhi_device.h"
#include "engine/runtime/platform/rhi/rhi_linker.h"
#include <memory>
#include <vector>

namespace rtr {

class Memory_buffer : public GUID, public RHI_linker<RHI_buffer> {
protected:
    Buffer_type m_type{};
    Buffer_usage m_usage{};

public:
    Memory_buffer(Buffer_type type, Buffer_usage usage) : m_type(type), m_usage(usage) {}
    ~Memory_buffer() = default;
    Buffer_type get_type() const { return m_type; }
    Buffer_usage get_usage() const { return m_usage; }

};

template<typename T>
class Uniform_buffer : public Memory_buffer {
protected:
    T m_data{};

public:
    Uniform_buffer(const T& data) : 
    Memory_buffer(Buffer_type::UNIFORM, Buffer_usage::STATIC), 
    m_data(data) {}
    ~Uniform_buffer() = default;

    T data() const { return m_data; }
    void set_data(const T& data) { m_data = data; }

    void link(const std::shared_ptr<RHI_device>& device) {
        m_rhi_resource = device->create_memory_buffer(
            m_type,  
            m_usage, 
            sizeof(T), 
            &m_data
        );
    }

    static std::shared_ptr<Uniform_buffer<T>> create(const T& data) {
        return std::make_shared<Uniform_buffer<T>>(data);
    }
};

template<typename T>
class Uniform_buffer_array : public Memory_buffer {
protected:
    std::vector<T> m_data{};

public:
    Uniform_buffer_array(const std::vector<T>& data) : 
    Memory_buffer(Buffer_type::UNIFORM, Buffer_usage::STATIC),
    m_data(data) {}

    ~Uniform_buffer_array() = default;

    void link(const std::shared_ptr<RHI_device>& device) {
        m_rhi_resource = device->create_memory_buffer(
            m_type,  
            m_usage, 
            sizeof(T) * m_data.size(), 
            m_data.data()
        );
    }

    static std::shared_ptr<Uniform_buffer_array<T>> create(const std::vector<T>& data) {
        return std::make_shared<Uniform_buffer_array<T>>(data);
    }
};

template<typename T>
class Storage_buffer : public Memory_buffer {  // 注意：类名拼写错误
protected:
    T m_data{};

public:
    Storage_buffer(const T& data) :  
    // 建议使用 DYNAMIC 时改用 COPY 策略
    Memory_buffer(Buffer_type::STORAGE, Buffer_usage::DYNAMIC), 
    m_data(data) {}

    ~Storage_buffer() = default;

    T data() const { return m_data; }
    void set_data(const T& data) { m_data = data; }

    void link(const std::shared_ptr<RHI_device>& device) {
        m_rhi_resource = device->create_memory_buffer(
            m_type,  
            m_usage, 
            sizeof(T), 
            &m_data
        );
    }

    static std::shared_ptr<Storage_buffer<T>> create(const T& data) {
        return std::make_shared<Storage_buffer<T>>(data);
    }
};

template<typename T>
class Storage_buffer_array : public Memory_buffer {
protected:
    std::vector<T> m_data{};

public:

    Storage_buffer_array(const std::vector<T>& data) :
    Memory_buffer(Buffer_type::STORAGE, Buffer_usage::DYNAMIC),
    m_data(data) {}

    ~Storage_buffer_array() = default;
    const std::shared_ptr<T[]> data() const { return m_data; }
    void set_data(const std::shared_ptr<T[]>& data) { m_data = data; }
    void link(const std::shared_ptr<RHI_device>& device) {
        m_rhi_resource = device->create_memory_buffer(
            m_type,
            m_usage,
            sizeof(T) * m_data.size(),
            m_data.data()
        );
    }

    static std::shared_ptr<Storage_buffer_array<T>> create(const std::vector<T>& data) {
        return std::make_shared<Storage_buffer_array<T>>(data);
    }

};

}