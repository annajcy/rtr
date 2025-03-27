#pragma once

#include "engine/global/base.h" 
#include "engine/runtime/enum.h"
#include "engine/runtime/struct.h"

namespace rtr {

struct GPU_access_flags {
    bool is_read;
    bool is_write;
    bool is_buffer_discard;
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

    virtual ~RHI_buffer() = default;
    virtual void bind() = 0;
    virtual void unbind() = 0;
    virtual void reallocate_data(const void* data, unsigned int data_size) = 0;
    virtual void subsitute_data(const void* data, unsigned int data_size, unsigned int offset) = 0;
    virtual void access_gpu_buffer(std::function<void(void*)> accessor, GPU_access_flags flags) = 0;
    virtual unsigned int id() = 0;

    unsigned int data_size() const { return m_data_size; }
    Buffer_type type() const { return m_type; }
    Buffer_usage usage() const { return m_usage; }
};

}; // namespace rtr
