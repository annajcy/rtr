#pragma once

#include "engine/runtime/core/geometry.h"
#include "engine/runtime/core/material.h"
#include "engine/runtime/core/texture.h"
#include "engine/runtime/platform/rhi/rhi_geometry.h"
#include "engine/runtime/platform/rhi/rhi_renderer.h"
#include "engine/runtime/platform/rhi/rhi_shader_program.h"
#include "engine/runtime/platform/rhi/rhi_texture.h"
#include <memory>
#include <unordered_map>

namespace rtr {

struct Swap_object {
    std::shared_ptr<Material> material{};
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
};

struct Swap_data {
    std::vector<Swap_object> render_objects{};
    std::vector<Swap_point_light> point_lights{};
    std::vector<Swap_spot_light> spot_lights{};

    Swap_directional_light directional_light{};
    Swap_camera camera{};

    std::shared_ptr<Texture> skybox_texture{};


    void clear() {
        render_objects.clear();
        point_lights.clear();
        spot_lights.clear();
        directional_light = Swap_directional_light{};
        camera = Swap_camera{};
        skybox_texture.reset();
    }
};

}