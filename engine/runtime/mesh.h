#pragma once
#include "engine/global/base.h"
#include "engine/runtime/node.h"

namespace rtr {

class Mesh : public Node {

public:
    Mesh() : Node(Node_type::MESH) {}
    ~Mesh() = default;

};

}