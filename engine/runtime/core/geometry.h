#pragma once
#include "engine/global/base.h"
#include "engine/global/guid.h"
#include "engine/runtime/enum.h"
#include <memory>
#include <string>
#include <unordered_map>

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

class Attribute_base : public GUID {
protected:
    Buffer_usage m_usage{};
    Buffer_data_type m_type{}; 
public:
    Attribute_base(
        Buffer_data_type type, 
        Buffer_usage usage
    ) : GUID(), 
        m_type(type), 
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

    using Ptr = std::shared_ptr<Vertex_attribute_base>;

    virtual ~Vertex_attribute_base() = default;
    virtual unsigned int unit_data_count() const = 0;
    virtual unsigned int unit_count() const = 0;
    virtual unsigned int unit_data_size() const = 0;
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
        Buffer_data_type::UINT,
        usage),
        m_data(data) {}

    using Ptr = std::shared_ptr<Element_atrribute>;

    ~Element_atrribute() = default;
    const std::vector<unsigned int>& data() const { return m_data; }
    unsigned int data_count() const override { return m_data.size(); }
    unsigned int data_size() const override { return m_data.size() * sizeof(unsigned int); }
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

};

using Position_attribute = Vertex_attribute<float, 3>;
using Normal_attribute = Vertex_attribute<float, 3>;
using Tangent_attribute = Vertex_attribute<float, 3>;
using UV_attribute = Vertex_attribute<float, 2>;
using Color_attribute = Vertex_attribute<float, 4>;

class Geometry : public GUID {
protected:
    std::unordered_map<std::string, unsigned int> m_vertex_attribute_names{};
    std::unordered_map<unsigned int, Vertex_attribute_base::Ptr> m_vertex_attributes{};
    Element_atrribute::Ptr m_element_attribute{};

public:
    Geometry(
        const std::unordered_map<std::string, unsigned int>& vertex_attribute_names,
        const std::unordered_map<unsigned int, Vertex_attribute_base::Ptr>& vertex_attributes,
        const Element_atrribute::Ptr & element_attribute
    ) : GUID(), 
        m_vertex_attribute_names(vertex_attribute_names),
        m_vertex_attributes(vertex_attributes), 
        m_element_attribute(element_attribute) {}

    ~Geometry() = default;

    using Ptr = std::shared_ptr<Geometry>;

    const std::unordered_map<std::string, unsigned int>& vertex_attribute_names() const { return m_vertex_attribute_names; }
    std::unordered_map<std::string, unsigned int>& vertex_attribute_names() { return m_vertex_attribute_names; }
    const std::unordered_map<unsigned int, Vertex_attribute_base::Ptr>& attributes() const { return m_vertex_attributes; }
    std::unordered_map<unsigned int, Vertex_attribute_base::Ptr>& attributes() { return m_vertex_attributes; }
    const Element_atrribute::Ptr& element_attribute() const { return m_element_attribute; }
    Element_atrribute::Ptr& element_attribute() { return m_element_attribute; }

    const Vertex_attribute_base::Ptr& attribute(const std::string& name) const {
        auto it = m_vertex_attribute_names.find(name);
        if (it != m_vertex_attribute_names.end()) {
            return m_vertex_attributes.at(it->second);
        }
        static Vertex_attribute_base::Ptr null_attribute = nullptr;
        return null_attribute;
    }

    const Vertex_attribute_base::Ptr& attribute(unsigned int position) const {
        auto it = m_vertex_attributes.find(position);
        if (it != m_vertex_attributes.end()) {
            return it->second;
        } 
        static Vertex_attribute_base::Ptr null_attribute = nullptr;
        return null_attribute;
    }

    void add_attribute(const std::string &name, unsigned int position, const Vertex_attribute_base::Ptr& attribute) {
        m_vertex_attribute_names.insert({name, position});
        m_vertex_attributes.insert({position, attribute});
    }

    void remove_attribute(const std::string& name) {
        auto it_name = m_vertex_attribute_names.find(name);
        if (it_name != m_vertex_attribute_names.end()) {
            m_vertex_attribute_names.erase(it_name);
        }
        auto it_position = m_vertex_attributes.find(it_name->second);
        if (it_position != m_vertex_attributes.end()) {
            m_vertex_attributes.erase(it_position);
        }
    }

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

    static Geometry::Ptr create(
        const std::unordered_map<std::string, unsigned int>& vertex_attribute_names,
        const std::unordered_map<unsigned int, Vertex_attribute_base::Ptr>& vertex_attributes,
        const Element_atrribute::Ptr & element_attribute
    ) {
        return std::make_shared<Geometry>(
            vertex_attribute_names,
            vertex_attributes,
            element_attribute
        );
    }

    static Geometry::Ptr create_box(float size = 1.0f) {
        auto half_size = size * 0.5f;

        std::unordered_map<std::string, unsigned int> vertex_attribute_names = {
            {"position", 0},
            {"uv", 1},
            {"normal", 2},
            {"tangent", 3}  
        };

        std::unordered_map<unsigned int, Vertex_attribute_base::Ptr> vertex_attributes = {
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
            {3, std::make_shared<Tangent_attribute>(std::vector<float>{
                // 每个面的切线方向 (假设UV水平方向为切线方向)
                // Front face
                1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
                // Back face
                -1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
                // Top face
                1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
                // Bottom face
                1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
                // Right face
                0.0f, 0.0f, -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, -1.0f,
                // Left face
                0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f
            })}
        };

        auto element_attribute = std::make_shared<Element_atrribute>(std::vector<unsigned int>{
            0, 1, 2, 2, 3, 0,   // Front face
            4, 5, 6, 6, 7, 4,   // Back face
            8, 9, 10, 10, 11, 8,  // Top face
            12, 13, 14, 14, 15, 12, // Bottom face
            16, 17, 18, 18, 19,  16, // Right face
            20, 21, 22, 22, 23, 20  
        });

        return std::make_shared<Geometry>(vertex_attribute_names, vertex_attributes, element_attribute);
    }


    static Geometry::Ptr create_plane(float width = 2.0f, float height = 2.0f) {
        float half_width = width * 0.5f;
        float half_height = height * 0.5f;

        std::unordered_map<std::string, unsigned int> vertex_attribute_names = {
            {"position", 0},
            {"uv", 1},
            {"normal", 2},
            {"tangent", 3} 
        };

        std::unordered_map<unsigned int, Vertex_attribute_base::Ptr> vertex_attributes = {
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
            })},
            {3, std::make_shared<Tangent_attribute>(std::vector<float>{
                1.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f
            })}
            
        };

        auto element_attribute = std::make_shared<Element_atrribute>(std::vector<unsigned int>{
            0, 1, 2,
            2, 3, 0
        });

        return std::make_shared<Geometry>(vertex_attribute_names, vertex_attributes, element_attribute);
    }

    static Geometry::Ptr create_screen_plane() {
        
        std::unordered_map<std::string, unsigned int> vertex_attribute_names = {
            {"position", 0},
            {"uv", 1}
        };

        std::unordered_map<unsigned int, Vertex_attribute_base::Ptr> vertex_attributes = {
            {0, std::make_shared<Position_attribute>(std::vector<float>{
                -1.0f,  1.0f,
                -1.0f, -1.0f,
                1.0f, -1.0f,
                1.0f,  1.0f,
            })},

            {1, std::make_shared<UV_attribute>(std::vector<float>{
                0.0f, 1.0f,
                0.0f, 0.0f,
                1.0f, 0.0f,
                1.0f, 1.0f
            })}
        };

        auto element_attribute = std::make_shared<Element_atrribute>(std::vector<unsigned int>{
            0, 1, 2,
            0, 2, 3
        });

        return std::make_shared<Geometry>(vertex_attribute_names, vertex_attributes, element_attribute);

    }

    static Geometry::Ptr create_sphere(float radius = 1.0, unsigned int lat_count = 60, unsigned int long_count = 60) {

        std::unordered_map<std::string, unsigned int> vertex_attribute_names = {
            {"position", 0},
            {"uv", 1},
            {"normal", 2},
            {"tangent", 3} 
        };

        auto lat_delta = glm::pi<float>() / lat_count;
        auto long_delta = 2.0f * glm::pi<float>() / long_count;

        std::vector<float> positions{};
        std::vector<float> normals{};
        std::vector<float> tangents{};
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
                positions.push_back(-z);

                normals.push_back(x);
                normals.push_back(y);
                normals.push_back(-z);

                float u = 1.0f - 1.0f * j / long_count;
                float v = 1.0f - 1.0f * i / lat_count;

                uvs.push_back(u);
                uvs.push_back(v);
            }

        for (int i = 0; i < lat_count; i ++) {
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
        }
            
        tangents.resize(positions.size());
        //以三角形为单位进行indices的遍历
        for (int i = 0; i < indices.size(); i += 3) {
            //1 取出当前三角形的三个顶点的索引
            int idx0 = indices[i];
            int idx1 = indices[i + 1];
            int idx2 = indices[i + 2];
    
            //2 根据三个顶点的索引，从positions数组中找到三个顶点的位置信息
            auto p0 = glm::vec3(positions[idx0 * 3], positions[idx0 * 3+1], positions[idx0 * 3+2]);
            auto p1 = glm::vec3(positions[idx1 * 3], positions[idx1 * 3+1], positions[idx1 * 3+2]);
            auto p2 = glm::vec3(positions[idx2 * 3], positions[idx2 * 3+1], positions[idx2 * 3+2]);
    
            //3 根据三个顶点的索引，从uvs数组中找到三个顶点的uv信息
            auto uv0 = glm::vec2(uvs[idx0 * 2], uvs[idx0 * 2+1]);
            auto uv1 = glm::vec2(uvs[idx1 * 2], uvs[idx1 * 2+1]);
            auto uv2 = glm::vec2(uvs[idx2 * 2], uvs[idx2 * 2+1]);
    
            //4 根据公式，计算当前三角形的tangent
            glm::vec3 e0 = p1 - p0;
            glm::vec3 e1 = p2 - p1;
    
            glm::vec2 duv0 = uv1 - uv0;
            glm::vec2 duv1 = uv2 - uv1;
    
            float f = 1.0f / (duv0.x * duv1.y - duv1.x * duv0.y);
    
            glm::vec3 tangent;
            tangent.x = f * (duv1.y * e0.x - duv0.y * e1.x);
            tangent.y = f * (duv1.y * e0.y - duv0.y * e1.y);
            tangent.z = f * (duv1.y * e0.z - duv0.y * e1.z);
            tangent = glm::normalize(tangent);
    
            //5 针对本三角形的三个顶点的normal，使tangent正交化(三个不同的tangent）
            auto normal0 = glm::normalize(glm::vec3(normals[idx0 * 3], normals[idx0 * 3 + 1], normals[idx0 * 3 + 2]));
            auto normal1 = glm::normalize(glm::vec3(normals[idx1 * 3], normals[idx1 * 3 + 1], normals[idx1 * 3 + 2]));
            auto normal2 = glm::normalize(glm::vec3(normals[idx2 * 3], normals[idx2 * 3 + 1], normals[idx2 * 3 + 2]));
    
            auto tangent0 = tangent - glm::dot(tangent, normal0) * normal0;
            auto tangent1 = tangent - glm::dot(tangent, normal1) * normal1;
            auto tangent2 = tangent - glm::dot(tangent, normal2) * normal2;
    
            //6 累加到每个顶点的tangent属性上
            tangents[idx0 * 3] += tangent0.x;
            tangents[idx0 * 3 + 1] += tangent0.y;
            tangents[idx0 * 3 + 2] += tangent0.z;
    
            tangents[idx1 * 3] += tangent1.x;
            tangents[idx1 * 3 + 1] += tangent1.y;
            tangents[idx1 * 3 + 2] += tangent1.z;
    
            tangents[idx2 * 3] += tangent2.x;
            tangents[idx2 * 3 + 1] += tangent2.y;
            tangents[idx2 * 3 + 2] += tangent2.z;
        }

        std::unordered_map<unsigned int, Vertex_attribute_base::Ptr> vertex_attributes = {
            {0, std::make_shared<Position_attribute>(positions)},
            {1, std::make_shared<UV_attribute>(uvs)},
            {2, std::make_shared<Normal_attribute>(normals)},
            {3, std::make_shared<Tangent_attribute>(tangents)}
        };

        auto element_attribute = std::make_shared<Element_atrribute>(indices);
        return std::make_shared<Geometry>(vertex_attribute_names, vertex_attributes, element_attribute);
    }


};

};