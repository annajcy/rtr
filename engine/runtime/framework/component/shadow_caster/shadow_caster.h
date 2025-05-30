#pragma once
#include "engine/runtime/framework/component/camera/camera.h"
#include "engine/runtime/function/render/frontend/texture.h"
#include <memory>

namespace rtr {

class Shadow_caster {

protected:
    std::shared_ptr<Texture_2D> m_shadow_map{};
    std::shared_ptr<Camera> m_shadow_camera{};
    
public:
    Shadow_caster(
        const std::shared_ptr<Camera>& shadow_camera
    ) : m_shadow_camera(shadow_camera) {}

    static std::shared_ptr<Shadow_caster> create(
        const std::shared_ptr<Camera>& shadow_camera
    ) {
        return std::make_shared<Shadow_caster>(
            shadow_camera
        );
    }
    virtual ~Shadow_caster() = default;

    const std::shared_ptr<Texture_2D>& shadow_map() const { return m_shadow_map; }
    std::shared_ptr<Texture_2D>& shadow_map() { return m_shadow_map; }
    const std::shared_ptr<Camera>& shadow_camera() const { return m_shadow_camera; }
    std::shared_ptr<Camera>& shadow_camera() { return m_shadow_camera; }
};


class Directional_light_shadow_caster : public Shadow_caster {
public:
    Directional_light_shadow_caster(
        const std::shared_ptr<Orthographic_camera>& shadow_camera
    ) : Shadow_caster(shadow_camera) {}

    ~Directional_light_shadow_caster() = default;

    static std::shared_ptr<Directional_light_shadow_caster> create(
        const std::shared_ptr<Orthographic_camera>& shadow_camera
    ) {
        return std::make_shared<Directional_light_shadow_caster>(
            shadow_camera
        );
    }

    std::shared_ptr<Orthographic_camera> orthographic_shadow_camera() const {
        return std::dynamic_pointer_cast<Orthographic_camera>(m_shadow_camera);
    }

};

class Point_light_shadow_caster : public Shadow_caster {
public:
    Point_light_shadow_caster(
        const std::shared_ptr<Perspective_camera>& shadow_camera
    ) : Shadow_caster(shadow_camera) {}
    ~Point_light_shadow_caster() = default;
    static std::shared_ptr<Point_light_shadow_caster> create(
        const std::shared_ptr<Perspective_camera>& shadow_camera
    ) {
        return std::make_shared<Point_light_shadow_caster>(
            shadow_camera
        );
    }
    std::shared_ptr<Perspective_camera> perspective_shadow_camera() const {
        return std::dynamic_pointer_cast<Perspective_camera>(m_shadow_camera);
    }

};

}