#include "engine/global/base.h"
#include "engine/runtime/enum.h"
#include "engine/runtime/resource/loader/image_loader.h"
#include "engine/runtime/rhi/rhi_device.h"
#include "engine/runtime/rhi/opengl/rhi_device_opengl.h"
#include "engine/runtime/rhi/rhi_geometry.h"
#include "engine/runtime/rhi/rhi_pipeline_state.h"
#include "engine/runtime/rhi/rhi_resource.h"

using namespace std;
using namespace rtr;

// Vertex Shader source code
const char* vertex_shader_source = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
uniform mat4 transform;
    
out vec2 TexCoord;
    
void main()
{
    gl_Position = transform * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";
    
// Fragment Shader source code
const char* fragment_shader_source = R"(
#version 460 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D ourTexture; // 缺少纹理uniform

void main()
{
    vec4 texColor = texture(ourTexture, TexCoord);
    FragColor = texColor;
}
)";

int main() {

    std::cout << "Hello, RHI!" << std::endl;

    auto device = std::make_shared<RHI_device_OpenGL>();
    auto window = device->create_window(800, 600, "RTR");

    // 分离的位置数据和纹理坐标数据
    std::vector<float> vertices = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };
    
    std::vector<float> tex_coords = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.5f, 1.0f
    };
    
    std::vector<unsigned int> indices = {
        0, 1, 2
    };  

    auto element = device->create_element_buffer(
        Buffer_usage::STATIC,
        indices.size(),
        indices.size() * sizeof(unsigned int),
        indices.data()
    );

    // 创建位置缓冲区
    auto position = device->create_vertex_buffer(
        Buffer_usage::STATIC,
        Buffer_data_type::FLOAT,
        Buffer_iterate_type::PER_VERTEX,
        3,  // 每个顶点3个分量
        vertices.size() * sizeof(float),
        vertices.data()
    );

    // 创建独立的纹理坐标缓冲区
    auto texCoord = device->create_vertex_buffer(
        Buffer_usage::STATIC,
        Buffer_data_type::FLOAT,
        Buffer_iterate_type::PER_VERTEX,
        2,  // 每个顶点2个分量
        tex_coords.size() * sizeof(float),
        tex_coords.data()
    );

    // 修改几何体绑定（需要对应修改vertex shader的location）
    auto geometry = device->create_geometry(
        std::unordered_map<unsigned int, RHI_buffer::Ptr>{
            {0, position},  // location 0 绑定位置数据
            {1, texCoord}    // location 1 绑定纹理坐标
        },
        element
    );

    auto vertex_shader_code = device->create_shader_code(Shader_type::VERTEX, vertex_shader_source);
    auto fragment_shader_code = device->create_shader_code(Shader_type::FRAGMENT, fragment_shader_source);

    auto image_loader = std::make_shared<Image_loader>();
    auto image = image_loader->load_from_path(
        Image_format::RGB_ALPHA, 
        "assets/image/default_texture.jpg"
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
            {Texture_filter_target::MIN, Texture_filter::LINEAR},
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

    texture->bind(0);
    
    int samplerLoc = 0;
    glm::mat4 transform = glm::mat4(1.0f);

    auto shader_program = device->create_shader_program(
        std::unordered_map<Shader_type, RHI_shader_code::Ptr>{
            {Shader_type::VERTEX, vertex_shader_code},
            {Shader_type::FRAGMENT, fragment_shader_code}
        },  
        std::unordered_map<std::string, RHI_uniform_entry> {
            {"transform", RHI_uniform_entry{Uniform_type::MAT4, &transform}},
            {"ourTexture", RHI_uniform_entry{Uniform_type::SAMPLER, &samplerLoc}}
           
        }, 
        std::unordered_map<std::string, RHI_uniform_array_entry>{}
    );


    auto pipeline_state = device->create_pipeline_state(Pipeline_state::opaque_pipeline_state());
    pipeline_state->cull_state.enable = false;

    pipeline_state->apply();

    geometry->bind();
    shader_program->bind();

    while (window->is_active()) {
        window->on_frame_begin();

        // 问题4：缺少矩阵动画和uniform更新
        static float rotation = 0.0f;
        rotation += 0.5f;
        transform = glm::rotate(glm::mat4(1.0f), 
                              glm::radians(rotation), 
                              glm::vec3(0.5f, 1.0f, 0.0f));

        shader_program->modify_uniform("transform", Uniform_type::MAT4, &transform);
        shader_program->update_uniforms();

        //device->check_error();
        geometry->draw();

        window->on_frame_end();
    }
    
    return 0;
}



