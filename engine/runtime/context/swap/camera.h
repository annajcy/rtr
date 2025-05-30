#pragma once

#include "engine/runtime/function/render/frontend/geometry.h"
#include "engine/runtime/function/render/frontend/texture.h"

#include "engine/runtime/function/render/material/material.h"
#include "engine/runtime/function/render/utils/skybox.h"

#include "glm/fwd.hpp"
#include <memory>
#include <vector>

namespace rtr {

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

}