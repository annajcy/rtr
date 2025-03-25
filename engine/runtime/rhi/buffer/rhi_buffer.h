#pragma once

#include "engine/global/base.h" 
#include "engine/runtime/enum.h"

namespace rtr {

inline constexpr unsigned int get_buffer_attribute_size(Buffer_attribute_type type) {
    switch (type) {
    case Buffer_attribute_type::FLOAT: return 4;
    case Buffer_attribute_type::INT: return 4;
    case Buffer_attribute_type::UINT: return 4;
    case Buffer_attribute_type::BOOL: return 1;
    default: return 0;
    }
}

class RHI_buffer {
protected:
    
    unsigned int m_data_count{};
    unsigned int m_unit_data_count{};
    Buffer_attribute_type m_attribute_type{};
    Buffer_usage m_usage{};
    Buffer_type m_type{};
    const void* m_data{};

public:
    RHI_buffer(
        Buffer_type type, 
        Buffer_usage usage,
        Buffer_attribute_type attribute_type,
        unsigned int unit_data_count,
        unsigned int data_count,
        const void* data
    ) : m_type(type), 
        m_usage(usage), 
        m_data(data), 
        m_attribute_type(attribute_type), 
        m_unit_data_count(unit_data_count),
        m_data_count(data_count) { }

    virtual ~RHI_buffer() = default;
    virtual void init() = 0;
    virtual void bind() = 0;
    virtual void unbind() = 0;
    virtual void destroy() = 0;
    virtual void update_buffer() = 0;

    Buffer_type type() const { return m_type; }
    Buffer_usage usage() const { return m_usage; }
    Buffer_attribute_type attribute_type() const { return m_attribute_type; }
    unsigned int data_count() const { return m_data_count; }
    unsigned int unit_data_count() const { return m_unit_data_count; }
    const void* data() const { return m_data; }
    // 返回数据占用的总字节数
    unsigned int size() const { return m_data_count * get_buffer_attribute_size(m_attribute_type); }
    // 每个单元的大小，具体含义需根据上下文确定
    unsigned int unit_size() { return m_unit_data_count * get_buffer_attribute_size(m_attribute_type); }
    void set_data(const void* new_data) {
        m_data = new_data;
        update_buffer();
    }
    
};

class RHI_vertex_buffer : public RHI_buffer {
protected:
    Buffer_iterate_type m_iterate_type{};
public:
    RHI_vertex_buffer(
        Buffer_usage usage,
        Buffer_attribute_type attribute_type,
        Buffer_iterate_type iterate_type,
        unsigned int unit_data_count,
        unsigned int data_count,
        const void* data
    ) : RHI_buffer(
        Buffer_type::VERTEX, 
        usage, 
        attribute_type, 
        unit_data_count, 
        data_count, 
        data), 
        m_iterate_type(iterate_type) { }


    virtual ~RHI_vertex_buffer() override = default;
    virtual void init() override = 0;
    virtual void bind() override = 0;
    virtual void unbind() override = 0;
    virtual void destroy() override = 0;
    virtual void update_buffer() override = 0;

    Buffer_iterate_type iterate_type() const { return m_iterate_type; }
};


class RHI_element_buffer : public RHI_buffer {
public:
    RHI_element_buffer(
        Buffer_usage usage,
        unsigned int data_count,
        const void* data
    ) : RHI_buffer(
        Buffer_type::INDEX, usage, 
        Buffer_attribute_type::UINT, 
        1, 
        data_count, 
        data) { }

    virtual ~RHI_element_buffer() override = default;
    virtual void init() override = 0;
    virtual void bind() override = 0;
    virtual void unbind() override = 0;
    virtual void destroy() override = 0;
    virtual void update_buffer() override = 0;
};

}; // namespace rtr
