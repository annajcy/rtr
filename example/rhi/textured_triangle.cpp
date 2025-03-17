#include "engine/global/base.h"

#include "engine/runtime/loader/image_loader.h"
#include "engine/runtime/loader/image_loader_stb_image.h"

#include "engine/runtime/rhi/device/rhi_device.h"
#include "engine/runtime/rhi/device/rhi_device_opengl.h"

using namespace std;
using namespace rtr;

// Vertex Shader source code
const char* vertexShaderSource = R"(
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
const char* fragmentShaderSource = R"(
#version 460 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture_sampler;

void main()
{
    FragColor = vec4(texture(texture_sampler, TexCoord).rgb, 1.0);
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

    auto image_loader = std::make_shared<Image_loader_stb>();
    image = image_loader->load_from_path(Image_format::RGB_ALPHA, "assets/image/default_texture.jpg");
    
    RHI_device_descriptor descriptor{};

    descriptor.width = 800;
    descriptor.height = 600;
    descriptor.title = "RTR Engine";

    auto device = std::make_shared<RHI_device_OpenGL>(descriptor);

    device->pipeline_state()->set_profile(Pipeline_state_profile::OPAQUE);

    //prepare geometry

    auto vertex_attribute = device->create_vertex_buffer(
        Buffer_usage::STATIC, 
        Buffer_attribute_type::FLOAT, 
        3, 
        vertices.size(), 
        vertices.data()
    );

    auto texture_attribute = device->create_vertex_buffer(
        Buffer_usage::STATIC,
        Buffer_attribute_type::FLOAT,
        2,
        texture_coords.size(),
        texture_coords.data()
    );

    auto element_buffer = device->create_element_buffer(
        Buffer_usage::STATIC,
        indices.size(),
        indices.data()
    );
    
    std::unordered_map<unsigned int, std::shared_ptr<RHI_vertex_buffer>> vertex_buffers = {
        {0, vertex_attribute},
        {1, texture_attribute}
    };

    auto geometry = device->create_geometry(vertex_buffers, element_buffer);

    //prepare shaders

    auto vertex_shader = device->create_shader_code(
        Shader_type::VERTEX,
        vertexShaderSource
    );

    auto fragment_shader = device->create_shader_code(
        Shader_type::FRAGMENT,
        fragmentShaderSource
    );

    std::unordered_map<Shader_type, std::shared_ptr<RHI_shader_code>> shaders = {
        {Shader_type::VERTEX, vertex_shader},
        {Shader_type::FRAGMENT, fragment_shader}
    };

    int texture_location = 3;

    std::unordered_map<std::string, Uniform_entry> uniforms = {
        {"texture_sampler", {Uniform_type::SAMPLER_2D, &texture_location}}
    };

    auto shader_program = device->create_shader_program(shaders);
    shader_program->set_uniforms(uniforms);
    
    //prepare textures
    auto texture = device->create_texture_2D(
        image->width(),
        image->height(),
        image->data()
    );

    texture->set_filter(Texture_filter::LINEAR, Texture_filter_target::MIN);
    texture->set_filter(Texture_filter::LINEAR_MIPMAP_LINEAR, Texture_filter_target::MAG);
    texture->set_wrap(Texture_wrap::CLAMP_TO_EDGE, Texture_wrap_target::U);
    texture->set_wrap(Texture_wrap::CLAMP_TO_EDGE, Texture_wrap_target::V);
    texture->generate_mipmap();

    //prepare binding state

    std::unordered_map<unsigned int, std::shared_ptr<RHI_texture_2D>> textures = {
        {3, texture}
    };

    device->binding_state()->set_geometry(geometry);
    device->binding_state()->set_shader_program(shader_program);
    device->binding_state()->set_texture_2D(textures);  

    while (device->window()->is_active()) {
        device->clear();
        
        device->draw();
        
        device->window()->update();
    }
    
    return 0;
}

