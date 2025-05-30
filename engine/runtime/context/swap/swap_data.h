#pragma once

#include "engine/runtime/context/swap/camera.h"
#include "engine/runtime/context/swap/light.h"
#include "engine/runtime/context/swap/renderable_object.h"
#include "engine/runtime/context/swap/shadow_caster.h"
#include "engine/runtime/function/render/frontend/geometry.h"
#include "engine/runtime/function/render/frontend/texture.h"

#include "engine/runtime/function/render/material/material.h"
#include "engine/runtime/function/render/utils/skybox.h"

#include "glm/fwd.hpp"
#include <memory>
#include <vector>

namespace rtr {


struct Swap_data {
    
    Swap_camera camera{};
    std::vector<Swap_renderable_object> render_objects{};

    std::vector<Swap_directional_light> directional_lights{};
    std::vector<Swap_point_light> point_lights{};
    std::vector<Swap_spot_light> spot_lights{};

    std::shared_ptr<Skybox> skybox{};    

    Swap_directional_light_shadow_caster dl_shadow_casters{};

    bool enable_csm_shadow{false};
    std::vector<Swap_CSM_shadow_caster> csm_shadow_casters{};

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

    std::vector<Swap_shadow_caster_renderable_object> get_shadow_casters() const {
        std::vector<Swap_shadow_caster_renderable_object> shadow_casters{};
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