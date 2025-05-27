#pragma once

#include "engine/runtime/platform/rhi/rhi_buffer.h"
#include "engine/runtime/platform/rhi/rhi_linker.h"
#include <type_traits>

namespace rtr {

template<typename T>
inline constexpr Buffer_data_type get_buffer_atrribute_type() {
    if constexpr (std::is_same_v<T, float>) {
        return Buffer_data_type::FLOAT;
    } else if constexpr (std::is_same_v<T, int>) {
        return Buffer_data_type::INT;
    } else if constexpr (std::is_same_v<T, unsigned int>) {
        return Buffer_data_type::UINT;  
    } else if constexpr (std::is_same_v<T, bool>) {
        return Buffer_data_type::BOOL;
    } else {
        static_assert(false, "Unsupported type");
    }
}

class Attribute_base : public RHI_linker<RHI_buffer> {
protected:
    Buffer_usage m_usage{};
    Buffer_data_type m_type{}; 

public:
    Attribute_base(
        Buffer_data_type type, 
        Buffer_usage usage
    ) : m_type(type), 
        m_usage(usage) {}

    virtual ~Attribute_base() = default;
    virtual const void* data_ptr() const = 0;
    virtual unsigned int data_count() const = 0;
    virtual unsigned int data_size() const = 0;
    
    Buffer_data_type type() const { return m_type; }
    Buffer_usage usage() const { return m_usage; }
};

class Vertex_attribute_base : public Attribute_base {
protected:
    Buffer_iterate_type m_iterate_type{};

public:
    Vertex_attribute_base(
        Buffer_data_type type, 
        Buffer_usage usage,
        Buffer_iterate_type iterate_type
    ) : Attribute_base(type, usage),
        m_iterate_type(iterate_type) {}

    virtual ~Vertex_attribute_base() = default;
    virtual unsigned int unit_data_count() const = 0;
    virtual unsigned int unit_count() const = 0;
    virtual unsigned int unit_data_size() const = 0;
    Buffer_iterate_type iterate_type() const { return m_iterate_type; }

    void link(const std::shared_ptr<RHI_device>& device) override {
        m_rhi = device->create_vertex_buffer(
            usage(), 
            type(), 
            iterate_type(), 
            unit_data_count(), 
            data_size(), 
            data_ptr()
        );
    }
    
};

class Element_atrribute : public Attribute_base {
private:
    std::vector<unsigned int> m_data{};

public:
    Element_atrribute(
        const std::vector<unsigned int>& data,
        Buffer_usage usage = Buffer_usage::STATIC
    ) : Attribute_base(
        Buffer_data_type::UINT,
        usage),
        m_data(data) {}

    ~Element_atrribute() = default;
    const std::vector<unsigned int>& data() const { return m_data; }
    unsigned int data_count() const override { return m_data.size(); }
    unsigned int data_size() const override { return m_data.size() * sizeof(unsigned int); }
    unsigned int& operator[](const int index) { return m_data[index]; }
    const unsigned int& operator[](const int index) const { return m_data[index]; }
    const void* data_ptr() const override { return reinterpret_cast<const void*>(m_data.data()); }

    void link(const std::shared_ptr<RHI_device>& device) override {
        m_rhi = device->create_element_buffer(
            usage(),
            data_count(),
            data_size(),
            data_ptr()
        );
    }

    static std::shared_ptr<Element_atrribute> create(
        const std::vector<unsigned int>& data,
        Buffer_usage usage = Buffer_usage::STATIC
    ) {
        return std::make_shared<Element_atrribute>(data, usage);
    }
};

template<typename T, unsigned int UNIT_DATA_COUNT>
class Vertex_attribute : public Vertex_attribute_base {
private:
    std::vector<T> m_data{};

public:
    Vertex_attribute(
        const std::vector<T>& data, 
        Buffer_usage usage = Buffer_usage::STATIC, 
        Buffer_iterate_type iterate_type = Buffer_iterate_type::PER_VERTEX
    ) : Vertex_attribute_base(
        get_buffer_atrribute_type<T>(), 
        usage, 
        iterate_type),
        m_data(data) {}

    ~Vertex_attribute() = default;

    std::vector<T>& data() { return m_data; }
    unsigned int data_count() const override { return m_data.size(); }
    unsigned int data_size() const override { return m_data.size() * sizeof(T); }
    unsigned int unit_data_count() const override  { return UNIT_DATA_COUNT; }
    unsigned int unit_data_size() const override { return UNIT_DATA_COUNT * sizeof(T); }
    unsigned int unit_count() const override { return m_data.size() / UNIT_DATA_COUNT; }
    const void* data_ptr() const override { return reinterpret_cast<const void*>(m_data.data()); }

    T& unit_data(unsigned int unit_index, unsigned int unit_data_index) { return m_data[unit_index * UNIT_DATA_COUNT + unit_data_index]; }
    const T& unit_data(unsigned int unit_index, unsigned int unit_data_index) const { return m_data[unit_index * UNIT_DATA_COUNT + unit_data_index]; }
    
    std::array<T, UNIT_DATA_COUNT> get_unit(unsigned int unit_index) const {
        std::array<T, UNIT_DATA_COUNT> unit_data{};
        for (unsigned int i = 0; i < UNIT_DATA_COUNT; i++) {
            unit_data[i] = m_data[unit_index * UNIT_DATA_COUNT + i];
        }
        return unit_data;
    }

    void set_unit(unsigned int unit_index, const std::array<T, UNIT_DATA_COUNT>& unit_data) {
        for (unsigned int i = 0; i < UNIT_DATA_COUNT; i++) {
            m_data[unit_index * UNIT_DATA_COUNT + i] = unit_data[i];
        }
    }

    void apply_unit(unsigned int unit_index, const std::function<void(std::array<T, UNIT_DATA_COUNT>&)>& func) {
        std::array<T, UNIT_DATA_COUNT> unit_data = get_unit(unit_index);
        func(unit_data);
        set_unit(unit_index, unit_data);
    }

    void apply(const std::function<void(std::vector<T>&)>& func) {
        func(m_data);
    }

    static std::shared_ptr<Vertex_attribute<T, UNIT_DATA_COUNT>> create(
        const std::vector<T>& data,
        Buffer_usage usage = Buffer_usage::STATIC,
        Buffer_iterate_type iterate_type = Buffer_iterate_type::PER_VERTEX
    ) {
        return std::make_shared<Vertex_attribute<T, UNIT_DATA_COUNT>>(data, usage, iterate_type);
    }

};

using Position_attribute = Vertex_attribute<float, 3>;
using Screen_position_attribute = Vertex_attribute<float, 2>;
using Normal_attribute = Vertex_attribute<float, 3>;
using Tangent_attribute = Vertex_attribute<float, 3>;
using UV_attribute = Vertex_attribute<float, 2>;
using Color_attribute = Vertex_attribute<float, 4>;

}