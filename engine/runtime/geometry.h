#pragma once
#include "engine/global/base.h"
#include "engine/runtime/geometry_attribute.h"
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

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
            {1, std::make_shared<Normal_attribute>(std::vector<float>{
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
            {2, std::make_shared<UV_attribute>(std::vector<float>{
                0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0,
                0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0,
                0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0,
                0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0,
                0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0,
                0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0,
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
};

};