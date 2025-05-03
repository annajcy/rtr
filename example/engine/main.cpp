#include "engine/runtime/global/enum.h"
#include "engine/editor/editor.h"
#include "engine/runtime/function/render/object/material.h"
#include "engine/runtime/function/render/object/shader.h"
#include "engine/runtime/function/render/object/texture.h"
#include "engine/runtime/function/render/object/skybox.h"
#include "engine/runtime/function/render/object/geometry.h"

#include "engine/runtime/framework/component/camera/camera_component.h"
#include "engine/runtime/framework/component/camera/camera_control_component.h"
#include "engine/runtime/framework/component/custom/rotate_component.h"
#include "engine/runtime/framework/component/light/light_component.h"
#include "engine/runtime/framework/component/mesh_renderer/mesh_renderer_component.h"
#include "engine/runtime/framework/component/node/node_component.h"
#include "engine/runtime/framework/core/game_object.h"
#include "engine/runtime/framework/core/scene.h"
#include "engine/runtime/framework/core/world.h"

#include "engine/runtime/resource/loader/image_loader.h"
#include "engine/runtime/runtime.h"

#include "glm/fwd.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

using namespace rtr;

const char* vertex_shader_source = R"(

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_uv;
layout(location = 2) in vec3 a_normal;

#ifdef ENABLE_NORMAL_MAP
layout(location = 3) in vec3 a_tangent;
#endif

layout(std140, binding = 0) uniform Camera_ubo {
    mat4 view;
    mat4 projection;
    vec3 camera_position;
    vec3 camera_direction;
    float near;
    float far;
};

layout(std140, binding = 4) uniform Light_camera_ubo {
    mat4 light_view;
    mat4 light_projection;
    vec3 light_camera_position;
    vec3 light_camera_direction;
    float light_near;
    float light_far;
};

uniform mat4 model;

out vec3 v_frag_position;
out vec2 v_uv;
out vec3 v_normal;

#ifdef ENABLE_NORMAL_MAP
out vec3 v_tangent;
out mat3 v_tbn;
#endif

void main() {
    gl_Position = projection * view * model * vec4(a_position, 1.0);
    //gl_Position = light_projection * light_view * model * vec4(a_position, 1.0);

    v_frag_position = vec3(model * vec4(a_position, 1.0));
    v_uv = a_uv;
    v_normal = mat3(transpose(inverse(model))) * a_normal;

#ifdef ENABLE_NORMAL_MAP
    v_tangent = mat3(model) * a_tangent;
    vec3 T = normalize(v_tangent);
    vec3 N = normalize(v_normal);
    vec3 B = normalize(cross(N, T));
    v_tbn = mat3(T, B, N);
#endif

}

)";

const char* fragment_shader_source = R"(
// 输入变量
in vec2 v_uv;
in vec3 v_frag_position;
in vec3 v_normal;

// 输出变量
out vec4 frag_color;

#define MAX_DIRECTIONAL_LIGHT 2
#define MAX_SPOT_LIGHT 8
#define MAX_POINT_LIGHT 8

struct Directional_light {
    float intensity;
    vec3 color;
    vec3 direction;
};

layout(std140, binding = 1) uniform Directional_light_array_ubo {
    int dl_count;
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

// 纹理采样器
#ifdef ENABLE_ALBEDO_MAP
layout(binding = 0) uniform sampler2D albedo_map;
#endif

#ifdef ENABLE_SPECULAR_MAP
layout(binding = 1) uniform sampler2D specular_map;
#endif

#ifdef ENABLE_NORMAL_MAP
in vec3 v_tangent;
in mat3 v_tbn;
layout(binding = 2) uniform sampler2D normal_map;
#endif

#ifdef ENABLE_ALPHA_MAP
layout(binding = 3) uniform sampler2D alpha_map;
#endif

#ifdef ENABLE_HEIGHT_MAP
layout(binding = 4) uniform sampler2D height_map;

uniform float parallax_scale;
uniform float parallax_layer_count;

vec2 parallax_uv(vec2 uv, vec3 view_dir, sampler2D height_map, mat3 tbn, float scale) {
	view_dir = normalize(transpose(tbn) * view_dir);
    view_dir.xy /= -view_dir.z;

    vec2 offset = view_dir.xy * texture(height_map, uv).r * scale;
    return uv + offset;
}

vec2 parallax_steep_uv(vec2 uv, vec3 view_dir, sampler2D height_map, mat3 tbn, float scale, float layer_count) {
	view_dir = normalize(transpose(tbn) * view_dir);
    view_dir.xy /= -view_dir.z;

    float layer_depth = 1.0 / layer_count;
    vec2 delta_uv = view_dir.xy * scale / layer_count;

    vec2 current_uv = uv;
    float current_height = texture(height_map, current_uv).r;
    float current_depth = 0.0;

    while (current_height > current_depth) {
        current_uv += delta_uv;
        current_height = texture(height_map, current_uv).r;
        current_depth += layer_depth;
    }

    return current_uv;
}

vec2 parallax_occlusion_uv(vec2 uv, vec3 view_dir, sampler2D height_map, mat3 tbn, float scale, float layer_count) {
	view_dir = normalize(transpose(tbn) * view_dir);
    view_dir.xy /= -view_dir.z;

    float layer_depth = 1.0 / layer_count;
    vec2 delta_uv = view_dir.xy * scale / layer_count;

    vec2 current_uv = uv;
    float current_height = texture(height_map, current_uv).r;
    float current_depth = 0.0;

    while (current_height > current_depth) {
        current_uv += delta_uv;
        current_height = texture(height_map, current_uv).r;
        current_depth += layer_depth;
    }

    vec2 prev_uv = current_uv - delta_uv;
    float prev_height = texture(height_map, prev_uv).r;
    float prev_depth = current_depth - layer_depth;
    float prev_distance = prev_height - prev_depth;
    float current_distance = current_depth - current_height;

    float weight = current_distance / (current_distance + prev_distance);
    return mix(current_uv, prev_uv, weight);
}
#endif

layout(std140, binding = 0) uniform Camera_ubo {
    mat4 view;
    mat4 projection;
    vec3 camera_position;
    vec3 camera_direction;
    float near;
    float far;
};

layout(std140, binding = 4) uniform Light_camera_ubo {
    mat4 light_view;
    mat4 light_projection;
    vec3 light_camera_position;
    vec3 light_camera_direction;
    float light_near;
    float light_far;
};

out vec4 v_light_camera_clip_space_position;

layout(binding = 5) uniform sampler2D shadow_map;
uniform float shadow_bias;

void main() {

    vec3 view_direction = normalize(camera_position - v_frag_position);

#ifdef ENABLE_HEIGHT_MAP
    vec2 uv = parallax_occlusion_uv(
        v_uv, 
        -view_direction, 
        height_map, 
        v_tbn, 
        parallax_scale,
        parallax_layer_count
    );
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

    vec4 world_position = vec4(v_frag_position, 1.0);
    vec4 light_camera_clip_space_position = light_projection * light_view * world_position;
    vec3 light_camera_ndc = light_camera_clip_space_position.xyz / light_camera_clip_space_position.w;
    vec3 projected_position = light_camera_ndc * 0.5 + 0.5;

    float projected_frag_depth = projected_position.z;
    float shadow_map_depth = texture(shadow_map, projected_position.xy).r;

    float bias = max(0.0005, shadow_bias * (1.0 - dot(normalized_normal, -light_camera_direction)));

    if (projected_frag_depth - bias > shadow_map_depth) {
        frag_color = vec4(vec3(ambient), 1.0);
        return;
    }

    frag_color = vec4(ambient + diffuse + specular, alpha);
}
)";

int main() {

    Engine_runtime_descriptor engine_runtime_descriptor{};
    auto runtime = Engine_runtime::create(engine_runtime_descriptor);
    auto editor = editor::Editor::create(runtime);

    auto world = World::create("world1");
    runtime->world() = world;

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

    auto plane_main_tex = Image_loader::load_from_path(
        Image_format::RGB_ALPHA, 
        "assets/image/grass/grass.jpg"
    );

    auto shader = Shader::create(
        "test", 
        std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> {
            {Shader_type::VERTEX, Shader_code::create(Shader_type::VERTEX, vertex_shader_source)},
            {Shader_type::FRAGMENT, Shader_code::create(Shader_type::FRAGMENT, fragment_shader_source)}
        }, 
        std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> {
            {"model", Uniform_entry<glm::mat4>::create(glm::mat4(1.0))},
            {"shadow_bias", Uniform_entry<float>::create(1.0f)},
            {"transparency", Uniform_entry<float>::create(1.0f)},
            {"ka", Uniform_entry<glm::vec3>::create(glm::vec3(0.1, 0.1, 0.1))},
            {"kd", Uniform_entry<glm::vec3>::create(glm::vec3(0.5, 0.5, 0.5))},
            {"ks", Uniform_entry<glm::vec3>::create(glm::vec3(1.0, 1.0, 1.0))},
            {"shininess", Uniform_entry<float>::create(32.0f)}
        },
        std::unordered_map<Shader_feature, std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>>> {
            {
                Shader_feature::HEIGHT_MAP, 
                std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> {
                    {"parallax_scale", Uniform_entry<float>::create(0.05f)},
                    {"parallax_layer_count", Uniform_entry<float>::create(10.0f)}
                }
            }
        },
        Shader::Shader_feature_set {
        	Shader::get_shader_feature_set(std::vector<Shader_feature> {
                Shader_feature::ALBEDO_MAP,
                Shader_feature::SPECULAR_MAP,
                Shader_feature::NORMAL_MAP,
                Shader_feature::ALPHA_MAP,
                Shader_feature::HEIGHT_MAP
            })
        }
    );

    shader->premake_all_shader_variants();
    shader->link_all_shader_variants(runtime->rhi_device());

    auto material = Test_material::create(shader);
    material->albedo_map = Texture_image::create(main_tex);
    material->height_map = Texture_image::create(height_map);
    material->normal_map = Texture_image::create(normal_map);
    //material->shadow_bias = 0.5f;
    //material->specular_map = Texture_image::create(sp_mask);
    //material->alpha_map = Texture_image::create(alpha_map);
    // material->normal_map = Texture_image::create(normal_map);
    // material->ka = glm::vec3(0.0);
    // material->kd = glm::vec3(0.0);
    // material->ks = glm::vec3(1.5);
    // material->transparency = 0.5;
    //material->shininess = 16.0;

    auto plane_material = Test_material::create(shader);
    plane_material->albedo_map = Texture_image::create(plane_main_tex);
    //plane_material->shadow_bias = 0.5f;

    auto scene = world->add_scene(Scene::create("scene1"));
    world->set_current_scene(scene);

    auto spherical = Skybox::create(Texture_image::create(bk_image));
    scene->set_skybox(spherical);

    // auto cubemap = Skybox::create(Texture_cubemap::create(std::unordered_map<Texture_cubemap_face, std::shared_ptr<Image>>{
    //     {Texture_cubemap_face::BACK, Image_loader::load_from_path(Image_format::RGB_ALPHA, "assets/image/skybox/cubemap/back.jpg", false)},
    //     {Texture_cubemap_face::BOTTOM, Image_loader::load_from_path(Image_format::RGB_ALPHA, "assets/image/skybox/cubemap/bottom.jpg", false)},
    //     {Texture_cubemap_face::FRONT, Image_loader::load_from_path(Image_format::RGB_ALPHA, "assets/image/skybox/cubemap/front.jpg", false)},
    //     {Texture_cubemap_face::LEFT, Image_loader::load_from_path(Image_format::RGB_ALPHA, "assets/image/skybox/cubemap/left.jpg", false)},
    //     {Texture_cubemap_face::RIGHT, Image_loader::load_from_path(Image_format::RGB_ALPHA, "assets/image/skybox/cubemap/right.jpg", false)},
    //     {Texture_cubemap_face::TOP, Image_loader::load_from_path(Image_format::RGB_ALPHA, "assets/image/skybox/cubemap/top.jpg", false)}
    // }));
    // scene->set_skybox(cubemap);

    auto camera_game_object = scene->add_game_object(Game_object::create("camera"));
    auto camera_node = camera_game_object->add_component<Node_component>()->node();
    camera_node->set_position(glm::vec3(0, 0, 5));
    camera_node->look_at_point(glm::vec3(0, 0, 0));

    auto camera_component = camera_game_object->add_component<Perspective_camera_component>();
    camera_component->add_resize_callback(runtime->window_system()->window());
    
    auto camera_control_component = camera_game_object->add_component<Trackball_camera_control_component>();

    auto sphere = scene->add_game_object(Game_object::create("go1"));
    auto sphere_node = sphere->add_component<Node_component>()->node();
    sphere_node->set_position(glm::vec3(-2, 0, 0));

    auto sphere_mesh_renderer = sphere->add_component<Mesh_renderer_component>()->mesh_renderer();
    sphere_mesh_renderer->geometry() = Geometry::create_sphere();
    sphere_mesh_renderer->material() = material;

    auto box = scene->add_game_object(Game_object::create("go2"));
    auto box_node = box->add_component<Node_component>()->node();
    box_node->set_position(glm::vec3(0, 0, 0));

    auto rotate_component = box->add_component<Rotate_component>();
    rotate_component->speed() = 0.1f;

    auto box_mesh_renderer = box->add_component<Mesh_renderer_component>()->mesh_renderer();
    box_mesh_renderer->geometry() = Geometry::create_box();
    box_mesh_renderer->material() = material;

    auto plane = scene->add_game_object(Game_object::create("plane"));
    auto plane_node = plane->add_component<Node_component>()->node();
    plane_node->set_position(glm::vec3(0, -1, 0));
    plane_node->look_at_direction(glm::vec3(0, 1, 0));
    plane_node->set_scale(glm::vec3(5.0));
    auto plane_mesh_renderer = plane->add_component<Mesh_renderer_component>()->mesh_renderer();
    plane_mesh_renderer->geometry() = Geometry::create_plane();
    plane_mesh_renderer->material() = plane_material;

    auto dl_game_object = scene->add_game_object(Game_object::create("dl"));
    auto dl_node = dl_game_object->add_component<Node_component>()->node();
    dl_node->look_at_direction(glm::vec3(1, -1, 1));
    auto dl = dl_game_object->add_component<Directional_light_component>();
    auto dl_camera_component = dl_game_object->add_component<Orthographic_camera_component>();
    dl_camera_component->is_shadow_caster() = true;

    auto dl_camera = dl_camera_component->orthographic_camera();
    dl_camera->set_orthographic_size(15.0f);

    auto pl0_game_object = scene->add_game_object(Game_object::create("pl0"));
    auto pl0_node = pl0_game_object->add_component<Node_component>()->node();
    pl0_node->set_position(glm::vec3(1, 0, 0));
    auto pl0 = pl0_game_object->add_component<Point_light_component>()->point_light();
    pl0->color() = glm::vec3(0, 1, 0);

    auto pl1_game_object = scene->add_game_object(Game_object::create("pl1"));
    auto pl1_node = pl1_game_object->add_component<Node_component>()->node();
    pl1_node->set_position(glm::vec3(-1, 0, 0));
    auto pl1 = pl1_game_object->add_component<Point_light_component>()->point_light();
    pl1->color() = glm::vec3(0, 0, 1);

    auto sl0_game_object = scene->add_game_object(Game_object::create("sl0"));
    auto sl0_node = sl0_game_object->add_component<Node_component>()->node();
    sl0_node->set_position(glm::vec3(0, 0, 1.0));
    sl0_node->look_at_direction(glm::vec3(0, 0, -1));
    auto sl0 = sl0_game_object->add_component<Spot_light_component>()->spot_light();
    sl0->inner_angle() = 15.0f;
    sl0->outer_angle() = 20.0f;
    sl0->color() = glm::vec3(1, 1, 0);
    sl0->intensity() = 0.5f;

    auto sl1_game_object = scene->add_game_object(Game_object::create("sl1"));
    auto sl1_node = sl1_game_object->add_component<Node_component>()->node();
    sl1_node->set_position(glm::vec3(0.0, 1.0, 0.0));
    sl1_node->look_at_direction(glm::vec3(0.0, -1.0, 0.0));
    auto sl1 = sl1_game_object->add_component<Spot_light_component>()->spot_light();
    sl1->color() = glm::vec3(1, 1, 0);
    sl1->intensity() = 0.5f;

    
    box_node->add_child(sphere_node, true);
    //box_node->add_child(camera_node, true);
    //box_node->add_child(sl0_node, true);
    gl_check_error();
    
    editor->run();

    return 0;
}