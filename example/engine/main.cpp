#include "engine/runtime/global/enum.h"
#include "engine/editor/editor.h"
#include "engine/runtime/function/render/core/material.h"
#include "engine/runtime/function/render/core/shader.h"
#include "engine/runtime/function/render/core/texture.h"
#include "engine/runtime/function/render/core/skybox.h"
#include "engine/runtime/function/render/core/geometry.h"

#include "engine/runtime/framework/component/camera/camera_component.h"
#include "engine/runtime/framework/component/camera/camera_control_component.h"
#include "engine/runtime/framework/component/custom/rotate_component.h"
#include "engine/runtime/framework/component/light/light_component.h"
#include "engine/runtime/framework/component/mesh_renderer/mesh_renderer_component.h"
#include "engine/runtime/framework/component/node/node_component.h"
#include "engine/runtime/framework/game_object.h"
#include "engine/runtime/framework/scene.h"
#include "engine/runtime/framework/world.h"

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

out vec3 v_frag_position;
out vec2 v_uv;
out vec3 v_normal;
out vec3 v_tangent;
out mat3 v_tbn;

void main() {
    gl_Position = projection * view * model * vec4(a_position, 1.0);

    v_uv = a_uv;
    v_normal = mat3(transpose(inverse(model))) * a_normal;
    v_frag_position = vec3(model * vec4(a_position, 1.0));
    v_tangent = mat3(model) * a_tangent;

    vec3 T = normalize(v_tangent);
    vec3 N = normalize(v_normal);
    vec3 B = normalize(cross(N, T));
    v_tbn = mat3(T, B, N);
}

)";

const char* fragment_shader_source = R"(
// 输入变量
in vec2 v_uv;
in vec3 v_normal;
in vec3 v_frag_position;
in vec3 v_tangent;
in mat3 v_tbn;

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
#ifdef ENABLE_ALBEDO_MAP
layout(binding = 0) uniform sampler2D albedo_map;
#endif

#ifdef ENABLE_SPECULAR_MAP
layout(binding = 1) uniform sampler2D specular_map;
#endif

#ifdef ENABLE_NORMAL_MAP
layout(binding = 2) uniform sampler2D normal_map;
#endif

#ifdef ENABLE_ALPHA_MAP
layout(binding = 3) uniform sampler2D alpha_map;
#endif

#ifdef ENABLE_HEIGHT_MAP
layout(binding = 4) uniform sampler2D height_map;
#endif

uniform float transparency;
uniform vec3 ka;     // 环境反射系数
uniform vec3 kd;     // 漫反射系数 (或使用 albedo_map)
uniform vec3 ks;    // 镜面反射系数
uniform float shininess;  

vec3 calculate_diffuse(vec3 normal, vec3 light_dir, float intensity, vec3 light_color, vec3 albedo) {
    float diffuse_factor = max(dot(normal, light_dir), 0.0);
    return kd * intensity * light_color * diffuse_factor * albedo;
}

vec3 calculate_specular(vec3 normal, vec3 view_dir, vec3 light_dir, float intensity, vec3 light_color, vec3 albedo, vec3 spec_mask) {
    vec3 halfway_dir = normalize(light_dir + view_dir);
    float specular_factor = pow(max(dot(normal, halfway_dir), 0.0), shininess);
    return ks * intensity * light_color * specular_factor * albedo * spec_mask;
}

float calculate_spot_intensity(vec3 light_dir, vec3 spot_dir, float inner_cos, float outer_cos) {
    float theta = dot(-light_dir, normalize(spot_dir));
    float epsilon = inner_cos - outer_cos;
    return clamp((theta - outer_cos) / epsilon, 0.0, 1.0);
}

#ifdef ENABLE_HEIGHT_MAP
vec2 parallax_uv(vec2 uv, vec3 view_dir, float height, float height_scale, mat3 tbn) {
	view_dir = normalize(transpose(tbn) * view_dir);
	vec2 offset = view_dir.xy / view_dir.z * height * height_scale;
	return uv - offset; 
}
#endif

void main() {

    vec3 view_direction = normalize(camera_position - v_frag_position);

#ifdef ENABLE_HEIGHT_MAP
    float height = texture(height_map, v_uv).r;
    vec2 uv = parallax_uv(v_uv, -view_direction, height, 0.1, v_tbn);
#else
    vec2 uv = v_uv;
#endif

#ifdef ENABLE_ALPHA_MAP
    float alpha = texture(alpha_map, uv).r * transparency;
#else
    float alpha = transparency;
#endif

#ifdef ENABLE_NORMAL_MAP
    vec3 normal_map_normal = texture(normal_map, uv).rgb * 2.0 - vec3(1.0);
    vec3 normal = normalize(v_tbn * normal_map_normal);
    vec3 normalized_normal = normalize(normal);
#else
    vec3 normalized_normal = normalize(v_normal);
#endif

#ifdef ENABLE_ALBEDO_MAP
    vec4 albedo = texture(albedo_map, uv);
#else
    vec4 albedo = vec4((normalized_normal + 1.0) / 2.0, 1.0);
#endif

#ifdef ENABLE_SPECULAR_MAP
    vec3 specular_mask = texture(specular_map, uv).rrr;
#else
    vec3 specular_mask = vec3(1.0);
#endif

    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    ambient += ka * albedo.rgb;

    // 计算方向光
    for (int i = 0; i < dl_count; i++) {
        vec3 light_dir = normalize(-dl_lights[i].direction);
        
        diffuse += calculate_diffuse(
            normalized_normal, 
            light_dir, 
            dl_lights[i].intensity, 
            dl_lights[i].color, 
            albedo.rgb
        );

        specular += calculate_specular(
            normalized_normal, 
            view_direction, 
            light_dir, 
            dl_lights[i].intensity, 
            dl_lights[i].color, 
            albedo.rgb, 
            specular_mask
        );
    }

    // 计算点光源
    for (int i = 0; i < pl_count; i++) {
        vec3 light_dir = normalize(pl_lights[i].position - v_frag_position);
        float distance = length(pl_lights[i].position - v_frag_position);
        float attenuation = 1.0 / 
            (pl_lights[i].attenuation.x + 
            pl_lights[i].attenuation.y * distance + 
            pl_lights[i].attenuation.z * distance * distance);
        
        diffuse += calculate_diffuse(
            normalized_normal, 
            light_dir, 
            pl_lights[i].intensity, 
            pl_lights[i].color, 
            albedo.rgb
        ) * attenuation;

        specular += calculate_specular(
            normalized_normal, 
            view_direction, 
            light_dir, 
            pl_lights[i].intensity, 
            pl_lights[i].color, 
            albedo.rgb, 
            specular_mask
        ) * attenuation;
    }

    // 计算聚光灯
    for (int i = 0; i < spl_count; i++) {
        vec3 light_dir = normalize(spl_lights[i].position - v_frag_position);
        float intensity = calculate_spot_intensity(
            light_dir, 
            spl_lights[i].direction, 
            spl_lights[i].inner_angle_cos, 
            spl_lights[i].outer_angle_cos
        );
        
        diffuse += calculate_diffuse(
            normalized_normal, 
            light_dir, 
            spl_lights[i].intensity, 
            spl_lights[i].color, 
            albedo.rgb
        ) * intensity;

        specular += calculate_specular(
            normalized_normal, 
            view_direction, 
            light_dir, 
            spl_lights[i].intensity, 
            spl_lights[i].color, 
            albedo.rgb, 
            specular_mask
        ) * intensity;
    }


    frag_color = vec4(ambient + diffuse + specular, alpha);
}
)";

int main() {

    auto bk_image = Image_loader::load_from_path(
        Image_format::RGB_ALPHA, 
         "assets/image/skybox/spherical/bk.jpg"
    );

    //specular
    // auto sp_mask = Image_loader::load_from_path(
    //     Image_format::RGB_ALPHA,
    //     "assets/image/box/sp_mask.png"
    // );

    // auto main_tex = Image_loader::load_from_path(
    //     Image_format::RGB_ALPHA, 
    //     "assets/image/box/box.png"
    // );

    // normal
    // auto normal_map = Image_loader::load_from_path(
    //     Image_format::RGB_ALPHA,
    //     "assets/image/brickwall/normal_map.png"
    // );

    // auto main_tex = Image_loader::load_from_path(
    //     Image_format::RGB_ALPHA, 
    //     "assets/image/brickwall/brickwall.jpg"
    // );

    //alpha
    // auto main_tex = Image_loader::load_from_path(
    //     Image_format::RGB_ALPHA, 
    //     "assets/image/grass/grass.jpg"
    // );

    // auto alpha_map = Image_loader::load_from_path(
    //     Image_format::RGB_ALPHA,
    //     "assets/image/grass/grassMask.png"
    // );

    //parallax
    
    auto height_map = Image_loader::load_from_path(
        Image_format::RGB_ALPHA,
        "assets/image/bricks/disp.jpg"
    );
    auto normal_map = Image_loader::load_from_path(
        Image_format::RGB_ALPHA,
        "assets/image/bricks/bricks_normal.jpg"
    );

    auto main_tex = Image_loader::load_from_path(
        Image_format::RGB_ALPHA, 
        "assets/image/bricks/bricks.jpg"
    );

    Engine_runtime_descriptor engine_runtime_descriptor{};
    auto runtime = Engine_runtime::create(engine_runtime_descriptor);

    auto world = World::create("world1");
    auto scene = world->add_scene(Scene::create("scene1"));
    world->set_current_scene(scene);

    // auto spherical = Skybox::create(Texture_image::create(bk_image));
    // scene->set_skybox(spherical);

    auto cubemap = Skybox::create(Texture_cubemap::create(std::unordered_map<Texture_cubemap_face, std::shared_ptr<Image>>{
        {Texture_cubemap_face::BACK, Image_loader::load_from_path(Image_format::RGB_ALPHA, "assets/image/skybox/cubemap/back.jpg", false)},
        {Texture_cubemap_face::BOTTOM, Image_loader::load_from_path(Image_format::RGB_ALPHA, "assets/image/skybox/cubemap/bottom.jpg", false)},
        {Texture_cubemap_face::FRONT, Image_loader::load_from_path(Image_format::RGB_ALPHA, "assets/image/skybox/cubemap/front.jpg", false)},
        {Texture_cubemap_face::LEFT, Image_loader::load_from_path(Image_format::RGB_ALPHA, "assets/image/skybox/cubemap/left.jpg", false)},
        {Texture_cubemap_face::RIGHT, Image_loader::load_from_path(Image_format::RGB_ALPHA, "assets/image/skybox/cubemap/right.jpg", false)},
        {Texture_cubemap_face::TOP, Image_loader::load_from_path(Image_format::RGB_ALPHA, "assets/image/skybox/cubemap/top.jpg", false)}
    }));
    scene->set_skybox(cubemap);

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
            Shader_feature::ALBEDO_MAP,
            Shader_feature::SPECULAR_MAP,
            Shader_feature::NORMAL_MAP,
            Shader_feature::ALPHA_MAP,
            Shader_feature::HEIGHT_MAP
        })
    );

    shader->premake_shader_variants();
    shader->link_all_shader_variants(runtime->rhi_device());

    auto material = Test_material::create(shader);
    material->albedo_map = Texture_image::create(main_tex);
    material->height_map = Texture_image::create(height_map);
    material->normal_map = Texture_image::create(normal_map);
    //material->specular_map = Texture_image::create(sp_mask);
    //material->alpha_map = Texture_image::create(alpha_map);
    // material->normal_map = Texture_image::create(normal_map);
    // material->ka = glm::vec3(0.0);
    // material->kd = glm::vec3(0.0);
    // material->ks = glm::vec3(1.5);
    // material->transparency = 0.5;
    //material->shininess = 16.0;

    auto game_object = scene->add_game_object(Game_object::create("go1"));
    auto node = game_object->add_component<Node_component>();
    node->set_position(glm::vec3(-2, 0, 0));

    auto mesh_renderer = game_object->add_component<Mesh_renderer_component>();
    mesh_renderer->set_geometry(Geometry::create_sphere());
    mesh_renderer->set_material(material);

    auto game_object2 = scene->add_game_object(Game_object::create("go2"));
    auto node2 = game_object2->add_component<Node_component>();
    node2->set_position(glm::vec3(0, 0, 0));

    // auto rotate_component = game_object2->add_component<Rotate_component>();
    // rotate_component->speed() = 0.1f;

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