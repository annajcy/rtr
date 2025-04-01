#include "engine/global/base.h"
#include "engine/runtime/core/camera.h"
#include "engine/runtime/core/geometry.h"
#include "engine/runtime/core/light.h"
#include "engine/runtime/enum.h"
#include "engine/runtime/resource/loader/image_loader.h"
#include "engine/runtime/rhi/rhi_device.h"
#include "engine/runtime/rhi/opengl/rhi_device_opengl.h"
#include "engine/runtime/rhi/rhi_geometry.h"
#include "engine/runtime/rhi/rhi_pipeline_state.h"
#include "engine/runtime/rhi/rhi_resource.h"
#include "engine/runtime/rhi/rhi_shader_program.h"
#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "engine/runtime/core/input.h"
#include <memory>

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

uniform vec3 cameraPosition;

struct Material {
    vec3 ambient;  // 原为 ka，需要与 C++ 代码中的 uniform 名称对应
    vec3 diffuse;  // 原为 kd
    vec3 specular; // 原为 ks
    float shininess;
}; // 添加分号

uniform Material material;

#define MAX_LIGHTS 16
uniform int activeLights;
uniform int mainLightIndex;

struct Light {
    int type;        // 0=ambient, 1=directional, 2=point, 3=spot
    vec3 color;
    float intensity;
    vec3 position;   // 点/聚光灯
    vec3 direction;  // 平行/聚光灯
    vec3 attenuation; // x=kc, y=k1, z=k2
    float innerAngle;
    float outerAngle;
};

uniform Light lights[MAX_LIGHTS];

uniform sampler2D mainTexture;

void main()
{
    vec4 texColor = texture(mainTexture, TexCoord);
    FragColor = texColor;
}
)";

int main() {

    auto device = RHI_device_OpenGL::create();
    auto window = device->create_window(800, 600, "RTR");
    auto input = std::make_shared<Input>(window);

    auto geo = Geometry::create_sphere(0.5f);

    auto geo_position = geo->attribute("position");
    auto geo_tex_coord = geo->attribute("uv");
    auto geo_normal = geo->attribute("normal");
    auto geo_indices = geo->element_attribute();
    
    auto element = device->create_element_buffer(
        Buffer_usage::STATIC,
        geo_indices->data_count(),
        geo_indices->data_size(),
        geo_indices->data_ptr()
    );

    // 创建位置缓冲区
    auto position = device->create_vertex_buffer(
        Buffer_usage::STATIC,
        Buffer_data_type::FLOAT,
        Buffer_iterate_type::PER_VERTEX,
        geo_position->unit_data_count(),  // 每个顶点3个分量
        geo_position->data_size(),
        geo_position->data_ptr()
    );

    // 创建独立的纹理坐标缓冲区
    auto tex_coord = device->create_vertex_buffer(
        Buffer_usage::STATIC,
        Buffer_data_type::FLOAT,
        Buffer_iterate_type::PER_VERTEX,
        geo_tex_coord->unit_data_count(),  // 每个顶点2个分量
        geo_tex_coord->data_size(),
        geo_tex_coord->data_ptr()
    );
    
    auto normal = device->create_vertex_buffer(
        Buffer_usage::STATIC,
        Buffer_data_type::FLOAT,
        Buffer_iterate_type::PER_VERTEX,
        geo_normal->unit_data_count(),  // 每个顶点3个分量
        geo_normal->data_size(),
        geo_normal->data_ptr()
    );

    // 修改几何体绑定（需要对应修改vertex shader的location）
    auto box_geometry = device->create_geometry(
        std::unordered_map<unsigned int, RHI_buffer::Ptr>{
            {0, position},  // location 0 绑定位置数据
            {1, tex_coord},    // location 1 绑定纹理坐标
            {2, normal}
        },
        element
    );
    
    auto image = Image_loader::load_from_path(
        Image_format::RGB_ALPHA, 
        "assets/image/earth.png"
    );

    auto texture = device->create_texture_2D(
        image->width(), 
        image->height(), 
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
            image->width(),
            image->height(),
            image->data(),
            Texture_buffer_type::UNSIGNED_BYTE,
            Texture_external_format::RGB_ALPHA
        }
    );

    texture->generate_mipmap();
    texture->bind_to_unit(1);

    auto directional_light = std::make_shared<Directional_light>(); 
	directional_light->look_at_direction(glm::vec3(1.0, -1.0, -1.0));
	directional_light->intensity() = 0.5f;

	auto ambient_light = std::make_shared<Ambient_light>();
	ambient_light->intensity() = 0.15f;
	
	auto spot_light = std::make_shared<Spot_light>();
	spot_light->position() = glm::vec3(0.0f, 0.0f, 2.0f);
	spot_light->look_at_direction(glm::vec3(0.0, 0.0f, -1.0f));
	spot_light->inner_angle() = 5.0f;
	spot_light->outer_angle() = 10.0f;
	spot_light->color() = glm::vec3(1.0, 1.0, 0.0);

	auto point_light_1 = std::make_shared<Point_light>();
	point_light_1->position() = glm::vec3(1.0f, 0.0f, 0.0f);
	point_light_1->color() = glm::vec3(1.0f, 0.0f, 0.0f);

	auto point_light_2 = std::make_shared<Point_light>();
	point_light_2->position() = glm::vec3(0.0f, 1.0f, 0.0f);
	point_light_2->color() = glm::vec3(0.0f, 1.0f, 0.0f);

	auto point_light_3 = std::make_shared<Point_light>();
	point_light_3->position() = glm::vec3(0.0f, 0.0f, -1.0f);
	point_light_3->color() = glm::vec3(0.0f, 0.0f, 1.0f);

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

    glm::mat4 model = glm::mat4(1.0f);

    auto shader_program = device->create_shader_program(
        std::unordered_map<Shader_type, RHI_shader_code::Ptr>{
            {Shader_type::VERTEX, vertex_shader_code},
            {Shader_type::FRAGMENT, fragment_shader_code}
        },  
        [&](){
            
            std::unordered_map<std::string, RHI_uniform_entry_base::Ptr> uniforms{
                {"model", RHI_uniform_entry<glm::mat4>::create(model)},
                {"view", RHI_uniform_entry<glm::mat4>::create(glm::mat4(1.0f))},
                {"projection", RHI_uniform_entry<glm::mat4>::create(glm::mat4(1.0f))},
                {"camera_position", RHI_uniform_entry<glm::vec3>::create(glm::vec3(0.0f))},
                {"mainTexture", RHI_uniform_entry<int>::create(1)},
                {"activeLights", RHI_uniform_entry<int>::create(0)},
                {"mainLightIndex", RHI_uniform_entry<int>::create(-1)},
                {"material.ambient", RHI_uniform_entry<glm::vec3>::create(glm::vec3(1.0f))},
                {"material.diffuse", RHI_uniform_entry<glm::vec3>::create(glm::vec3(1.0f))},
                {"material.specular", RHI_uniform_entry<glm::vec3>::create(glm::vec3(1.0f))},
                {"material.shininess", RHI_uniform_entry<float>::create(32.0f)}
            };
    
            // 为每个可能的灯光索引预注册参数
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

    auto renderer = device->create_renderer(shader_program, box_geometry, nullptr);

    while (window->is_active()) {
        window->on_frame_begin();
        camera_control->update();

        model = glm::rotate(model, 0.01f, glm::vec3(0.5f, 1.0f, 0.0f));

        shader_program->modify_uniform<glm::mat4>("model", model);
        shader_program->modify_uniform<glm::mat4>("view", camera->view_matrix());
        shader_program->modify_uniform<glm::mat4>("projection", camera->projection_matrix());
        shader_program->modify_uniform<glm::vec3>("camera_position", camera->position());
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
                shader_program->modify_uniform<float>((prefix + ".innerAngle").c_str(), spot_light->inner_angle());
                shader_program->modify_uniform<float>((prefix + ".outerAngle").c_str(), spot_light->outer_angle());
            }
        	
        }

        shader_program->update_uniforms();

        renderer->draw();

        device->check_error();

        window->on_frame_end();
    }
    
    return 0;
}



