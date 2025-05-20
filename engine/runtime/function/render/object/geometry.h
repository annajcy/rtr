#pragma once

#include "engine/runtime/function/render/core/render_resource.h"
#include "engine/runtime/function/render/object/attribute_buffer.h"
#include "engine/runtime/platform/rhi/rhi_buffer.h"
#include "engine/runtime/platform/rhi/rhi_device.h"
#include "engine/runtime/platform/rhi/rhi_linker.h"

#include <memory>
#include <unordered_map>

namespace rtr {

class Geometry : public RHI_linker<RHI_geometry>, public Render_resource {
protected:
    std::unordered_map<unsigned int, std::shared_ptr<Vertex_attribute_base>> m_vertex_attributes{};
    std::shared_ptr<Element_atrribute> m_element_attribute{};

public:
    Geometry(
        const std::unordered_map<unsigned int, std::shared_ptr<Vertex_attribute_base>>& vertex_attributes,
        const std::shared_ptr<Element_atrribute> & element_attribute
    ) : Render_resource(Render_resource_type::GEOMETRY), 
        m_vertex_attributes(vertex_attributes), 
        m_element_attribute(element_attribute) {}

    ~Geometry() = default;

    const std::unordered_map<unsigned int, std::shared_ptr<Vertex_attribute_base>>& attributes() const { return m_vertex_attributes; }
    std::unordered_map<unsigned int, std::shared_ptr<Vertex_attribute_base>>& attributes() { return m_vertex_attributes; }
    const std::shared_ptr<Element_atrribute>& element_attribute() const { return m_element_attribute; }
    std::shared_ptr<Element_atrribute>& element_attribute() { return m_element_attribute; }

    const std::shared_ptr<Vertex_attribute_base>& attribute(unsigned int position) const {
        auto it = m_vertex_attributes.find(position);
        if (it != m_vertex_attributes.end()) {
            return it->second;
        } 
        static std::shared_ptr<Vertex_attribute_base> null_attribute = nullptr;
        return null_attribute;
    }

    void link(const std::shared_ptr<RHI_device>& device) override {

        auto get_rhi_buffers = [&](const std::unordered_map<unsigned int, std::shared_ptr<Vertex_attribute_base>>& attributes) {
            std::unordered_map<unsigned int, std::shared_ptr<RHI_buffer>> rhi_buffers{};
            for (const auto& [position, attribute] : attributes) {
                if (!attribute->is_linked()) attribute->link(device);
                rhi_buffers.insert({position, attribute->rhi_resource()});
            }
            return rhi_buffers;
        };

        if (!m_element_attribute->is_linked()) m_element_attribute->link(device);

        m_rhi_resource = device->create_geometry(
            get_rhi_buffers(m_vertex_attributes),
            m_element_attribute->rhi_resource()
        );
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

    static std::shared_ptr<Geometry> create(
        const std::unordered_map<unsigned int, std::shared_ptr<Vertex_attribute_base>>& vertex_attributes,
        const std::shared_ptr<Element_atrribute> & element_attribute
    ) {
        return std::make_shared<Geometry>(
            vertex_attributes,
            element_attribute
        );
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

        return std::make_shared<Geometry>(vertex_attributes, element_attribute);
    }


    static std::shared_ptr<Geometry> create_plane(float width = 2.0f, float height = 2.0f) {
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

        return std::make_shared<Geometry>(vertex_attributes, element_attribute);
    }

    static std::shared_ptr<Geometry> create_screen_plane() {

        std::unordered_map<unsigned int, std::shared_ptr<Vertex_attribute_base>> vertex_attributes = {
            {0, std::make_shared<Screen_position_attribute>(std::vector<float>{
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

        return std::make_shared<Geometry>(vertex_attributes, element_attribute);

    }

    static std::shared_ptr<Geometry> create_sphere(float radius = 1.0, unsigned int lat_count = 60, unsigned int long_count = 60) {

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

        std::unordered_map<unsigned int, std::shared_ptr<Vertex_attribute_base>> vertex_attributes = {
            {0, std::make_shared<Position_attribute>(positions)},
            {1, std::make_shared<UV_attribute>(uvs)},
            {2, std::make_shared<Normal_attribute>(normals)},
            {3, std::make_shared<Tangent_attribute>(tangents)}
        };

        auto element_attribute = std::make_shared<Element_atrribute>(indices);
        return std::make_shared<Geometry>(vertex_attributes, element_attribute);
    }


};

};