#include "engine/global/base.h"

#include "engine/runtime/resource/loader/resource_loader.h"
#include "engine/runtime/rhi/rhi_device.h"
#include "engine/runtime/rhi/opengl/rhi_device_opengl.h"

#include "engine/runtime/core/renderer.h"

using namespace std;
using namespace rtr;

// Vertex Shader source code
const char* vertex_shader_source = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
void main()
{
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";
    
// Fragment Shader source code
const char* fragment_shader_source = R"(
#version 460 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture_sampler;

void main()
{
    FragColor = vec4(texture(texture_sampler, TexCoord).rgb, 1.0);
}
)";


const char* fragmentShaderSource1 = R"(
   #version 460 core
out vec4 frag_color;

in vec3 world_position;
in vec2 uv;
in vec3 normal;

uniform float time;
uniform sampler2D main_tex;

uniform vec3 camera_position;

struct Material {
    vec3 ambient;  // 原为 ka，需要与 C++ 代码中的 uniform 名称对应
    vec3 diffuse;  // 原为 kd
    vec3 specular; // 原为 ks
    float shininess;
}; // 添加分号

uniform Material material;

#define MAX_LIGHTS 16
uniform int active_lights;
uniform int main_light_index;

struct Light {
    int type;        // 0=ambient, 1=directional, 2=point, 3=spot
    vec3 color;
    float intensity;
    vec3 position;   // 点/聚光灯
    vec3 direction;  // 平行/聚光灯
    vec3 attenuation; // x=kc, y=k1, z=k2
    float inner_angle;
    float outer_angle;
};

uniform Light lights[MAX_LIGHTS];

// 公共光照计算函数
vec3 diffuse_component(vec3 light_dir, vec3 normal, vec3 light_color, vec3 object_color, float intensity) {
    float diff = max(dot(normal, light_dir), 0.0);
    return light_color * diff * object_color * intensity;
}

vec3 specular_component(vec3 light_dir, vec3 normal, vec3 view_dir, vec3 light_color, float shininess, float intensity) {
    // 改用半角向量计算高光
    vec3 H = normalize(light_dir + view_dir);
    float spec = pow(max(dot(normal, H), 0.0), shininess);
    return light_color * spec * intensity;
}

// 环境光处理
// 修改 ambient_light 函数
vec3 ambient_light(Light light, Material mat, vec3 object_color) {
    return light.color * light.intensity * mat.ambient * object_color; // ka → ambient
}

// 修改 directional_light 函数
vec3 directional_light(Light light, Material mat, vec3 normal, vec3 view_dir, vec3 object_color) {
    vec3 light_dir = normalize(-light.direction);
    vec3 diffuse = mat.diffuse * diffuse_component(light_dir, normal, light.color, object_color, light.intensity); // kd → diffuse
    vec3 specular = mat.specular * specular_component(light_dir, normal, view_dir, light.color, mat.shininess, light.intensity); // ks → specular
    return diffuse + specular;
}

// 修改 point_light 函数
vec3 point_light(Light light, Material mat, vec3 normal, vec3 view_dir, vec3 world_pos, vec3 object_color) {  // 添加object_color参数
    vec3 light_dir = normalize(light.position - world_pos);
    float dist = length(light.position - world_pos);
    float attenuation = 1.0 / (light.attenuation.z * dist*dist + 
                              light.attenuation.y * dist + 
                              light.attenuation.x);
    
    vec3 diffuse = mat.diffuse * diffuse_component(light_dir, normal, light.color, object_color, light.intensity);
    vec3 specular = mat.specular * specular_component(light_dir, normal, view_dir, light.color, mat.shininess, light.intensity);
    return (diffuse + specular) * attenuation;
}

// 修改 spot_light 函数
vec3 spot_light(Light light, Material mat, vec3 normal, vec3 view_dir, vec3 world_pos, vec3 object_color) {  // 添加object_color参数
    vec3 light_dir = normalize(light.position - world_pos);
    float theta = dot(light_dir, normalize(-light.direction));
    float epsilon = light.inner_angle - light.outer_angle;
    float intensity = clamp((theta - light.outer_angle) / epsilon, 0.0, 1.0);
    
    float dist = length(light.position - world_pos);
    float attenuation = 1.0 / (light.attenuation.z * dist*dist + 
                              light.attenuation.y * dist + 
                              light.attenuation.x);
    
    vec3 diffuse = mat.diffuse * diffuse_component(light_dir, normal, light.color, object_color, light.intensity);
    vec3 specular = mat.specular * specular_component(light_dir, normal, view_dir, light.color, mat.shininess, light.intensity);
    return (diffuse + specular) * attenuation * intensity;
}

vec3 calculate_lighting(Light light, Material mat, vec3 normal, vec3 view_dir, vec3 world_pos, vec3 object_color) {
    switch(light.type) {
        case 0: return ambient_light(light, mat, object_color);
        case 1: return directional_light(light, mat, normal, view_dir, object_color);
        case 2: return point_light(light, mat, normal, view_dir, world_pos, object_color);  // 传递object_color
        case 3: return spot_light(light, mat, normal, view_dir, world_pos, object_color);    // 传递object_color
    }
    return vec3(0.0);
}

void main() {
    vec3 object_color = texture(main_tex, uv).rgb;
    vec3 N = normalize(normal);
    vec3 V = normalize(camera_position - world_position);

    vec3 result = vec3(0.0);
    for (int i = 0; i < active_lights; ++i) {
        if(i == main_light_index) {
            continue;
        }
        result += calculate_lighting(lights[i], material, N, V, world_position, object_color);
    }
    
    if (active_lights > 0) {
        result += calculate_lighting(lights[main_light_index], material, N, V, world_position, object_color);
    }

    frag_color = vec4(result, 1.0);
}
    )";


std::vector<float> vertices = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f,  0.5f, 0.0f
};

std::vector<float> texture_coords = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.5f, 1.0f
};

std::vector<unsigned int> indices = {
    0, 1, 2
};    

std::shared_ptr<Image> image{};

int main() {

    Renderer rd();

    // auto image_loader = std::make_shared<Image_loader_stb>();
    // image = image_loader->load_from_path(Image_format::RGB_ALPHA, "assets/image/default_texture.jpg");
    
    // RHI_device_descriptor descriptor{};

    // descriptor.width = 800;
    // descriptor.height = 600;
    // descriptor.title = "RTR Engine";

    // auto device = std::make_shared<RHI_device_OpenGL>(descriptor);
    // //prepare geometry

    // auto vertex_attribute = device->create_vertex_buffer(
    //     Buffer_usage::STATIC, 
    //     Buffer_attribute_type::FLOAT, 
    //     Buffer_iterate_type::PER_VERTEX,
    //     3, 
    //     vertices.size(), 
    //     vertices.data()
    // );

    // auto texture_attribute = device->create_vertex_buffer(
    //     Buffer_usage::STATIC,
    //     Buffer_attribute_type::FLOAT,
    //     Buffer_iterate_type::PER_VERTEX,
    //     2,
    //     texture_coords.size(),
    //     texture_coords.data()
    // );

    // auto element_buffer = device->create_element_buffer(
    //     Buffer_usage::STATIC,
    //     indices.size(),
    //     indices.data()
    // );
    
    // std::unordered_map<unsigned int, std::shared_ptr<RHI_vertex_buffer>> vertex_buffers = {
    //     {0, vertex_attribute},
    //     {1, texture_attribute}
    // };

    // auto geometry = device->create_geometry(vertex_buffers, element_buffer);

    // //prepare shaders

    // auto vertex_shader = device->create_shader_code(
    //     Shader_type::VERTEX,
    //     vertexShaderSource
    // );

    // auto fragment_shader = device->create_shader_code(
    //     Shader_type::FRAGMENT,
    //     fragmentShaderSource
    // );

    // std::unordered_map<Shader_type, std::shared_ptr<RHI_shader_code>> shaders = {
    //     {Shader_type::VERTEX, vertex_shader},
    //     {Shader_type::FRAGMENT, fragment_shader}
    // };

    // int texture_location = 3;

    // std::unordered_map<std::string, RHI_uniform_entry> uniforms = {
    //     {"texture_sampler", {Uniform_type::SAMPLER, &texture_location}}
    // };

    // auto shader_program = device->create_shader_program(shaders);
    // shader_program->set_uniforms(uniforms);
    
    // //prepare textures
    // auto texture = device->create_texture_2D(
    //     image->width(),
    //     image->height(),
    //     image->data()
    // );

    // // texture->set_filter(Texture_filter::LINEAR, Texture_filter_target::MIN);
    // // texture->set_filter(Texture_filter::LINEAR_MIPMAP_LINEAR, Texture_filter_target::MAG);
    // // texture->set_wrap(Texture_wrap::CLAMP_TO_EDGE, Texture_wrap_target::U);
    // // texture->set_wrap(Texture_wrap::CLAMP_TO_EDGE, Texture_wrap_target::V);


    // texture->generate_mipmap();

    // //prepare binding state

    // std::unordered_map<unsigned int, std::shared_ptr<RHI_texture_2D>> textures = {
    //     {3, texture}
    // };

    // device->binding_state()->geometry() = geometry;
    // device->binding_state()->shader_program() = shader_program;
    // device->binding_state()->textures_2D() = textures;  

    // //prepare input
    // auto input = std::make_shared<Input>(device->window());

    // while (device->window()->is_active()) {
    //     device->window()->on_frame_begin();
    //     device->clear();
    //     device->draw();

    //     // std::cout << input->mouse_x() << " " << input->mouse_y() << std::endl;
    //     // std::cout << input->mouse_dx() << " " << input->mouse_dy() << std::endl;
    //     // std::cout << input->mouse_scroll_dx() << " " << input->mouse_scroll_dy() << std::endl;

    //     device->window()->on_frame_end();
    // }
    
    return 0;
}

