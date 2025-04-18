// #include "engine/runtime/global/base.h" 
// #include "engine/runtime/core/geometry.h"
// #include "engine/runtime/core/input.h"
// #include "engine/runtime/core/camera.h"
// #include "engine/runtime/platform/rhi/opengl/rhi_device_opengl.h"

// #include "engine/runtime/resource/loader/image_loader.h"

// using namespace std;
// using namespace rtr;

// // Vertex Shader source code
// const char* vertex_shader_source = R"(
// #version 460 core
// layout (location = 0) in vec3 aPos;

// out vec3 UVW;

// uniform mat4 view;
// uniform mat4 projection;

// uniform vec3 cameraPosition;

// void main()
// {
// 	vec4 transformPosition = vec4(aPos, 1.0);

// 	mat4 model = mat4(1.0);
// 	model[3] = vec4(cameraPosition, 1.0); // 直接设置第四列

// 	vec4 pos = projection * view * model * vec4(aPos, 1.0);
// 	gl_Position = pos.xyww;
// 	UVW = aPos;
// }
// )";


// const char* fragment_shader_source = R"(
// #version 460 core
// out vec4 FragColor;

// in vec3 UVW;

// uniform sampler2D mainTex;

// const float PI = 3.1415926535897932384626433832795;

// vec2 UVWToUV(vec3 uvw) {
// 	vec3 uvwN = normalize(uvw);
// 	float u = atan(uvwN.z, uvwN.x) / (2.0 * PI) + 0.5;
// 	float v = asin(uvwN.y) / PI + 0.5;
// 	return vec2(u, v);
// }

// void main()
// {
// 	vec2 uv = UVWToUV(UVW);
// 	FragColor = texture(mainTex, uv);
// }

// )";


// const char* fragment_shader_source_cubemap = R"(
// #version 460 core
// out vec4 FragColor;

// in vec3 UVW;

// uniform samplerCube cubemapTex;

// void main()
// {
// 	FragColor = texture(cubemapTex, normalize(UVW));
// }
// )";


// int main() {

//     auto device = std::make_shared<RHI_device_OpenGL>();
    
//     auto window = device->create_window(800, 600, "RTR");
//     auto input = std::make_shared<Input>(window);
//     auto screen_frame_buffer = device->create_screen_frame_buffer(window);

//     auto bk = Image_loader::load_from_path(
//         Image_format::RGB_ALPHA,
//         "assets/image/bk.jpg"
//     );

    // auto mainTex = device->create_texture_2D(
    //     bk->width(), 
    //     bk->height(), 
    //     1, 
    //     Texture_internal_format::RGB_ALPHA, 
    //     std::unordered_map<Texture_wrap_target, Texture_wrap>{
    //         {Texture_wrap_target::U, Texture_wrap::CLAMP_TO_EDGE},
    //         {Texture_wrap_target::V, Texture_wrap::CLAMP_TO_EDGE}
    //     }, 
    //     std::unordered_map<Texture_filter_target, Texture_filter>{
    //         {Texture_filter_target::MIN, Texture_filter::LINEAR},
    //         {Texture_filter_target::MAG, Texture_filter::LINEAR}
    //     }, 
    //     Image_data{
    //         bk->width(),
    //         bk->height(),
    //         bk->data_ptr(),
    //         Texture_buffer_type::UNSIGNED_BYTE,
    //         Texture_external_format::RGB_ALPHA
    //     }
    // );

//     mainTex->bind_to_unit(0);

//     auto back_image = Image_loader::load_from_path(
//         Image_format::RGB_ALPHA,
//         "assets/image/skybox/back.jpg",
//         false
//     );
//     auto front_image = Image_loader::load_from_path(
//         Image_format::RGB_ALPHA,
//         "assets/image/skybox/front.jpg",
//         false
//     );
//     auto left_image = Image_loader::load_from_path(
//         Image_format::RGB_ALPHA,
//         "assets/image/skybox/left.jpg",
//         false
//     );
//     auto right_image = Image_loader::load_from_path(
//         Image_format::RGB_ALPHA,
//         "assets/image/skybox/right.jpg",
//         false
//     );
//     auto top_image = Image_loader::load_from_path(
//         Image_format::RGB_ALPHA,
//         "assets/image/skybox/top.jpg",
//         false
//     );
//     auto bottom_image = Image_loader::load_from_path(
//         Image_format::RGB_ALPHA,
//         "assets/image/skybox/bottom.jpg",
//         false
//     );

//     std::unordered_map<Texture_cubemap_face, Image_data> cubemap_data = {
//         {Texture_cubemap_face::RIGHT, Image_data{
//             right_image->width(),
//             right_image->height(),
//             right_image->data_ptr(),
//             Texture_buffer_type::UNSIGNED_BYTE,
//             Texture_external_format::RGB_ALPHA
//         }},
//         {Texture_cubemap_face::LEFT, Image_data{
//             left_image->width(),
//             left_image->height(),
//             left_image->data_ptr(),
//             Texture_buffer_type::UNSIGNED_BYTE,
//             Texture_external_format::RGB_ALPHA
//         }},
//         {Texture_cubemap_face::TOP, Image_data{
//             top_image->width(),
//             top_image->height(),
//             top_image->data_ptr(),
//             Texture_buffer_type::UNSIGNED_BYTE,
//             Texture_external_format::RGB_ALPHA
//         }},
//         {Texture_cubemap_face::BOTTOM, Image_data{
//             bottom_image->width(),
//             bottom_image->height(),
//             bottom_image->data_ptr(),
//             Texture_buffer_type::UNSIGNED_BYTE,
//             Texture_external_format::RGB_ALPHA
//         }},
//         {Texture_cubemap_face::FRONT, Image_data{
//             front_image->width(),
//             front_image->height(),
//             front_image->data_ptr(),
//             Texture_buffer_type::UNSIGNED_BYTE,
//             Texture_external_format::RGB_ALPHA
//         }},
//         {Texture_cubemap_face::BACK, Image_data{
//             back_image->width(),
//             back_image->height(),
//             back_image->data_ptr(),
//             Texture_buffer_type::UNSIGNED_BYTE,
//             Texture_external_format::RGB_ALPHA
//         }}
//     };

//     auto cubemap_tex = device->create_texture_cubemap(
//     	front_image->width(),
//     	front_image->height(),
//     	1,
//     	Texture_internal_format::RGB_ALPHA,
//     	std::unordered_map<Texture_wrap_target, Texture_wrap>{
//     		{Texture_wrap_target::U, Texture_wrap::CLAMP_TO_EDGE},
//     		{Texture_wrap_target::V, Texture_wrap::CLAMP_TO_EDGE},
//     		{Texture_wrap_target::W, Texture_wrap::CLAMP_TO_EDGE}
//     	},
//     	std::unordered_map<Texture_filter_target, Texture_filter>{
//     		{Texture_filter_target::MIN, Texture_filter::LINEAR},
//     		{Texture_filter_target::MAG, Texture_filter::LINEAR}
//     	},
//     	cubemap_data
//     );

//     cubemap_tex->bind_to_unit(1);

//     auto camera = Perspective_camera::create(
//         60.0f, 
//         (float)window->width() / (float)window->height(), 
//         0.1f, 
//         50.0f
//     );

//     camera->position() = glm::vec3(0.0f, 0.0f, 3.0f);
//     camera->look_at_point(glm::vec3(0.0f, 0.0f, 0.0f));

//     auto camera_control = Trackball_camera_control::create(camera, input);

//     auto vertex_shader_code = device->create_shader_code(Shader_type::VERTEX, vertex_shader_source);
//     auto fragment_shader_code = device->create_shader_code(Shader_type::FRAGMENT, fragment_shader_source);
//     auto fragment_shader_code_cubemap = device->create_shader_code(Shader_type::FRAGMENT, fragment_shader_source_cubemap);

//     auto shader_program = device->create_shader_program(
//         std::unordered_map<Shader_type, RHI_shader_code::Ptr>{
//             {Shader_type::VERTEX, vertex_shader_code},
//             {Shader_type::FRAGMENT, fragment_shader_code}
//         },
//         {
//             {"mainTex", RHI_uniform_entry<int>::create(0)},
//             {"view", RHI_uniform_entry<glm::mat4>::create(camera->view_matrix())},
//             {"projection", RHI_uniform_entry<glm::mat4>::create(camera->projection_matrix())},
//             {"cameraPosition", RHI_uniform_entry<glm::vec3>::create(camera->position())}
//         }
//     );

//     auto shader_program_cubemap = device->create_shader_program(
//         std::unordered_map<Shader_type, RHI_shader_code::Ptr>{
//             {Shader_type::VERTEX, vertex_shader_code},
//             {Shader_type::FRAGMENT, fragment_shader_code_cubemap}
//         },
//         {
//             {"cubemapTex", RHI_uniform_entry<int>::create(1)},
//             {"view", RHI_uniform_entry<glm::mat4>::create(camera->view_matrix())},
//             {"projection", RHI_uniform_entry<glm::mat4>::create(camera->projection_matrix())},
//             {"cameraPosition", RHI_uniform_entry<glm::vec3>::create(camera->position())}
//         }
//     );

//     auto geo = Geometry::create_box();

//     auto position = geo->attribute("position");    
//     auto position_buffer = device->create_vertex_buffer(
//         Buffer_usage::STATIC,
//         Buffer_data_type::FLOAT,
//         Buffer_iterate_type::PER_VERTEX,
//         position->unit_data_count(),
//         position->data_size(),
//         position->data_ptr()
//     );

//     auto uv = geo->attribute("uv");
//     auto uv_buffer = device->create_vertex_buffer(
//         Buffer_usage::STATIC,
//         Buffer_data_type::FLOAT,
//         Buffer_iterate_type::PER_VERTEX,
//         uv->unit_data_count(),
//         uv->data_size(),
//         uv->data_ptr()
//     );

//     auto element = geo->element_attribute();
//     auto element_buffer = device->create_element_buffer(
//         Buffer_usage::STATIC,
//         element->data_count(),
//         element->data_size(),
//         element->data_ptr()
//     );

//     auto geometry = device->create_geometry(
//         std::unordered_map<unsigned int, RHI_buffer::Ptr>{
//             {0, position_buffer},
//             {1, uv_buffer}
//         },
//         element_buffer
//     );

//     Clear_state clear_state = Clear_state::enabled();
//     clear_state.color_clear_value = glm::vec4(0.1, 0.5, 0.3, 1.0);
//     auto renderer = device->create_renderer(clear_state);
    

//     while (window->is_active()) {
//         window->on_frame_begin();
//         camera_control->update();

//         std::cout << camera->front().x << ", " << camera->front().y << ", " << camera->front().z << std::endl;

//         // shader_program->modify_uniform("view", camera->view_matrix());
//         // shader_program->modify_uniform("projection", camera->projection_matrix());
//         // shader_program->modify_uniform("cameraPosition", camera->position());

//         // shader_program->update_uniforms();

//         shader_program_cubemap->modify_uniform("view", camera->view_matrix());
//         shader_program_cubemap->modify_uniform("projection", camera->projection_matrix());
//         shader_program_cubemap->modify_uniform("cameraPosition", camera->position());
//         shader_program_cubemap->update_uniforms();

//         renderer->init();

//         renderer->clear(screen_frame_buffer);
//         renderer->draw(
//             shader_program_cubemap,
//             geometry,
//             screen_frame_buffer
//         );

//         device->check_error();
//         window->on_frame_end();
//     }
    
//     return 0;
// }

