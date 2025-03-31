#include "engine/global/base.h"
#include "engine/runtime/core/camera.h"
#include "engine/runtime/core/geometry.h"
#include "engine/runtime/enum.h"
#include "engine/runtime/resource/loader/image_loader.h"
#include "engine/runtime/rhi/rhi_device.h"
#include "engine/runtime/rhi/opengl/rhi_device_opengl.h"
#include "engine/runtime/rhi/rhi_geometry.h"
#include "engine/runtime/rhi/rhi_pipeline_state.h"
#include "engine/runtime/rhi/rhi_resource.h"
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

uniform sampler2D mainTexture;

void main()
{
    vec4 texColor = texture(mainTexture, TexCoord);
    FragColor = texColor;
}
)";

int main() {

    auto image_loader = std::make_shared<Image_loader>();

    auto device = std::make_shared<RHI_device_OpenGL>();
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
        3,  // 每个顶点3个分量
        geo_position->data_size(),
        geo_position->data_ptr()
    );

    // 创建独立的纹理坐标缓冲区
    auto tex_coord = device->create_vertex_buffer(
        Buffer_usage::STATIC,
        Buffer_data_type::FLOAT,
        Buffer_iterate_type::PER_VERTEX,
        2,  // 每个顶点2个分量
        geo_tex_coord->data_size(),
        geo_tex_coord->data_ptr()
    );
    
    auto normal = device->create_vertex_buffer(
        Buffer_usage::STATIC,
        Buffer_data_type::FLOAT,
        Buffer_iterate_type::PER_VERTEX,
        3,  // 每个顶点3个分量
        geo_normal->data_size(),
        geo_normal->data_ptr()
    );

    // 修改几何体绑定（需要对应修改vertex shader的location）
    auto geometry = device->create_geometry(
        std::unordered_map<unsigned int, RHI_buffer::Ptr>{
            {0, position},  // location 0 绑定位置数据
            {1, tex_coord},    // location 1 绑定纹理坐标
            {2, normal}
        },
        element
    );
    
    auto image = image_loader->load_from_path(
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
    texture->bind(1);

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
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 0.0f);
    int main_tex_sampler_unit = 1;

    auto shader_program = device->create_shader_program(
        std::unordered_map<Shader_type, RHI_shader_code::Ptr>{
            {Shader_type::VERTEX, vertex_shader_code},
            {Shader_type::FRAGMENT, fragment_shader_code}
        },  
        std::unordered_map<std::string, RHI_uniform_entry> {
            {"model", RHI_uniform_entry{Uniform_type::MAT4, glm::value_ptr(model)}},
            {"view", RHI_uniform_entry{Uniform_type::MAT4, glm::value_ptr(view)}},
            {"projection", RHI_uniform_entry{Uniform_type::MAT4, glm::value_ptr(projection)}},
            {"camera_position", RHI_uniform_entry{Uniform_type::VEC3, glm::value_ptr(camera_position)}},
            {"mainTexture", RHI_uniform_entry{Uniform_type::SAMPLER, &main_tex_sampler_unit}}
        }, 
        std::unordered_map<std::string, RHI_uniform_array_entry>{}
    );

    auto pipeline_state = device->create_pipeline_state(Pipeline_state::opaque_pipeline_state());
    pipeline_state->apply();

    while (window->is_active()) {
        window->on_frame_begin();
        camera_control->update();
        
        shader_program->modify_uniform<glm::mat4>("model", [](glm::mat4* model){
            *model = glm::rotate(*model, 0.01f, glm::vec3(0.5f, 1.0f, 0.0f));
        });

        shader_program->modify_uniform<glm::mat4>("view", [&](glm::mat4* view){
            *view = camera->view_matrix();
        });

        shader_program->modify_uniform<glm::mat4>("projection", [&](glm::mat4* projection){
            *projection = camera->projection_matrix();
        });

        shader_program->modify_uniform<glm::vec3>("camera_position", [&](glm::vec3* camera_position){
            *camera_position = camera->position();
        });
        
        shader_program->update_uniforms();

        shader_program->bind();
        geometry->bind();
        geometry->draw();
        geometry->unbind();
        shader_program->unbind();

        device->check_error();

        window->on_frame_end();
    }
    
    return 0;
}



