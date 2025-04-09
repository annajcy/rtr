

#include "engine/runtime/global/base.h" 
#include "engine/runtime/core/geometry.h"
#include "engine/runtime/core/input.h"
#include "engine/runtime/core/camera.h"
#include "engine/runtime/core/geometry.h"
#include "engine/runtime/core/light.h"
#include "engine/runtime/platform/rhi/opengl/rhi_device_opengl.h"

#include "engine/runtime/resource/loader/image_loader.h"


using namespace std;
using namespace rtr;

// Vertex Shader source code
const char* vertex_shader_source = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
    
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
    
void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    Normal = mat3(transpose(inverse(model))) * aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0));
}
)";
    
// Fragment Shader source code
const char* fragment_shader_source = R"(
#version 460 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

struct Material {
    vec3 ka;  
    vec3 kd; 
    vec3 ks; 
    float shininess;
}; 

uniform Material material;
uniform sampler2D mainTexture;

struct Light {
    int type;        // 0=ambient, 1=directional, 2=point, 3=spot
    vec3 color;
    float intensity;
    vec3 position;   
    vec3 direction;  
    vec3 attenuation; // x=kc, y=k1, z=k2
    float innerAngle;
    float outerAngle;
};

#define MAX_LIGHTS 16
uniform Light lights[MAX_LIGHTS];
uniform int activeLights;
uniform int mainLightIndex;

uniform vec3 cameraPosition;

vec3 diffuseComponent(vec3 lightDir, vec3 normal, vec3 lightColor, vec3 objectColor, float intensity) {
    float diff = max(dot(normal, lightDir), 0.0);
    return diff * lightColor * objectColor * intensity;
}

vec3 specularComponent(vec3 lightDir, vec3 normal, vec3 viewDir, vec3 lightColor, vec3 objectColor, float shininess, float intensity) {
	vec3 H = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, H), 0.0), shininess);
    return spec * lightColor * objectColor * intensity;
}

vec3 ambientComponent(vec3 lightColor, vec3 objectColor, float intensity) {
    return lightColor * objectColor * intensity;
}

vec3 calculateAmbientLight(Light light, Material mat, vec3 normal, vec3 cameraPos, vec3 fragPos, vec3 objectColor) {
    return mat.ka * ambientComponent(light.color, objectColor, light.intensity);
}

vec3 calculateDirectionalLight(Light light, Material mat, vec3 normal, vec3 cameraPos, vec3 fragPos, vec3 objectColor) {
    vec3 lightDir = normalize(-light.direction);
    vec3 viewDir = normalize(cameraPos - fragPos);
    vec3 diffuse = mat.kd * diffuseComponent(lightDir, normal, light.color, objectColor, light.intensity);
    vec3 specular = mat.ks * specularComponent(lightDir, normal, viewDir, light.color, objectColor, mat.shininess, light.intensity);
    return diffuse + specular;
}

vec3 calculatePointLight(Light light, Material mat, vec3 normal, vec3 cameraPos, vec3 fragPos, vec3 objectColor) {
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 viewDir = normalize(cameraPos - fragPos);
    float kc = light.attenuation.x; 
    float k1 = light.attenuation.y;
    float k2 = light.attenuation.z;
    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (kc + k1 * dist + k2 * dist * dist);
    vec3 diffuse = mat.kd * diffuseComponent(lightDir, normal, light.color, objectColor, light.intensity);
    vec3 specular = mat.ks * specularComponent(lightDir, normal, viewDir, light.color, objectColor, mat.shininess, light.intensity);
    return (diffuse + specular) * attenuation;
}

vec3 calculateSpotLight(Light light, Material mat, vec3 normal, vec3 cameraPos, vec3 fragPos, vec3 objectColor) {
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 targetDir = normalize(-light.direction);
    vec3 viewDir = normalize(cameraPos - fragPos);
    vec3 diffuse = mat.kd * diffuseComponent(lightDir, normal, light.color, objectColor, light.intensity);
    vec3 specular = mat.ks * specularComponent(lightDir, normal, viewDir, light.color, objectColor, mat.shininess, light.intensity);
    float theta = dot(targetDir, lightDir);
    float attenuation = clamp((theta - light.outerAngle) / (light.innerAngle - light.outerAngle), 0.0, 1.0);
    return (diffuse + specular) * attenuation;
}
 
void main()
{
    vec4 texColor = texture(mainTexture, TexCoord);
    vec3 objectColor = texColor.rgb;
    vec3 N = normalize(Normal);

    vec3 result = vec3(0.0);

    for (int i = 0; i < activeLights; i++) {
        Light light = lights[i];

        if (i == mainLightIndex) {
            objectColor *= 1.2; // 主光源照射区域增加20%亮度
        }

        if (light.type == 0) {
            result += calculateAmbientLight(light, material, N, cameraPosition, FragPos, objectColor);
        } else if (light.type == 1) {
            result += calculateDirectionalLight(light, material, N, cameraPosition, FragPos, objectColor);
        } else if (light.type == 2) {
            result += calculatePointLight(light, material, N, cameraPosition, FragPos, objectColor);
        } else if (light.type == 3) {
            result += calculateSpotLight(light, material, N, cameraPosition, FragPos, objectColor);
        }
    }

    FragColor = vec4(result, 1.0);
}

)";

int main() {

    auto device = RHI_device_OpenGL::create();
    
    auto window = device->create_window(1280, 720, "RTR");
    auto imgui = device->create_imgui(window);
    auto input = Input::create(window);

    auto screen_frame_buffer = device->create_screen_frame_buffer(window);
    
    auto sphere = Geometry::create_sphere(0.5);

    auto sphere_position = sphere->attribute("position");
    auto sphere_tex_coord = sphere->attribute("uv");
    auto sphere_normal = sphere->attribute("normal");
    auto sphere_indices = sphere->element_attribute();
    
    auto sphere_element_buffer = device->create_element_buffer(
        Buffer_usage::STATIC,
        sphere_indices->data_count(),
        sphere_indices->data_size(),
        sphere_indices->data_ptr()
    );

    // 创建位置缓冲区
    auto sphere_position_buffer = device->create_vertex_buffer(
        Buffer_usage::STATIC,
        Buffer_data_type::FLOAT,
        Buffer_iterate_type::PER_VERTEX,
        sphere_position->unit_data_count(),  // 每个顶点3个分量
        sphere_position->data_size(),
        sphere_position->data_ptr()
    );

    // 创建独立的纹理坐标缓冲区
    auto sphere_tex_coord_buffer = device->create_vertex_buffer(
        Buffer_usage::STATIC,
        Buffer_data_type::FLOAT,
        Buffer_iterate_type::PER_VERTEX,
        sphere_tex_coord->unit_data_count(),  // 每个顶点2个分量
        sphere_tex_coord->data_size(),
        sphere_tex_coord->data_ptr()
    );
    
    auto sphere_normal_buffer = device->create_vertex_buffer(
        Buffer_usage::STATIC,
        Buffer_data_type::FLOAT,
        Buffer_iterate_type::PER_VERTEX,
        sphere_normal->unit_data_count(),  // 每个顶点3个分量
        sphere_normal->data_size(),
        sphere_normal->data_ptr()
    );

    // 修改几何体绑定（需要对应修改vertex shader的location）
    auto box_geometry = device->create_geometry(
        std::unordered_map<unsigned int, RHI_buffer::Ptr>{
            {0, sphere_position_buffer},  // location 0 绑定位置数据
            {1, sphere_tex_coord_buffer},    // location 1 绑定纹理坐标
            {2, sphere_normal_buffer}
        },
        sphere_element_buffer
    );
    
    auto earth_image = Image_loader::load_from_path(
        Image_format::RGB_ALPHA, 
        "assets/image/earth.png"
    );

    auto earth_texture = device->create_texture_2D(
        earth_image->width(), 
        earth_image->height(), 
        4, 
        Texture_internal_format::SRGB_ALPHA,
        std::unordered_map<Texture_wrap_target, Texture_wrap>{
            {Texture_wrap_target::U, Texture_wrap::REPEAT},
            {Texture_wrap_target::V, Texture_wrap::REPEAT}
        },
        std::unordered_map<Texture_filter_target, Texture_filter>{
            {Texture_filter_target::MIN, Texture_filter::LINEAR_MIPMAP_LINEAR},
            {Texture_filter_target::MAG, Texture_filter::LINEAR}
        },
        Image_data{
            earth_image->width(),
            earth_image->height(),
            earth_image->data_ptr(),
            Texture_buffer_type::UNSIGNED_BYTE,
            Texture_external_format::RGB_ALPHA
        }
    );

    auto default_image = Image_loader::load_from_path(
        Image_format::RGB_ALPHA,
        "assets/image/default_texture.jpg"
    );

    earth_texture->generate_mipmap();
    earth_texture->bind_to_unit(1);

    auto default_texture = device->create_texture_2D(
        default_image->width(),
        default_image->height(),
        4,
        Texture_internal_format::SRGB_ALPHA,
        std::unordered_map<Texture_wrap_target, Texture_wrap>{
            {Texture_wrap_target::U, Texture_wrap::REPEAT},
            {Texture_wrap_target::V, Texture_wrap::REPEAT}
        },
        std::unordered_map<Texture_filter_target, Texture_filter>{
            {Texture_filter_target::MIN, Texture_filter::LINEAR_MIPMAP_LINEAR},
            {Texture_filter_target::MAG, Texture_filter::LINEAR}
        },
        Image_data{
            default_image->width(),
            default_image->height(),
            default_image->data_ptr(),
            Texture_buffer_type::UNSIGNED_BYTE,
            Texture_external_format::RGB_ALPHA
        }
    );

    default_texture->generate_mipmap();
    default_texture->bind_to_unit(0);

    auto directional_light = std::make_shared<Directional_light>(); 
	directional_light->look_at_direction(glm::vec3(0.0, -1.0, 0.0));
	directional_light->intensity() = 1.0f;

	auto ambient_light = std::make_shared<Ambient_light>();
	ambient_light->intensity() = 0.5f;

	auto spot_light = std::make_shared<Spot_light>();
	spot_light->position() = glm::vec3(0.0f, 0.0f, 2.0f);
	spot_light->look_at_direction(glm::vec3(0.0, 0.0f, -1.0f));
	spot_light->inner_angle() = 5.0f;
	spot_light->outer_angle() = 10.0f;
    spot_light->intensity() = 5.0f;
    spot_light->color() = glm::vec3(1.0, 1.0, 0.0);

	auto point_light_1 = std::make_shared<Point_light>();
	point_light_1->position() = glm::vec3(2.0f, 0.0f, 0.0f);
    point_light_1->intensity() = 5.0f;
    point_light_1->color() = glm::vec3(1.0, 1.0, 1.0);

	auto point_light_2 = std::make_shared<Point_light>();
	point_light_2->position() = glm::vec3(-2.0f, 1.0f, 0.0f);
    point_light_2->intensity() = 5.0f;
    point_light_2->color() = glm::vec3(1.0, 1.0, 1.0);

	auto point_light_3 = std::make_shared<Point_light>();
	point_light_3->position() = glm::vec3(0.0f, -1.0f, 0.0f);
    point_light_3->intensity() = 5.0f;
    point_light_3->color() = glm::vec3(0.0, 0.0, 1.0);

    auto light_setting = std::make_shared<Light_setting>(
        std::vector<Light::Ptr>{
            directional_light,
            ambient_light,
            spot_light,
            point_light_1,
            point_light_2,
            point_light_3
        }
    );

    auto camera = Perspective_camera::create(
        60.0f, 
        (float)window->width() / (float)window->height(), 
        0.1f, 
        50.0f
    );

    camera->position() = glm::vec3(0.0f, 0.0f, 3.0f);
    camera->look_at_point(glm::vec3(0.0f, 0.0f, 0.0f));

    auto camera_control = Trackball_camera_control::create(camera, input);

    auto vertex_shader_code = device->create_shader_code(Shader_type::VERTEX, vertex_shader_source);
    auto fragment_shader_code = device->create_shader_code(Shader_type::FRAGMENT, fragment_shader_source);

    auto shader_program = device->create_shader_program(
        std::unordered_map<Shader_type, RHI_shader_code::Ptr>{
            {Shader_type::VERTEX, vertex_shader_code},
            {Shader_type::FRAGMENT, fragment_shader_code}
        },  
        [&](){
            
            std::unordered_map<std::string, RHI_uniform_entry_base::Ptr> uniforms{
                {"model", RHI_uniform_entry<glm::mat4>::create( glm::mat4(1.0f))},
                {"view", RHI_uniform_entry<glm::mat4>::create(glm::mat4(1.0f))},
                {"projection", RHI_uniform_entry<glm::mat4>::create(glm::mat4(1.0f))},
                {"cameraPosition", RHI_uniform_entry<glm::vec3>::create(glm::vec3(0.0f))},
                {"mainTexture", RHI_uniform_entry<int>::create(1)},
                {"activeLights", RHI_uniform_entry<int>::create(0)},
                {"mainLightIndex", RHI_uniform_entry<int>::create(5)},
                {"material.ka", RHI_uniform_entry<glm::vec3>::create(glm::vec3(0.2f))},
                {"material.kd", RHI_uniform_entry<glm::vec3>::create(glm::vec3(0.5f))},
                {"material.ks", RHI_uniform_entry<glm::vec3>::create(glm::vec3(0.2f))},
                {"material.shininess", RHI_uniform_entry<float>::create(32.0f)}
            };
    
            for (int i = 0; i < 16; ++ i) {
                std::string prefix = "lights[" + std::to_string(i) + "]";
                uniforms[prefix + ".type"] = RHI_uniform_entry<int>::create(0);
                uniforms[prefix + ".color"] = RHI_uniform_entry<glm::vec3>::create(glm::vec3(0.0f));
                uniforms[prefix + ".intensity"] = RHI_uniform_entry<float>::create(0.0f);
                uniforms[prefix + ".position"] = RHI_uniform_entry<glm::vec3>::create(glm::vec3(0.0f));
                uniforms[prefix + ".direction"] = RHI_uniform_entry<glm::vec3>::create(glm::vec3(0.0f));
                uniforms[prefix + ".attenuation"] = RHI_uniform_entry<glm::vec3>::create(glm::vec3(0.0f));
                uniforms[prefix + ".innerAngle"] = RHI_uniform_entry<float>::create(0.0f);
                uniforms[prefix + ".outerAngle"] = RHI_uniform_entry<float>::create(0.0f);
            }

            return uniforms;
        }()
    );

    auto pipeline_state = device->create_pipeline_state(Pipeline_state::opaque_pipeline_state());
    pipeline_state->apply();

    Clear_state clear_state = Clear_state::enabled();
    clear_state.color_clear_value = glm::vec4(0.1, 0.5, 0.3, 1.0);
    auto renderer = device->create_renderer(clear_state);

    glm::mat4 model = glm::mat4(1.0f);

    while (window->is_active()) {
        window->on_frame_begin();
        camera_control->update();

        model = glm::rotate(model, 0.01f, glm::vec3(0.5f, 1.0f, 0.0f));

        shader_program->modify_uniform<glm::mat4>("model", model);
        shader_program->modify_uniform<glm::mat4>("view", camera->view_matrix());
        shader_program->modify_uniform<glm::mat4>("projection", camera->projection_matrix());
        shader_program->modify_uniform<glm::vec3>("cameraPosition", camera->position());
        shader_program->modify_uniform<int>("activeLights", light_setting->active_light_count());
        shader_program->modify_uniform<int>("mainLightIndex", light_setting->main_light_index());
        
        // 遍历所有灯光
        for (size_t i = 0; i < light_setting->active_light_count(); ++i) {
            const auto& light = light_setting->lights()[i];
            std::string prefix = "lights[" + std::to_string(i) + "]";

            // 设置通用属性
            shader_program->modify_uniform<int>((prefix + ".type").c_str(), static_cast<int>(light->type()));
            shader_program->modify_uniform<glm::vec3>((prefix + ".color").c_str(), light->color());
            shader_program->modify_uniform<float>((prefix + ".intensity").c_str(), light->intensity());
            
            // 设置方向光属性示例
            if (auto dir_light = std::dynamic_pointer_cast<Directional_light>(light)) {
                shader_program->modify_uniform<glm::vec3>((prefix + ".direction").c_str(), dir_light->front());
            }
            
            // 设置点光源属性示例
            if (auto point_light = std::dynamic_pointer_cast<Point_light>(light)) {
                shader_program->modify_uniform<glm::vec3>((prefix + ".position").c_str(), point_light->position());
                shader_program->modify_uniform<glm::vec3>((prefix + ".attenuation").c_str(), glm::vec3(point_light->kc(), point_light->k1(), point_light->k2()));
            }
            // 设置聚光灯属性示例
            if (auto spot_light = std::dynamic_pointer_cast<Spot_light>(light)) {
                shader_program->modify_uniform<glm::vec3>((prefix + ".position").c_str(), spot_light->position());
                shader_program->modify_uniform<glm::vec3>((prefix + ".direction").c_str(), spot_light->front());
                shader_program->modify_uniform<float>((prefix + ".innerAngle").c_str(), spot_light->inner_angle_cos());
                shader_program->modify_uniform<float>((prefix + ".outerAngle").c_str(), spot_light->outer_angle_cos());
            }
        	
        }

        shader_program->update_uniforms();
        renderer->init();

        renderer->clear(screen_frame_buffer);
        renderer->draw(
            shader_program,
            box_geometry,
            screen_frame_buffer
        );

        device->check_error();
        window->on_frame_end();

        imgui->begin_frame();
        imgui->begin_render("Light Setting");
        imgui->color_edit("Ambient Light Color", glm::value_ptr(ambient_light->color()));
        imgui->color_edit("Directional Light Color", glm::value_ptr(directional_light->color()));
        imgui->color_edit("Spot Light Color", glm::value_ptr(spot_light->color()));
        imgui->color_edit("Point Light 1 Color", glm::value_ptr(point_light_1->color()));
        imgui->color_edit("Point Light 2 Color", glm::value_ptr(point_light_2->color()));
        imgui->color_edit("Point Light 3 Color", glm::value_ptr(point_light_3->color()));
        imgui->text("Editor frame rate", std::to_string(imgui->frame_rate()));
        imgui->end_render();
        imgui->end_frame();
    }
    
    return 0;
}



