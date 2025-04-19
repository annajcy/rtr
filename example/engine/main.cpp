#include "engine/editor/editor.h"
#include "engine/runtime/core/material.h"
#include "engine/runtime/core/memory_buffer.h"
#include "engine/runtime/core/shader.h"
#include "engine/runtime/core/texture.h"
#include "engine/runtime/framework/component/camera/camera_component.h"
#include "engine/runtime/framework/component/camera/camera_control_component.h"
#include "engine/runtime/framework/component/node/node_component.h"
#include "engine/runtime/framework/game_object.h"
#include "engine/runtime/framework/scene.h"
#include "engine/runtime/framework/world.h"
#include "engine/runtime/core/geometry.h"
#include "engine/runtime/global/enum.h"
#include "engine/runtime/platform/rhi/rhi_shader_program.h"
#include "engine/runtime/resource/loader/image_loader.h"
#include "engine/runtime/runtime.h"
#include "glm/fwd.hpp"
#include <memory>
#include <vector>

using namespace rtr;

const char* vertex_shader_source = R"(

layout(location = 0) in vec3 a_position;
layout(location = 2) in vec2 a_uv;
layout(location = 1) in vec3 a_normal;
layout(location = 3) in vec3 a_tangent;

layout(std140, binding = 0) uniform Camera {
    mat4 view;
    mat4 projection;
    vec3 camera_position;
};

uniform mat4 model;

out vec2 v_uv;
out vec3 v_normal;
out vec3 v_frag_position;

void main() {
    gl_Position = projection * view * model * vec4(a_position, 1.0);

    v_uv = a_uv;
    v_normal = mat3(transpose(inverse(model))) * a_normal;
    v_frag_position = vec3(model * vec4(a_position, 1.0));
}

)";

const char* fragment_shader_source = R"(

// 输入变量
in vec2 v_uv;
in vec3 v_normal;
in vec3 v_frag_position;

// 添加宏定义
#define ENABLE_ALBEDO_MAP  // 由材质系统根据是否设置albedo_map动态控制

// 输出变量
out vec4 frag_color;

// 纹理采样器
layout(binding = 0) uniform sampler2D u_albedo_map;

void main() {
#ifdef ENABLE_ALBEDO_MAP
    // 启用albedo时使用纹理采样
    frag_color = texture(u_albedo_map, v_uv);
#else
    // 未启用时用法线值显示（标准化到0~1范围）
    vec3 normalized_normal = normalize(v_normal);
    frag_color = vec4((normalized_normal + 1.0) / 2.0, 1.0);
#endif
}
)";

int main() {

    auto bk_image = Image_loader::load_from_path(
        Image_format::RGB_ALPHA, 
         "assets/image/bk.jpg"
    );

    auto image = Image_loader::load_from_path(
        Image_format::RGB_ALPHA, 
        "assets/image/earth.png"
    );

    Engine_runtime_descriptor engine_runtime_descriptor{};
    auto runtime = Engine_runtime::create(engine_runtime_descriptor);

    auto world = World::create("world1");
    auto scene = world->add_scene(Scene::create("scene1"));
    world->set_current_scene(scene);

    scene->set_skybox(std::make_shared<Texture2D>(bk_image));

    auto game_object = scene->add_game_object(Game_object::create("go1"));
 
    auto node = game_object->add_component<Node_component>(Node_component::create());

    auto camera = game_object->add_component<Perspective_camera_component>(Perspective_camera_component::create(
        45.0f,
        1.0f,
        0.1f,
        100.0f
    ));

    camera->node()->set_position(glm::vec3(0, 0, 3));
    camera->node()->look_at_point(glm::vec3(0, 0, 0));

    auto camera_control = game_object->add_component<Trackball_camera_control_component>(Trackball_camera_control_component::create());

    auto directional_light = game_object->add_component<Directional_light_component>(Directional_light_component::create());
    directional_light->intensity() = 1.0f;
    directional_light->node()->look_at_direction(glm::vec3(0, -1, 0));

    auto point_light = game_object->add_component<Point_light_component>(Point_light_component::create());
    point_light->intensity() = 5;
    point_light->node()->set_position(glm::vec3(-2, 0, 0));

    auto spot_light = game_object->add_component<Spot_light_component>(Spot_light_component::create());
    spot_light->intensity() = 5;
    spot_light->node()->set_position(glm::vec3(2, 0, 0));
    spot_light->inner_angle() = 5.0f;
    spot_light->outer_angle() = 10.0f;

    auto shader = Shader::create(
        Shader_program::create(
            "test", 
            std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> {
                {Shader_type::VERTEX, Shader_code::create(Shader_type::VERTEX, vertex_shader_source)},
                {Shader_type::FRAGMENT, Shader_code::create(Shader_type::FRAGMENT, fragment_shader_source)}
            }, 
            std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> {
                {"model", Uniform_entry<glm::mat4>::create(glm::mat4(1.0))}
            }), 
        Shader::get_feature_set_from_features_list(std::vector<Shader_feature> {
            Shader_feature::ALBEDO_MAP
        })
    );

    shader->premake_shader_variants();

    auto geometry = Geometry::create_sphere();
    auto material = Test_material::create(shader);
    material->albedo_map = std::make_shared<Texture2D>(image);

    auto mesh_renderer = game_object->add_component<Mesh_renderer_component>(Mesh_renderer_component::create(
        geometry,
        material
    ));

    runtime->world() = world;

    auto editor = editor::Editor::create(runtime);
    editor->run();

    return 0;
}