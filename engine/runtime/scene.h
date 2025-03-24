#pragma once
#include "engine/global/base.h"
#include "engine/runtime/mesh.h"
#include "engine/runtime/node.h"
#include "engine/runtime/camera.h"
#include <memory>

namespace rtr {

class Scene : public Node {
public:
    Scene() : Node(Node_type::SCENE){}
    ~Scene() = default;
};

};