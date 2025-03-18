#pragma once
#include "engine/global/base.h"
#include "engine/runtime/geometry_attribute.h"

namespace rtr {

class Geometry {
protected:
    std::unordered_map<unsigned int, std::shared_ptr<Geometry_vertex_attribute_base>> m_attributes;
    std::shared_ptr<Geometry_element_atrribute> m_element_attribute;

public:
    Geometry(
        std::unordered_map<unsigned int, std::shared_ptr<Geometry_vertex_attribute_base>> attributes,
        std::shared_ptr<Geometry_element_atrribute> element_attribute
    ) : m_attributes(attributes), m_element_attribute(element_attribute) {}
    ~Geometry() = default;

    std::unordered_map<unsigned int, std::shared_ptr<Geometry_vertex_attribute_base>> attributes() const { return m_attributes; }
    std::shared_ptr<Geometry_element_atrribute>& element_attribute() { return m_element_attribute; }
    std::shared_ptr<Geometry_vertex_attribute_base>& attribute(unsigned int location) { return m_attributes.at(location); }

};

};