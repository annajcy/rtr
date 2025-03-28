#pragma once

#include "engine/runtime/core/node.h"

namespace rtr {

class Scene : public Node {
public:
    Scene() : Node(Node_type::SCENE){}
    ~Scene() = default;
};

};