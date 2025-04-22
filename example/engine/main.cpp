#include "engine/editor/editor.h"
#include "engine/runtime/core/material.h"
#include "engine/runtime/core/memory_buffer.h"
#include "engine/runtime/core/shader.h"
#include "engine/runtime/core/texture.h"
#include "engine/runtime/core/skybox.h"

#include "engine/runtime/framework/component/camera/camera_component.h"
#include "engine/runtime/framework/component/camera/camera_control_component.h"
#include "engine/runtime/framework/component/custom/rotate_component.h"
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

layout(std140, binding = 0) uniform Camera {
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

    auto spherical = Skybox::create(Texture_2D::create(bk_image));
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
                {"model", Uniform_entry<glm::mat4>::create(glm::mat4(1.0))}
            }), 
        Shader::get_shader_feature_set(std::vector<Shader_feature> {
            Shader_feature::ALBEDO_MAP
        })
    );

    shader->premake_shader_variants();
    shader->link_all_shader_variants(runtime->rhi_device());

    auto material = Test_material::create(shader);
    material->albedo_map = std::make_shared<Texture_2D>(image);

    auto game_object = scene->add_game_object(Game_object::create("go1"));
    auto node = game_object->add_component<Node_component>();
    node->set_position(glm::vec3(-1, 0, 0));

    auto mesh_renderer = game_object->add_component<Mesh_renderer_component>();
    mesh_renderer->set_geometry(Geometry::create_sphere());
    mesh_renderer->set_material(material);

    auto game_object2 = scene->add_game_object(Game_object::create("go2"));
    auto node2 = game_object2->add_component<Node_component>();
    node2->set_position(glm::vec3(1, 0, 0));

    auto rotate_component = game_object2->add_component<Rotate_component>();
    rotate_component->speed() = 0.1f;

    auto mesh_renderer2 = game_object2->add_component<Mesh_renderer_component>();
    mesh_renderer2->set_geometry(Geometry::create_box());
    mesh_renderer2->set_material(material);

    node2->add_child(node, true);
    
    runtime->world() = world;

    auto editor = editor::Editor::create(runtime);
    editor->run();

    return 0;
}