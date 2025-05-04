#pragma once
#include "engine/runtime/framework/component/camera/camera.h"
#include "engine/runtime/function/render/object/texture.h"
#include <memory>

namespace rtr {




struct PCF_settings{
    float radius{};
    float tightness{};
    int sample_count{};
};

class Shadow_caster_settings {
    float light_size{};
    PCF_settings pcf_settings{};
};

class Shadow_caster {

protected:
    std::shared_ptr<Texture_2D> m_shadow_map{};
    std::shared_ptr<Camera> m_shadow_camera{};
    

public:

    

    Shadow_caster(
        const std::shared_ptr<Texture_2D>& shadow_map,
        const std::shared_ptr<Camera>& shadow_camera
    ) : m_shadow_map(shadow_map), m_shadow_camera(shadow_camera) {}

    static std::shared_ptr<Shadow_caster> create(
        const std::shared_ptr<Texture_2D>& shadow_map,
        const std::shared_ptr<Camera>& shadow_camera
    ) {
        return std::make_shared<Shadow_caster>(shadow_map, shadow_camera);
    }
    virtual ~Shadow_caster() = default;

    const std::shared_ptr<Texture_2D>& shadow_map() const { return m_shadow_map; }
    const std::shared_ptr<Camera>& shadow_camera() const { return m_shadow_camera; }
    
};

}