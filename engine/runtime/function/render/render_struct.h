#pragma once

#include "engine/runtime/global/base.h"
#include "engine/runtime/platform/rhi/rhi_device.h"

namespace rtr {

struct Render_object {
    std::unordered_map<unsigned int, std::shared_ptr<RHI_texture>> textures{};
    std::shared_ptr<RHI_shader_program> shader_program{};
    std::shared_ptr<RHI_geometry> geometry{};
    glm::mat4 model_matrix{1.0f};
};

struct Render_directional_light {
    float intensity{1.0f};
    glm::vec3 color{1.0f};
    glm::vec3 direction{0.0f, -1.0f, 0.0f};
};

struct Render_point_light {
    float intensity{1.0f};
    glm::vec3 color{1.0f};
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 attenuation{1.0f, 0.0f, 0.0f};
};

struct Render_spot_light {
    float intensity{1.0f};
    glm::vec3 color{1.0f};
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 direction{0.0f, -1.0f, 0.0f};
    float inner_angle_cos{0.0f};
    float outer_angle_cos{0.0f};
};

struct Render_camera {
    glm::mat4 view_matrix{1.0f};
    glm::mat4 projection_matrix{1.0f};
    glm::vec3 camera_position{1.0f};
};

struct Swap_data {
    std::vector<Render_object> render_objects{};
    std::vector<Render_point_light> point_lights{};
    std::vector<Render_spot_light> spot_lights{};

    Render_directional_light directional_light{};
    Render_camera camera{};


    void clear() {
        render_objects.clear();
        point_lights.clear();
        spot_lights.clear();
        directional_light = Render_directional_light{};
        camera = Render_camera{};
    }
};

struct Execute_context {
    std::shared_ptr<RHI_renderer> renderer{};
    std::shared_ptr<RHI_frame_buffer> target_frame_buffer{};
    const Swap_data& render_data;
};

}