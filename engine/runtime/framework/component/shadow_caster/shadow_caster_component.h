#pragma once
#include "engine/runtime/context/swap_struct.h"
#include "engine/runtime/framework/component/camera/camera.h"
#include "engine/runtime/framework/component/component_base.h"
#include "engine/runtime/framework/component/light/light.h"
#include "engine/runtime/framework/component/node/node.h"
#include "engine/runtime/framework/component/shadow_caster/shadow_caster.h"
#include "engine/runtime/function/render/object/texture.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/matrix.hpp"
#include <cmath>
#include <memory>
#include <utility>
#include <vector>

namespace rtr {
    
class Directional_light_shadow_caster_component : public Component_base {
protected:
    std::shared_ptr<Directional_light> m_directional_light{};
    std::shared_ptr<Perspective_camera> m_main_camera{};
    std::vector<std::shared_ptr<Shadow_caster>> m_shadow_casters{};

    int m_shadow_map_width{1024};
    int m_shadow_map_height{1024};

public:
    Directional_light_shadow_caster_component(
        const std::shared_ptr<Directional_light>& directional_light,
        const std::shared_ptr<Perspective_camera>& main_camera,
        unsigned int csm_layers,
        int shadow_map_width,
        int shadow_map_height
    ) : Component_base(Component_type::SHADOW_CASTER), 
        m_directional_light(directional_light),
        m_main_camera(main_camera),
        m_shadow_casters(csm_layers),
        m_shadow_map_width(shadow_map_width),
        m_shadow_map_height(shadow_map_height) {}

    virtual ~Directional_light_shadow_caster_component() override = default;

    static std::shared_ptr<Directional_light_shadow_caster_component> create(
        const std::shared_ptr<Directional_light>& directional_light,
        const std::shared_ptr<Perspective_camera>& main_camera,
        unsigned int csm_layers,
        int shadow_map_width,
        int shadow_map_height
    ) {
        return std::make_shared<Directional_light_shadow_caster_component>(
            directional_light, 
            main_camera, 
            csm_layers,
            shadow_map_width,
            shadow_map_height
        );
    }

    const std::shared_ptr<Directional_light>& directional_light() const { return m_directional_light; }
    const std::shared_ptr<Perspective_camera>& main_camera() const { return m_main_camera; }
    const std::vector<std::shared_ptr<Shadow_caster>>& shadow_casters() const { return m_shadow_casters; }
    std::vector<std::shared_ptr<Shadow_caster>>& shadow_casters() { return m_shadow_casters; }

    unsigned int csm_layers() const { return m_shadow_casters.size(); }

    std::vector<std::pair<float, float>> generate_csm_layers() {
        std::vector<std::pair<float, float>> layers{};

        float near = m_main_camera->near_bound();
        float far = m_main_camera->far_bound();

        for (int i = 0; i < csm_layers(); i ++) {
            auto d = near * std::pow(far / near, (float)i / (float)csm_layers());
            auto d_next = near * std::pow(far / near, (float)(i + 1) / (float)csm_layers());
            layers.push_back({d, d_next});
        }

        return layers;
    }

    glm::vec3 ndc_to_world(const glm::vec3& ndc, float near, float far) {
        auto view_matrix = m_main_camera->view_matrix();

        auto projection_matrix = glm::perspective(
            glm::radians(m_main_camera->fov()),
            m_main_camera->aspect_ratio(),
            near,
            far
        );

        glm::vec4 ndc_ = glm::vec4(ndc, 1.0f);
        glm::vec4 world = glm::inverse(projection_matrix * view_matrix) * ndc_;
        world /= world.w;
        return glm::vec3(world);
    }

    std::vector<glm::vec3> generate_csm_frustum_vertices(float near, float far) {
        std::vector<glm::vec3> vertices{};
        const auto ndc_vertices = std::vector<glm::vec3>{
            {1.0f, 1.0f, 1.0f},
            {-1.0f, 1.0f, 1.0f},
            {-1.0f, -1.0f, 1.0f},
            {1.0f, -1.0f, 1.0f},
            {1.0f, 1.0f, -1.0f},
            {-1.0f, 1.0f, -1.0f},
            {-1.0f, -1.0f, -1.0f},
            {1.0f, -1.0f, -1.0f}
        };

        for (const auto& ndc : ndc_vertices) {
            vertices.push_back(ndc_to_world(ndc, near, far));
        }

        return vertices;
    }

    
    void update_shadow_casters() {
        auto csm_layers = generate_csm_layers();

        for (size_t i = 0; i < csm_layers.size(); i ++) {
            auto& [near, far] = csm_layers[i];
            auto csm_frustum_vertices = generate_csm_frustum_vertices(near, far);
            
            glm::vec3 shadow_camera_position = glm::vec3(0);
            for (auto &vertex : csm_frustum_vertices) {
                shadow_camera_position += vertex;
            }
            shadow_camera_position /= (float)csm_frustum_vertices.size();

            auto node = Node::create();
            node->set_position(shadow_camera_position);
            node->look_at_direction(m_directional_light->node()->world_front());

            auto view_matrix = glm::lookAt(
                shadow_camera_position, 
                shadow_camera_position + m_directional_light->node()->world_front(),
                glm::vec3(0, 1, 0)
            );

            std::vector<glm::vec3> light_space_frustum_vertices{};
            for (auto &vertex : csm_frustum_vertices) {
                light_space_frustum_vertices.push_back(view_matrix * glm::vec4(vertex, 1.0f));
            }

            float min_x = std::numeric_limits<float>::max();
            float max_x = std::numeric_limits<float>::lowest();
            float min_y = std::numeric_limits<float>::max();
            float max_y = std::numeric_limits<float>::lowest();
            float min_z = std::numeric_limits<float>::max();
            float max_z = std::numeric_limits<float>::lowest();

            for (auto &vertex : light_space_frustum_vertices) {
                min_x = std::min(min_x, vertex.x);
                max_x = std::max(max_x, vertex.x);
                min_y = std::min(min_y, vertex.y);
                max_y = std::max(max_y, vertex.y);
                min_z = std::min(min_z, vertex.z);
                max_z = std::max(max_z, vertex.z);
            }

            auto shadow_camera = Orthographic_camera::create(node);
            shadow_camera->near_bound() = -max_z;
            shadow_camera->far_bound() = -min_z;
            shadow_camera->left_bound() = min_x;
            shadow_camera->right_bound() = max_x;
            shadow_camera->bottom_bound() = min_y;
            shadow_camera->top_bound() = max_y;

            auto shadow_map = Texture_2D::create_depth_attachemnt(
                m_shadow_map_width, 
                m_shadow_map_height
            );

            m_shadow_casters[i] = Shadow_caster::create(
                shadow_map, 
                shadow_camera
            );

        }
    }

    void tick(const Logic_tick_context& tick_context) override {
        update_shadow_casters();
        auto& csm_shadow_maps_data = tick_context.logic_swap_data.csm_shadow_maps;
        csm_shadow_maps_data.resize(m_shadow_casters.size());
        for (size_t i = 0; i < csm_shadow_maps_data.size(); i ++) {
            auto& swap_shadow_map = csm_shadow_maps_data[i];
            auto& shadow_caster = m_shadow_casters[i];
            if (shadow_caster) {
                swap_shadow_map = Swap_shadow_map {
                    .shadow_map = shadow_caster->shadow_map(),
                    .shadow_camera = Swap_camera {
                        .view_matrix = shadow_caster->shadow_camera()->view_matrix(),
                        .projection_matrix = shadow_caster->shadow_camera()->projection_matrix(),
                        .camera_position = shadow_caster->shadow_camera()->node()->world_position(),
                        .camera_direction = shadow_caster->shadow_camera()->node()->world_front(),
                        .near = shadow_caster->shadow_camera()->near_bound(),
                        .far = shadow_caster->shadow_camera()->far_bound()
                    }
                };
            }
        }
    }
};

}