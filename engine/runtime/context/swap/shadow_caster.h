#pragma once

#include "engine/runtime/function/render/frontend/geometry.h"
#include "engine/runtime/function/render/frontend/texture.h"

#include "engine/runtime/function/render/material/material.h"
#include "engine/runtime/function/render/utils/skybox.h"

#include "glm/fwd.hpp"
#include <memory>
#include <vector>

#include "engine/runtime/context/swap/camera.h"

namespace rtr {

struct Swap_directional_light_shadow_caster  {
    std::shared_ptr<Texture_2D> shadow_map{};
    Swap_orthographic_camera shadow_camera{};
};

struct Swap_point_light_shadow_caster {
    std::shared_ptr<Texture_2D> shadow_map{};
    Swap_perspective_camera shadow_camera{};
};

struct Swap_CSM_shadow_caster {
    Swap_directional_light_shadow_caster shadow_caster{};
    float split_near{};
    float split_far{};
};



}