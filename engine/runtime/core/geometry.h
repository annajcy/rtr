#pragma once
#include "engine/global/base.h"
#include "engine/global/guid.h"
#include "engine/runtime/enum.h"

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

class Attribute_base : public GUID {
protected:
    Buffer_usage m_usage{};
    Buffer_attribute_type m_type{}; 
public:
    Attribute_base(
        Buffer_attribute_type type, 
        Buffer_usage usage
    ) : GUID(), 
        m_type(type), 
        m_usage(usage) {}

    virtual ~Attribute_base() = default;
    virtual const void* data_ptr() const = 0;
    virtual unsigned int data_count() const = 0;
    
    Buffer_attribute_type type() const { return m_type; }
    Buffer_usage usage() const { return m_usage; }
};

class Vertex_attribute_base : public Attribute_base {
protected:
    Buffer_iterate_type m_iterate_type{};
public:
    Vertex_attribute_base(
        Buffer_attribute_type type, 
        Buffer_usage usage,
        Buffer_iterate_type iterate_type
    ) : Attribute_base(type, usage),
        m_iterate_type(iterate_type) {}
    virtual ~Vertex_attribute_base() = default;
    virtual unsigned int unit_data_count() const = 0;
    virtual unsigned int unit_count() const = 0;
    Buffer_iterate_type iterate_type() const { return m_iterate_type; }
    
};

class Element_atrribute : public Attribute_base {
private:
    std::vector<unsigned int> m_data{};
public:
    Element_atrribute(
        const std::vector<unsigned int>& data,
        Buffer_usage usage = Buffer_usage::STATIC
    ) : Attribute_base(
        Buffer_attribute_type::UINT,
        usage),
        m_data(data) {}

    ~Element_atrribute() = default;
    const std::vector<unsigned int>& data() const { return m_data; }
    unsigned int data_count() const override { return m_data.size(); }
    unsigned int& operator[](const int index) { return m_data[index]; }
    const unsigned int& operator[](const int index) const { return m_data[index]; }
    const void* data_ptr() const override { return reinterpret_cast<const void*>(m_data.data()); }
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
    unsigned int unit_data_count() const override  { return UNIT_DATA_COUNT; }
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

};

using Position_attribute = Vertex_attribute<float, 3>;
using Normal_attribute = Vertex_attribute<float, 3>;
using UV_attribute = Vertex_attribute<float, 2>;
using Color_attribute = Vertex_attribute<float, 4>;

class Geometry : public GUID {
protected:
    std::unordered_map<unsigned int, std::shared_ptr<Vertex_attribute_base>> m_vertex_attributes{};
    std::shared_ptr<Element_atrribute> m_element_attribute{};

public:
    Geometry(
        std::unordered_map<unsigned int, std::shared_ptr<Vertex_attribute_base>> vertex_attributes,
        std::shared_ptr<Element_atrribute> element_attribute
    ) : GUID(), 
        m_vertex_attributes(vertex_attributes), 
        m_element_attribute(element_attribute) {}
    ~Geometry() = default;

    std::unordered_map<unsigned int, std::shared_ptr<Vertex_attribute_base>> attributes() const { return m_vertex_attributes; }
    std::shared_ptr<Element_atrribute>& element_attribute() { return m_element_attribute; }
    std::shared_ptr<Vertex_attribute_base>& attribute(unsigned int location) { return m_vertex_attributes.at(location); }

    static Bouding_box compute_bounding_box(const Position_attribute& position_attribute) {
        Bouding_box bounding_box{};
        for (unsigned int i = 0; i < position_attribute.unit_count(); i++) {
            auto position = position_attribute.get_unit(i);
            glm::vec3 position_ = {position[0], position[1], position[2]};
            bounding_box.min = glm::min(bounding_box.min, position_);
            bounding_box.max = glm::max(bounding_box.max, position_);
        }
        return bounding_box;
    }

    static Sphere compute_bounding_sphere(const Position_attribute& position_attribute) {
        return Sphere(compute_bounding_box(position_attribute));
    }

    static std::shared_ptr<Geometry> create_box(float size = 1.0f) {
        auto half_size = size * 0.5f;
        std::unordered_map<unsigned int, std::shared_ptr<Vertex_attribute_base>> vertex_attributes = {
            {0, std::make_shared<Position_attribute>(std::vector<float>{
                // Front face
                -half_size, -half_size, half_size, half_size, -half_size, half_size, half_size, half_size, half_size, -half_size, half_size, half_size,
                // Back face
                -half_size, -half_size, -half_size, -half_size, half_size, -half_size, half_size, half_size, -half_size, half_size, -half_size, -half_size,
                // Top face
                -half_size, half_size, half_size, half_size, half_size, half_size, half_size, half_size, -half_size, -half_size, half_size, -half_size,
                // Bottom face
                -half_size, -half_size, -half_size, half_size, -half_size, -half_size, half_size, -half_size, half_size, -half_size, -half_size, half_size,
                // Right face
                half_size, -half_size, half_size, half_size, -half_size, -half_size, half_size, half_size, -half_size, half_size, half_size, half_size,
                // Left face
                -half_size, -half_size, -half_size, -half_size, -half_size, half_size, -half_size, half_size, half_size, -half_size, half_size, -half_size
            })},

            {1, std::make_shared<UV_attribute>(std::vector<float>{
                0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0,
                0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0,
                0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0,
                0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0,
                0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0,
                0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0,
            })},

            {2, std::make_shared<Normal_attribute>(std::vector<float>{
                //front
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,

                //back
                0.0f, 0.0f, -1.0f,
                0.0f, 0.0f, -1.0f,
                0.0f, 0.0f, -1.0f,
                0.0f, 0.0f, -1.0f,

                //up
                0.0f, 1.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 1.0f, 0.0f,

                //down
                0.0f, -1.0f, 0.0f,
                0.0f, -1.0f, 0.0f,
                0.0f, -1.0f, 0.0f,
                0.0f, -1.0f, 0.0f,

                //right
                1.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f,

                //left
                -1.0f, 0.0f, 0.0f,
                -1.0f, 0.0f, 0.0f,
                -1.0f, 0.0f, 0.0f,
                -1.0f, 0.0f, 0.0f,
            })},
        };

        auto element_attribute = std::make_shared<Element_atrribute>(std::vector<unsigned int>{
            0, 1, 2, 2, 3, 0,   // Front face
            4, 5, 6, 6, 7, 4,   // Back face
            8, 9, 10, 10, 11, 8,  // Top face
            12, 13, 14, 14, 15, 12, // Bottom face
            16, 17, 18, 18, 19,  16, // Right face
            20, 21, 22, 22, 23, 20  
        });

        return std::make_shared<Geometry>(vertex_attributes, element_attribute);
    }


    static std::shared_ptr<Geometry> create_plane(float width, float height) {
        float half_width = width * 0.5f;
        float half_height = height * 0.5f;
        std::unordered_map<unsigned int, std::shared_ptr<Vertex_attribute_base>> vertex_attributes = {
            {0, std::make_shared<Position_attribute>(std::vector<float>{
                -half_width, -half_height, 0.0f,
                half_width, -half_height, 0.0f,
                half_width, half_height, 0.0f,
                -half_width, half_height, 0.0f,
            })},

            {1, std::make_shared<UV_attribute>(std::vector<float>{
                0.0f, 0.0f,
                1.0f, 0.0f,
                1.0f, 1.0f,
                0.0f, 1.0f
            })},

            {2, std::make_shared<Normal_attribute>(std::vector<float>{
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
            })}
        };


        auto element_attribute = std::make_shared<Element_atrribute>(std::vector<unsigned int>{
            0, 1, 2,
            2, 3, 0
        });

        return std::make_shared<Geometry>(vertex_attributes, element_attribute);
    }

    static std::shared_ptr<Geometry> create_sphere(float radius = 1.0, unsigned int lat_count = 60, unsigned int long_count = 60) {
        auto lat_delta = glm::pi<float>() / lat_count;
        auto long_delta = 2.0f * glm::pi<float>() / long_count;

        std::vector<float> positions{};
        std::vector<float> normals{};
        std::vector<float> uvs{};
        std::vector<unsigned int> indices{};

        for (int i = 0; i <= lat_count; i ++) 
            for (int j = 0; j <= long_count; j ++) {
                float phi = i * lat_delta;
                float theta = j * long_delta;

                float y = radius * cos(phi);
                float x = radius * sin(phi) * cos(theta);
                float z = radius * sin(phi) * sin(theta);

                positions.push_back(x);
                positions.push_back(y);
                positions.push_back(z);

                normals.push_back(x);
                normals.push_back(y);
                normals.push_back(z);

                float u = 1.0f - 1.0f * j / long_count;
                float v = 1.0f - 1.0f * i / lat_count;

                uvs.push_back(u);
                uvs.push_back(v);
            }

        for (int i = 0; i < lat_count; i ++) 
            for (int j = 0; j < long_count; j ++) {
                int p1 = i * (long_count + 1) + j, p3 = p1 + 1;
                int p2 = p1 + long_count + 1, p4 = p2 + 1;

                indices.push_back(p1);
                indices.push_back(p2);
                indices.push_back(p3);
                indices.push_back(p3);
                indices.push_back(p2);
                indices.push_back(p4);
            }

        std::unordered_map<unsigned int, std::shared_ptr<Vertex_attribute_base>> vertex_attributes = {
            {0, std::make_shared<Position_attribute>(positions)},
            {1, std::make_shared<UV_attribute>(uvs)},
            {2, std::make_shared<Normal_attribute>(normals)}
        };

        auto element_attribute = std::make_shared<Element_atrribute>(indices);
        return std::make_shared<Geometry>(vertex_attributes, element_attribute);
    
    }


};

};