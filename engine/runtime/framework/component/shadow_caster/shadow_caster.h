#pragma once
#include "engine/runtime/framework/component/camera/camera.h"
#include "engine/runtime/framework/component/component_base.h"
#include "engine/runtime/function/render/object/texture.h"
#include <memory>

namespace rtr {
class Shadow_caster {

protected:
    std::shared_ptr<Texture_depth_attachment> m_shadow_map{};
    std::shared_ptr<Camera> m_shadow_camera{};

public:


    

};

}