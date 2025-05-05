#pragma once

#include "engine/runtime/function/render/object/geometry.h"
#include "engine/runtime/function/render/object/material.h"
#include "engine/runtime/function/render/object/skybox.h"
#include "engine/runtime/function/render/object/texture.h"
#include "glm/fwd.hpp"
#include <memory>
#include <vector>

namespace rtr {

struct Swap_object {
    std::shared_ptr<Material> material{};
    std::shared_ptr<Geometry> geometry{};
    glm::mat4 model_matrix{1.0f};
    bool is_cast_shadow{false};
};

struct Swap_shadow_caster_object {
    std::shared_ptr<Geometry> geometry{};
    glm::mat4 model_matrix{1.0f};
};

struct Swap_directional_light {
    float intensity{1.0f};
    glm::vec3 color{1.0f};
    glm::vec3 direction{0.0f, -1.0f, 0.0f};
};

struct Swap_point_light {
    float intensity{1.0f};
    glm::vec3 color{1.0f};
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 attenuation{1.0f, 0.0f, 0.0f};
};

struct Swap_spot_light {
    float intensity{1.0f};
    glm::vec3 color{1.0f};
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 direction{0.0f, -1.0f, 0.0f};
    float inner_angle_cos{0.0f};
    float outer_angle_cos{0.0f};
};

struct Swap_camera {
    glm::mat4 view_matrix{1.0f};
    glm::mat4 projection_matrix{1.0f};
    glm::vec3 camera_position{1.0f};
    glm::vec3 camera_direction{1.0f};
    float near;
    float far;
};

struct Swap_orthographic_camera {
    glm::mat4 view_matrix{1.0f};
    glm::mat4 projection_matrix{1.0f};
    glm::vec3 camera_position{1.0f};
    glm::vec3 camera_direction{1.0f};
    float near;
    float far;
    float left{};
    float right{};
    float bottom{};
    float top{};
};

struct Swap_perspective_camera {
    glm::mat4 view_matrix{1.0f};
    glm::mat4 projection_matrix{1.0f};
    glm::vec3 camera_position{1.0f};
    glm::vec3 camera_direction{1.0f};
    float near;
    float far;
    float fov{};
    float aspect_ratio{};
};

struct Swap_directional_light_shadow_caster  {
    std::shared_ptr<Texture_2D> shadow_map{};
    Swap_orthographic_camera shadow_camera{};
};

struct Swap_point_light_shadow_caster {
    std::shared_ptr<Texture_2D> shadow_map{};
    Swap_perspective_camera shadow_camera{};
};

struct Swap_data {
    
    Swap_camera camera{};
    std::vector<Swap_object> render_objects{};

    std::vector<Swap_directional_light> directional_lights{};
    std::vector<Swap_point_light> point_lights{};
    std::vector<Swap_spot_light> spot_lights{};

    std::shared_ptr<Skybox> skybox{};    

    Swap_directional_light_shadow_caster dl_shadow_casters{};
    
    bool enable_csm{false};
    std::vector<Swap_directional_light_shadow_caster> csm_shadow_casters{};

    void clear() {
        render_objects.clear();
        point_lights.clear();
        spot_lights.clear();
        directional_lights.clear();
        csm_shadow_casters.clear();
        camera = Swap_camera{};
        dl_shadow_casters = Swap_directional_light_shadow_caster{};
        skybox.reset();
    }

    std::vector<Swap_shadow_caster_object> get_shadow_casters() const {
        std::vector<Swap_shadow_caster_object> shadow_casters{};
        for (const auto& obj : render_objects) {
            if (obj.is_cast_shadow) {
                shadow_casters.push_back({
                    .geometry = obj.geometry,
                    .model_matrix = obj.model_matrix
                });
            }
        }
        return shadow_casters;
    }
};

}