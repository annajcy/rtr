#pragma once
#include "engine/global/base.h"
#include "engine/runtime/rhi/buffer/rhi_buffer.h"
#include <array>
#include <vector>

namespace rtr {

template<typename T>
inline constexpr Buffer_attribute_type get_buffer_atrribute_type() {
    if constexpr (std::is_same_v<T, float>) {
        return Buffer_attribute_type::FLOAT;
    } else if constexpr (std::is_same_v<T, int>) {
        return Buffer_attribute_type::INT;
    } else if constexpr (std::is_same_v<T, unsigned int>) {
        return Buffer_attribute_type::UINT;  
    } else if constexpr (std::is_same_v<T, bool>) {
        return Buffer_attribute_type::BOOL;
    } else {
        static_assert(false, "Unsupported type");
    }
}

class Geometry_attribute_base {
protected:
    Buffer_usage m_usage{};
    Buffer_attribute_type m_type{}; 
public:
    Geometry_attribute_base(
        Buffer_attribute_type type, 
        Buffer_usage usage
    ) : m_type(type), 
        m_usage(usage) {}

    virtual ~Geometry_attribute_base() = default;
    virtual const void* data_ptr() = 0;
    virtual unsigned int data_count() const = 0;
    
    Buffer_attribute_type type() const { return m_type; }
    Buffer_usage usage() const { return m_usage; }
};

class Geometry_vertex_attribute_base {
protected:
    Buffer_iterate_type m_iterate_type{};
public:
    Geometry_vertex_attribute_base(
        Buffer_iterate_type iterate_type
    ) : m_iterate_type(iterate_type) {}
    virtual ~Geometry_vertex_attribute_base() = default;
    virtual unsigned int unit_data_count() const = 0;
    virtual unsigned int unit_count() const = 0;
    Buffer_iterate_type iterate_type() const { return m_iterate_type; }
};

class Geometry_element_atrribute : public Geometry_attribute_base {
private:
    std::vector<unsigned int> m_data{};
public:
    Geometry_element_atrribute(
        const std::vector<unsigned int>& data,
        Buffer_usage usage
    ) : Geometry_attribute_base(
        Buffer_attribute_type::UINT,
        usage),
        m_data(data) {}

    ~Geometry_element_atrribute() = default;
    const std::vector<unsigned int>& data() const { return m_data; }
    unsigned int data_count() const override { return m_data.size(); }
    unsigned int& operator[](const int index) { return m_data[index]; }
    const unsigned int& operator[](const int index) const { return m_data[index]; }
    const void* data_ptr() override { return reinterpret_cast<const void*>(m_data.data()); }

};

template<typename T, unsigned int UNIT_DATA_COUNT>
class Geometry_vertex_attribute : public Geometry_attribute_base, public Geometry_vertex_attribute_base {
private:
    
    std::vector<T> m_data{};

public:
    Geometry_vertex_attribute(
        const std::vector<T>& data, 
        Buffer_usage usage, 
        Buffer_iterate_type iterate_type
    ) : Geometry_attribute_base(
        get_buffer_atrribute_type<T>(), 
        usage),
        Geometry_vertex_attribute_base(iterate_type),
        m_data(data) {}

    ~Geometry_vertex_attribute() = default;

    const std::vector<T>& data() const { return m_data; }
    unsigned int data_count() const override { return m_data.size(); }
    unsigned int unit_data_count() const override  { return UNIT_DATA_COUNT; }
    unsigned int unit_count() const override { return m_data.size() / UNIT_DATA_COUNT; }
    const void* data_ptr() override { return reinterpret_cast<const void*>(m_data.data()); }

    T& unit_data(unsigned int unit_index, unsigned int unit_data_index) { return m_data[unit_index * UNIT_DATA_COUNT + unit_data_index]; }
    const T& unit_data(unsigned int unit_index, unsigned int unit_data_index) const { return m_data[unit_index * UNIT_DATA_COUNT + unit_data_index]; }
    
    std::array<T, UNIT_DATA_COUNT> get_unit(unsigned int unit_index) {
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
};

using Position_attribute = Geometry_vertex_attribute<float, 3>;
using Normal_attribute = Geometry_vertex_attribute<float, 3>;
using UV_attribute = Geometry_vertex_attribute<float, 2>;
using Color_attribute = Geometry_vertex_attribute<float, 4>;

}