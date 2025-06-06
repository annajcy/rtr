#include "engine/runtime/function/render/frontend/frame_buffer.h"
#include "engine/runtime/function/render/frontend/shader.h"
#include "engine/runtime/function/render/frontend/texture.h"
#include "engine/runtime/platform/rhi/opengl/rhi_error_opengl.h"
#include "engine/runtime/function/render/frontend/geometry.h"
#include "engine/runtime/platform/rhi/opengl/rhi_device_opengl.h"
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
layout(location = 0) out vec4 FragColor0;
layout(location = 1) out vec4 FragColor1;

in vec2 v_texCoords; // 你需要从顶点着色器传递纹理坐标

void main()
{
    // 根据纹理坐标生成一些变化图案
    // FragColor0 = vec4(v_texCoords.x, v_texCoords.y, 0.5, 1.0);

    // 或者一个简单的棋盘格
    float pattern = mod(floor(gl_FragCoord.x / 16.0) + floor(gl_FragCoord.y / 16.0), 2.0);
    FragColor0 = vec4(pattern, pattern, pattern, 1.0);

    FragColor1 = vec4(0.2, 0.5, 1.0, 1.0); // 第二个可以保持不变或也修改
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

void main()
{
    vec4 color0 = textureLod(texture0, TexCoords, 2);
    vec4 color1 = texture(texture1, TexCoords, 2);
    FragColor = mix(color0, color1, 0.5); // 混合两个缓冲区
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
        Element_attribute::create(indices)
    );


    auto geometry = geo->rhi(device);

    auto sc0 = Shader_code::create(Shader_type::VERTEX, vertex_shader_source);
    auto sc1 = Shader_code::create(Shader_type::FRAGMENT, fragment_shader_source);

    auto sp = Shader_program::create("shader_program", std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> {
        {Shader_type::VERTEX, sc0},
        {Shader_type::FRAGMENT, sc1}
    },  std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> {});
    

    auto shader_program = sp->rhi(device);

    auto ca0 = Texture_2D::create_color_attachemnt_rgba(
        window->width(),
        window->height(),
        4
    );

    auto ca1 = Texture_2D::create_color_attachemnt_rgba(
        window->width(),
        window->height(),
        4
    );

    auto da = Texture_2D::create_depth_attachemnt(
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


    auto frame_buffer = fb->rhi(device);
    auto screen_vertex_shader_code = Shader_code::create(Shader_type::VERTEX, vertex_shader_source1);
    auto screen_fragment_shader_code = Shader_code::create(Shader_type::FRAGMENT, fragment_shader_source1);

    auto screen_shader_program = Shader_program::create("screen_shader_program", std::unordered_map<Shader_type, std::shared_ptr<Shader_code>> {
        {Shader_type::VERTEX, screen_vertex_shader_code},
        {Shader_type::FRAGMENT, screen_fragment_shader_code}
    },  std::unordered_map<std::string, std::shared_ptr<Uniform_entry_base>> {
        {"texture0", Uniform_entry<int>::create(0)},
        {"texture1", Uniform_entry<int>::create(1)}
    });

    auto shader_program1 = screen_shader_program->rhi(device);

    auto sg = Geometry::create_plane();

    auto screen_geometry = sg->rhi(device);

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

        ca0->rhi(device)->bind_to_unit(0);
        ca1->rhi(device)->bind_to_unit(1);

        ca0->rhi(device)->generate_mipmap();
        ca1->rhi(device)->generate_mipmap();


        gl_check_error();
    
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

