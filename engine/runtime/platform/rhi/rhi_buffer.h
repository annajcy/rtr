#pragma once

#include "engine/runtime/tool/math.h"

#include <memory>

namespace rtr {


enum class Buffer_type {
    VERTEX,
    ELEMENT,
    UNIFORM,
    STORAGE,
};

enum class Buffer_usage {
    STATIC,
    DYNAMIC,
    STREAM,
};

enum class Buffer_iterate_type {
    PER_VERTEX,
    PER_INSTANCE,
};

enum class Buffer_data_type {
    FLOAT,
    INT,
    UINT,
    BOOL,
};


inline constexpr unsigned int sizeof_buffer_data(Buffer_data_type type) {
    switch (type) {
    case Buffer_data_type::FLOAT: return 4;
    case Buffer_data_type::INT: return 4;
    case Buffer_data_type::UINT: return 4;
    case Buffer_data_type::BOOL: return 1;
    default: return 0;
    }
}

struct RHI_buffer_access_flags {
    bool is_read{true};
    bool is_write{true};
    bool is_buffer_discard{false};

    static RHI_buffer_access_flags read_only() {
        return RHI_buffer_access_flags{
            .is_read = true,
            .is_write = false,
            .is_buffer_discard = false
        };
    }

    static RHI_buffer_access_flags write_only() {
        return RHI_buffer_access_flags{
           .is_read = false,
           .is_write = true,
           .is_buffer_discard = false
        };
    }

    static RHI_buffer_access_flags read_write() {
        return RHI_buffer_access_flags{
          .is_read = true,
          .is_write = true,
          .is_buffer_discard = false
        };
    }
};

class RHI_buffer {
protected:
    Buffer_type m_type{};
    Buffer_usage m_usage{};
    unsigned int m_data_size{};

public:
    RHI_buffer(
        Buffer_type type, 
        Buffer_usage usage,
        unsigned int data_size,
        const void* data
    ) : m_type(type), 
        m_usage(usage), 
        m_data_size(data_size) { }

    virtual ~RHI_buffer() {}

    virtual void reallocate_data(const void* data, unsigned int data_size) = 0;
    virtual void subsitute_data(const void* data, unsigned int data_size, unsigned int offset) = 0;
    virtual void map_buffer(std::function<void(void*)> accessor, const RHI_buffer_access_flags& flags) = 0;

    unsigned int data_size() const { return m_data_size; }
    Buffer_type type() const { return m_type; }
    Buffer_usage usage() const { return m_usage; }
};


class IRHI_vertex_buffer { 
protected:
    Buffer_data_type m_data_type{};
    Buffer_iterate_type m_iterate_type{};
    unsigned int m_unit_data_count{};
public:

    IRHI_vertex_buffer(
        Buffer_data_type data_type,
        Buffer_iterate_type iterate_type,
        unsigned int unit_data_count
    ) : m_data_type(data_type),
        m_iterate_type(iterate_type),
        m_unit_data_count(unit_data_count) {}

    virtual ~IRHI_vertex_buffer() {}
    unsigned int unit_data_count() const { return m_unit_data_count; }
    unsigned int unit_data_size() const { return m_unit_data_count * sizeof_buffer_data(m_data_type); }
    Buffer_data_type buffer_data_type() const { return m_data_type; }
    Buffer_iterate_type iterate_type() const { return m_iterate_type; }

};

class IRHI_element_buffer {
protected:
    unsigned int m_data_count{};
public:
    IRHI_element_buffer(unsigned int data_count) : m_data_count(data_count) {}
    virtual ~IRHI_element_buffer() {}
    unsigned int data_count() const { return m_data_count; }
    Buffer_data_type attribute_type() const { return Buffer_data_type::UINT; }
};


class IRHI_memory_buffer {
protected:
    int m_alignment{0};

public:
    IRHI_memory_buffer() {}
    virtual ~IRHI_memory_buffer() {}
    virtual void bind_memory(unsigned int position) = 0;
    virtual void bind_partial_memory(unsigned int position, unsigned int offset, unsigned int size) = 0;
    int alignment() const { return m_alignment; }
};

class RHI_memory_buffer_binder {
public:
    RHI_memory_buffer_binder() {}
    virtual ~RHI_memory_buffer_binder() {}
    virtual void bind_memory_buffer(const std::shared_ptr<RHI_buffer>& buffer, unsigned int binding_point) = 0;
    virtual void bind_memory_bufer_partial(const std::shared_ptr<RHI_buffer>& buffer, unsigned int binding_point, unsigned int offset, unsigned int size) = 0;
};

}; // namespace rtr
