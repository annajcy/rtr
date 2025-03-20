#pragma once
#include "engine/global/base.h"
#include "engine/runtime/mesh.h"
#include "engine/runtime/node.h"
#include "engine/runtime/camera.h"
#include <memory>

namespace rtr {

class Scene : public Node {
private:
    std::shared_ptr<Mesh> m_sky_box{};

public:
    Scene(std::shared_ptr<Mesh>& skybox) : Node(Node_type::SCENE), m_sky_box(skybox) {}
    ~Scene() = default;
    
};

};