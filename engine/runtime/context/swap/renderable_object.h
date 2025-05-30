#pragma once

#include "engine/runtime/function/render/frontend/geometry.h"
#include "engine/runtime/function/render/frontend/texture.h"

#include "engine/runtime/function/render/material/material.h"
#include "engine/runtime/function/render/utils/skybox.h"

#include "glm/fwd.hpp"
#include <memory>
#include <vector>

namespace rtr {

struct Swap_renderable_object {
    std::shared_ptr<Material> material{};
    std::shared_ptr<Geometry> geometry{};
    glm::mat4 model_matrix{1.0f};
    bool is_cast_shadow{false};
};

struct Swap_shadow_caster_renderable_object {
    std::shared_ptr<Geometry> geometry{};
    glm::mat4 model_matrix{1.0f};
};
}