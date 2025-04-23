#include "engine/editor/editor.h"
#include "engine/runtime/core/material.h"
#include "engine/runtime/core/memory_buffer.h"
#include "engine/runtime/core/shader.h"
#include "engine/runtime/core/texture.h"
#include "engine/runtime/core/skybox.h"

#include "engine/runtime/framework/component/camera/camera_component.h"
#include "engine/runtime/framework/component/camera/camera_control_component.h"
#include "engine/runtime/framework/component/custom/rotate_component.h"
#include "engine/runtime/framework/component/light/light_component.h"
#include "engine/runtime/framework/component/mesh_renderer/mesh_renderer_component.h"
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
layout(location = 1) in vec2 a_uv;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec3 a_tangent;

layout(std140, binding = 0) uniform Camera_ubo {
    mat4 view;
    mat4 projection;
    vec3 camera_position;
};

uniform mat4 model;

out vec2 v_uv;
out vec3 v_normal;
out vec3 v_frag_position;
out vec3 v_tangent;

void main() {
    gl_Position = projection * view * model * vec4(a_position, 1.0);

    v_uv = a_uv;
    v_normal = mat3(transpose(inverse(model))) * a_normal;
    v_frag_position = vec3(model * vec4(a_position, 1.0));
    v_tangent = mat3(model) * a_tangent;
}

)";

const char* fragment_shader_source = R"(
// 输入变量
in vec2 v_uv;
in vec3 v_normal;
in vec3 v_frag_position;
in vec3 v_tangent;

#define MAX_DIRECTIONAL_LIGHT 2
#define MAX_SPOT_LIGHT 8
#define MAX_POINT_LIGHT 8

layout(std140, binding = 0) uniform Camera_ubo {
    mat4 view;
    mat4 projection;
    vec3 camera_position;
};

struct Directional_light {
    float intensity;
    vec3 color;
    vec3 direction;
};

layout(std140, binding = 1) uniform Directional_light_array_ubo {
    int dl_count;
    int main_light_index;
    Directional_light dl_lights[MAX_DIRECTIONAL_LIGHT];
};

struct Point_light {
    float intensity;
    vec3 position;
    vec3 color;
    vec3 attenuation;
};

layout(std140, binding = 2) uniform Point_light_array_ubo {
    int pl_count;
    Point_light pl_lights[MAX_POINT_LIGHT];
};

struct Spot_light {
    float intensity;
    float inner_angle_cos;
    float outer_angle_cos;
    vec3 direction;
    vec3 position;
    vec3 color;
};

layout(std140, binding = 3) uniform Spot_light_array_ubo {
    int spl_count;
    Spot_light spl_lights[MAX_SPOT_LIGHT];
};

// 输出变量
out vec4 frag_color;

// 纹理采样器
layout(binding = 0) uniform sampler2D albedo_map;
uniform float transparency;
uniform vec3 ka;     // 环境反射系数
uniform vec3 kd;     // 漫反射系数 (或使用 albedo_map)
uniform vec3 ks;    // 镜面反射系数
uniform float shininess;  


void main() {

    vec3 normalized_normal = normalize(v_normal);

#ifdef ENABLE_ALBEDO_MAP
    vec4 albedo = texture(albedo_map, v_uv);
#else
    vec4 albedo = vec4((normalized_normal + 1.0) / 2.0, 1.0);
#endif

    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    vec3 view_direction = normalize(camera_position - v_frag_position);

    ambient += ka * albedo.rgb;

    // 计算方向光
    for (int i = 0; i < dl_count; i++) {
        vec3 light_direction = normalize(-dl_lights[i].direction);
        vec3 halfway_direction = normalize(light_direction + view_direction);
        float diffuse_factor = max(dot(normalized_normal, light_direction), 0.0);
        float specular_factor = pow(max(dot(normalized_normal, halfway_direction), 0.0), shininess);
        diffuse += kd * dl_lights[i].intensity * dl_lights[i].color * diffuse_factor * albedo.rgb;
        specular += ks * dl_lights[i].intensity * dl_lights[i].color * specular_factor * albedo.rgb;
    }

    // 计算点光源
    for (int i = 0; i < pl_count; i++) {
        vec3 light_direction = normalize(pl_lights[i].position - v_frag_position);
        vec3 halfway_direction = normalize(light_direction + view_direction);
        float diffuse_factor = max(dot(normalized_normal, light_direction), 0.0);
        float specular_factor = pow(max(dot(normalized_normal, halfway_direction), 0.0), shininess);
        float distance = length(pl_lights[i].position - v_frag_position);
        float attenuation = 1.0 / (pl_lights[i].attenuation.x + pl_lights[i].attenuation.y * distance + pl_lights[i].attenuation.z * distance * distance);
        diffuse += kd * pl_lights[i].intensity * pl_lights[i].color * diffuse_factor * albedo.rgb * attenuation;
        specular += ks * pl_lights[i].intensity * pl_lights[i].color * specular_factor * albedo.rgb * attenuation;
    }

    for (int i = 0; i < spl_count; i++) {
        vec3 light_direction = normalize(spl_lights[i].position - v_frag_position);
        vec3 halfway_direction = normalize(light_direction + view_direction);
        float diffuse_factor = max(dot(normalized_normal, light_direction), 0.0);
        float specular_factor = pow(max(dot(normalized_normal, halfway_direction), 0.0), shininess);

        float theta = dot(-light_direction, normalize(spl_lights[i].direction));
        float epsilon = spl_lights[i].inner_angle_cos - spl_lights[i].outer_angle_cos;
        float intensity = clamp((theta - spl_lights[i].outer_angle_cos) / epsilon, 0.0, 1.0);

        diffuse += kd * spl_lights[i].intensity * spl_lights[i].color * diffuse_factor * albedo.rgb  * intensity;
        specular += ks * spl_lights[i].intensity * spl_lights[i].color * specular_factor * albedo.rgb  * intensity;
    }


    frag_color = vec4(ambient + diffuse + specular, transparency);
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

    auto spherical = Skybox::create(Texture_image::create(bk_image));
    scene->set_skybox(spherical);

    // auto cubemap = Skybox::create(Texture_cubemap::create(std::unordered_map<Texture_cubemap_face, std::shared_ptr<Image>>{
    //     {Texture_cubemap_face::BACK, Image_loader::load_from_path(Image_format::RGB_ALPHA, "assets/image/skybox/back.jpg", false)},
    //     {Texture_cubemap_face::BOTTOM, Image_loader::load_from_path(Image_format::RGB_ALPHA, "assets/image/skybox/bottom.jpg", false)},
    //     {Texture_cubemap_face::FRONT, Image_loader::load_from_path(Image_format::RGB_ALPHA, "assets/image/skybox/front.jpg", false)},
    //     {Texture_cubemap_face::LEFT, Image_loader::load_from_path(Image_format::RGB_ALPHA, "assets/image/skybox/left.jpg", false)},
    //     {Texture_cubemap_face::RIGHT, Image_loader::load_from_path(Image_format::RGB_ALPHA, "assets/image/skybox/right.jpg", false)},
    //     {Texture_cubemap_face::TOP, Image_loader::load_from_path(Image_format::RGB_ALPHA, "assets/image/skybox/top.jpg", false)}
    // }));
    // scene->set_skybox(cubemap);

    auto camera_game_object = scene->add_game_object(Game_object::create("camera"));
    auto camera_node = camera_game_object->add_component<Node_component>();
    camera_node->set_position(glm::vec3(0, 0, 5));
    camera_node->look_at_point(glm::vec3(0, 0, 0));

    auto camera = camera_game_object->add_component<Perspective_camera_component>();
    camera->add_resize_callback(runtime->window_system()->window());

    auto camera_control = camera_game_object->add_component<Trackball_camera_control_component>();

    auto shader = Shader::create(
        Shader_program::create(
            "test", 
            std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> {
                {Shader_type::VERTEX, Shader_code::create(Shader_type::VERTEX, vertex_shader_source)},
                {Shader_type::FRAGMENT, Shader_code::create(Shader_type::FRAGMENT, fragment_shader_source)}
            }, 
            std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> {
                {"model", Uniform_entry<glm::mat4>::create(glm::mat4(1.0))},
                {"transparency", Uniform_entry<float>::create(1.0f)},
                {"ka", Uniform_entry<glm::vec3>::create(glm::vec3(0.1, 0.1, 0.1))},
                {"kd", Uniform_entry<glm::vec3>::create(glm::vec3(0.5, 0.5, 0.5))},
                {"ks", Uniform_entry<glm::vec3>::create(glm::vec3(1.0, 1.0, 1.0))},
                {"shininess", Uniform_entry<float>::create(32.0f)}
            }), 
        Shader::get_shader_feature_set(std::vector<Shader_feature> {
            Shader_feature::ALBEDO_MAP
        })
    );

    shader->premake_shader_variants();
    shader->link_all_shader_variants(runtime->rhi_device());

    auto material = Test_material::create(shader);
    material->albedo_map = std::make_shared<Texture_image>(image);
    // material->ka = glm::vec3(0.0);
    // material->kd = glm::vec3(0.0);
    // material->ks = glm::vec3(1.0);
    // material->transparency = 0.5;

    auto game_object = scene->add_game_object(Game_object::create("go1"));
    auto node = game_object->add_component<Node_component>();
    node->set_position(glm::vec3(-2, 0, 0));

    auto mesh_renderer = game_object->add_component<Mesh_renderer_component>();
    mesh_renderer->set_geometry(Geometry::create_sphere());
    mesh_renderer->set_material(material);

    auto game_object2 = scene->add_game_object(Game_object::create("go2"));
    auto node2 = game_object2->add_component<Node_component>();
    node2->set_position(glm::vec3(0, 0, 0));

    auto rotate_component = game_object2->add_component<Rotate_component>();
    rotate_component->speed() = 0.1f;

    auto mesh_renderer2 = game_object2->add_component<Mesh_renderer_component>();
    mesh_renderer2->set_geometry(Geometry::create_box(1.0));
    mesh_renderer2->set_material(material);

    node2->add_child(node, true);
    //node2->add_child(camera_node, true);

    auto game_object3 = scene->add_game_object(Game_object::create("go3"));
    auto node3 = game_object3->add_component<Node_component>();
    auto directional_light = game_object3->add_component<Directional_light_component>();
    //directional_light->color() = glm::vec3(1, 0, 0);
    node3->look_at_direction(glm::vec3(0, 0, -1));

    auto go4 = scene->add_game_object(Game_object::create("go4"));
    auto node4 = go4->add_component<Node_component>();
    node4->set_position(glm::vec3(1, 0, 0));
    auto pl0 = go4->add_component<Point_light_component>();
    pl0->color() = glm::vec3(0, 1, 0);

    auto go5 = scene->add_game_object(Game_object::create("go5"));
    auto node5 = go5->add_component<Node_component>();
    node5->set_position(glm::vec3(-1, 0, 0));
    auto pl1 = go5->add_component<Point_light_component>();
    pl1->color() = glm::vec3(0, 0, 1);

    auto go6 = scene->add_game_object(Game_object::create("go6"));
    auto node6 = go6->add_component<Node_component>();
    node6->set_position(glm::vec3(0, 0, 1.0));
    node6->look_at_direction(glm::vec3(0, 0, -1));
    auto sl0 = go6->add_component<Spot_light_component>();
    sl0->inner_angle() = 15.0f;
    sl0->outer_angle() = 20.0f;
    sl0->color() = glm::vec3(1, 1, 0);
    sl0->intensity() = 0.5f;

    node2->add_child(node6, true);
    
    auto go7 = scene->add_game_object(Game_object::create("go7"));
    auto node7 = go7->add_component<Node_component>();
    node7->set_position(glm::vec3(0.0, 1.0, 0.0));
    node7->look_at_direction(glm::vec3(0.0, -1.0, 0.0));
    auto sl1 = go7->add_component<Spot_light_component>();
    sl1->color() = glm::vec3(1, 1, 0);
    sl1->intensity() = 0.5f;
    
    runtime->world() = world;

    auto editor = editor::Editor::create(runtime);
    editor->run();

    return 0;
}