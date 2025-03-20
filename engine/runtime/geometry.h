#pragma once
#include "engine/global/base.h"
#include "engine/global/math.h"
#include "engine/runtime/geometry_attribute.h"
#include "glm/fwd.hpp"
#include <array>

namespace rtr {

class Geometry : public GUID {
protected:
    std::unordered_map<unsigned int, std::shared_ptr<Geometry_vertex_attribute_base>> m_vertex_attributes{};
    std::shared_ptr<Geometry_element_atrribute> m_element_attribute{};

public:
    Geometry(
        std::unordered_map<unsigned int, std::shared_ptr<Geometry_vertex_attribute_base>> vertex_attributes,
        std::shared_ptr<Geometry_element_atrribute> element_attribute
    ) : GUID(), 
        m_vertex_attributes(vertex_attributes), 
        m_element_attribute(element_attribute) {}
    ~Geometry() = default;

    std::unordered_map<unsigned int, std::shared_ptr<Geometry_vertex_attribute_base>> attributes() const { return m_vertex_attributes; }
    std::shared_ptr<Geometry_element_atrribute>& element_attribute() { return m_element_attribute; }
    std::shared_ptr<Geometry_vertex_attribute_base>& attribute(unsigned int location) { return m_vertex_attributes.at(location); }

    static Bouding_box compute_bounding_box(Position_attribute& position_attribute) {
        Bouding_box bounding_box{};
        for (unsigned int i = 0; i < position_attribute.unit_count(); i++) {
            auto position = position_attribute.get_unit(i);
            glm::vec3 position_ = {position[0], position[1], position[2]};
            bounding_box.min = glm::min(bounding_box.min, position_);
            bounding_box.max = glm::max(bounding_box.max, position_);
        }
        return bounding_box;
    }

    static Sphere compute_bounding_sphere(Position_attribute& position_attribute) {
        return Sphere(compute_bounding_box(position_attribute));
    }

    static std::shared_ptr<Geometry> create_box(float size = 1.0f) {
        auto half_size = size * 0.5f;
        std::unordered_map<unsigned int, std::shared_ptr<Geometry_vertex_attribute_base>> vertex_attributes = {
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

        auto element_attribute = std::make_shared<Geometry_element_atrribute>(std::vector<unsigned int>{
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
        std::unordered_map<unsigned int, std::shared_ptr<Geometry_vertex_attribute_base>> vertex_attributes = {
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


        auto element_attribute = std::make_shared<Geometry_element_atrribute>(std::vector<unsigned int>{
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

        std::unordered_map<unsigned int, std::shared_ptr<Geometry_vertex_attribute_base>> vertex_attributes = {
            {0, std::make_shared<Position_attribute>(positions)},
            {1, std::make_shared<UV_attribute>(uvs)},
            {2, std::make_shared<Normal_attribute>(normals)}
        };

        auto element_attribute = std::make_shared<Geometry_element_atrribute>(indices);
        return std::make_shared<Geometry>(vertex_attributes, element_attribute);
    
    }
};

};