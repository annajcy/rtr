#pragma once
#include "engine/global/base.h"
#include "engine/runtime/core/node.h"
#include "engine/runtime/resource/resource_base.h"
#include <memory>


namespace rtr {

class Model : public Resource_base {
    std::shared_ptr<Node> m_root_node{};

public:
    Model(const std::string& path) : 
    Resource_base(
        Resource_type::MODEL, 
        Hash::from_string(path)) {}

    using Ptr = std::shared_ptr<Model>;

    virtual ~Model() = default;

    const std::shared_ptr<Node>& root_node() const { return m_root_node; }

};

class Model_loader {
public:
    static Model::Ptr load(const std::string& path) { return nullptr; }
};

};