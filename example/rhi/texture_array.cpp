#include "engine/runtime/function/render/object/frame_buffer.h"
#include "engine/runtime/function/render/object/shader.h"
#include "engine/runtime/function/render/object/texture.h"
#include "engine/runtime/global/base.h" 
#include "engine/runtime/function/render/object/geometry.h"
#include "engine/runtime/platform/rhi/opengl/rhi_device_opengl.h"
#include "engine/runtime/platform/rhi/rhi_texture.h"
#include <memory>

using namespace std;
using namespace rtr;

// Vertex Shader source code
const char* vertex_shader_source = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
void main()
{
    gl_Position = vec4(aPos, 1.0);
}
)";
    
// Fragment Shader source code
// 修改片段着色器支持多目标输出
const char* fragment_shader_source = R"(
#version 460 core
layout(location = 0) out vec4 FragColor0;  // 第一个颜色附件
layout(location = 1) out vec4 FragColor1;  // 第二个颜色附件

void main()
{
    FragColor0 = vec4(1.0, 0.5, 0.2, 1.0); // 红色通道为主
    FragColor1 = vec4(0.2, 0.5, 1.0, 1.0); // 蓝色通道为主
}
)";

const char* vertex_shader_source1 = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    TexCoords = aUV;
}
)";

// 修改后处理片段着色器
const char* fragment_shader_source1 = R"(
#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D texture0;  // 第一个颜色缓冲
uniform sampler2D texture1;  // 第二个颜色缓冲
uniform sampler2DArray textureArray;  // 纹理数组

void main()
{
    // vec4 color0 = texture(texture0, TexCoords);
    // vec4 color1 = texture(texture1, TexCoords);
    // FragColor = mix(color0, color1, 0.5); // 混合两个缓冲区
    vec4 color0 = texture(textureArray, vec3(TexCoords, 0));
    vec4 color1 = texture(textureArray, vec3(TexCoords, 1));
    //FragColor = mix(color0, color1, 0.5); // 混合两个缓冲区
    FragColor = color1; // 混合两个缓冲区
}
)";

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

int main() {

    auto device = std::make_shared<RHI_device_OpenGL>();
    
    auto window = device->create_window(800, 600, "RTR");
    auto screen_frame_buffer = device->create_screen_buffer(window);

    auto geo = Geometry::create(
        std::unordered_map<unsigned int, std::shared_ptr<Vertex_attribute_base>> {
            {0, Vertex_attribute<float, 3>::create(vertices)},
            {1, Vertex_attribute<float, 2>::create(tex_coords)}
        },  
        Element_atrribute::create(indices)
    );

    geo->link(device);
    auto geometry = geo->rhi_resource();

    auto sc0 = Shader_code::create(Shader_type::VERTEX, vertex_shader_source);
    auto sc1 = Shader_code::create(Shader_type::FRAGMENT, fragment_shader_source);

    auto sp = Shader_program::create("shader_program", std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> {
        {Shader_type::VERTEX, sc0},
        {Shader_type::FRAGMENT, sc1}
    },  std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> {});
    
    sp->link(device);
    auto shader_program = sp->rhi_resource();

    auto ca0 = Texture_color_attachment::create(
        window->width(),
        window->height()
    );

    auto ca1 = Texture_color_attachment::create(
        window->width(),
        window->height()
    );

    auto da = Texture_depth_attachment::create(
        window->width(),
        window->height()
    );

    auto fb = Frame_buffer::create(
        window->width(),
        window->height(),
        std::vector<std::shared_ptr<Texture>>{
            ca0,
            ca1
        },
        da  
    );

    auto text_arr_builder = device->create_texture_builder();

    auto texture_array = device->create_texture_2D_array(
        window->width(),
        window->height(),
        4,
        Texture_internal_format::RGB_ALPHA,
        std::unordered_map<Texture_wrap_target, Texture_wrap> {
            {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_EDGE},
            {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_EDGE}
        },
        std::unordered_map<Texture_filter_target, Texture_filter> {
            {Texture_filter_target::MIN, Texture_filter::LINEAR},
            {Texture_filter_target::MAG, Texture_filter::LINEAR}
        },
        2
    );

    fb->link(device);
    auto frame_buffer = fb->rhi_resource();

    auto screen_vertex_shader_code = Shader_code::create(Shader_type::VERTEX, vertex_shader_source1);
    auto screen_fragment_shader_code = Shader_code::create(Shader_type::FRAGMENT, fragment_shader_source1);

    auto screen_shader_program = Shader_program::create("screen_shader_program", std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> {
        {Shader_type::VERTEX, screen_vertex_shader_code},
        {Shader_type::FRAGMENT, screen_fragment_shader_code}
    },  std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> {
        {"texture0", Uniform_entry<int>::create(0)},
        {"texture1", Uniform_entry<int>::create(1)},
        {"textureArray", Uniform_entry<int>::create(2)}
    });
    
    screen_shader_program->link(device);
    auto shader_program1 = screen_shader_program->rhi_resource();

    auto sg = Geometry::create_plane();
    sg->link(device);
    auto screen_geometry = sg->rhi_resource();

    Clear_state clear_state = Clear_state::enabled();
    clear_state.color_clear_value = glm::vec4(0.1, 0.5, 0.3, 1.0);
    auto renderer = device->create_renderer(clear_state);

    while (window->is_active()) {
        window->on_frame_begin();

        device->check_error();

        renderer->init();
        renderer->clear(frame_buffer);

        renderer->draw(
            shader_program,
            geometry,
            frame_buffer
        );

        ca0->rhi_resource()->bind_to_unit(0);
        ca1->rhi_resource()->bind_to_unit(1);

        text_arr_builder->build_texture_2D_array(
            texture_array, 
            std::vector<std::shared_ptr<RHI_texture>> {
                ca0->rhi_resource(),
                ca1->rhi_resource()
        });

        texture_array->bind_to_unit(2);
    
        renderer->clear(screen_frame_buffer);

        renderer->draw(
            shader_program1,
            screen_geometry,
            screen_frame_buffer
        );

        window->on_frame_end();
    }
    
    return 0;
}


