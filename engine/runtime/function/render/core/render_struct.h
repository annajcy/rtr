#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"

#define MAX_SPOT_LIGHT 8
#define MAX_POINT_LIGHT 8
#define MAX_DIRECTIONAL_LIGHT 8
#define MAX_CAMERA 8

namespace rtr {

struct Camera_ubo {
    glm::mat4 view_matrix{};      
    glm::mat4 projection_matrix{}; 

    glm::vec3 camera_position{}; 
    float padding1[1];
    glm::vec3 camera_direction{};
    float padding2[1];
    float near{};
    float far{};
    float padding3[2];
}; 

struct Camera_ubo_array {
    int count{};
    float padding1[3];
    Camera_ubo camera_ubo[MAX_CAMERA];
};

struct Perspective_camera_ubo {
    glm::mat4 view_matrix{};
    glm::mat4 projection_matrix{};
    glm::vec3 camera_position{};
    float padding1[1];
    glm::vec3 camera_direction{};
    float padding2[1];
    float near{};
    float far{};
    float fov{};
    float aspect_ratio{};
};

struct Perspective_camera_ubo_array {
    int count{};
    float padding1[3];
    Perspective_camera_ubo perspective_camera_ubo[MAX_CAMERA];
};

struct Orthographic_camera_ubo {
    glm::mat4 view_matrix{};
    glm::mat4 projection_matrix{};
    glm::vec3 camera_position{};
    float padding1[1];
    glm::vec3 camera_direction{};
    float padding2[1];
    float near{};
    float far{};
    float left{};
    float right{};
    float bottom{};
    float top{};
    float padding3[2];
};

struct Orthographic_camera_ubo_array {
    int count{};
    float padding1[3];
    Orthographic_camera_ubo orthographic_camera_ubo[MAX_CAMERA];
};

struct Directional_light_ubo {
    float intensity{};             
    float padding1[3];          

    glm::vec3 color{};   
    float padding2[1];

    glm::vec3 direction{};
    float padding3[1];         
}; 

struct Directional_light_ubo_array {
    int count{};
    float padding1[3];
    Directional_light_ubo directional_light_ubo[MAX_DIRECTIONAL_LIGHT];
};

struct Point_light_ubo {
    float intensity{};             
    float padding1[3];            

    glm::vec3 position{};    
    float padding2[1];

    glm::vec3 color{};
    float padding3[1];             

    glm::vec3 attenuation{};       
    float padding4[1];       
}; 

struct Point_light_ubo_array {
    int count{};
    float padding1[3];
    Point_light_ubo point_light_ubo[MAX_POINT_LIGHT];
};

struct Spot_light_ubo {
    float intensity{};
    float inner_angle_cos{};
    float outer_angle_cos{};  
    float padding1[1];

    glm::vec3 direction{};
    float padding2[1];   

    glm::vec3 position{};          
    float padding3[1];

    glm::vec3 color{};
    float padding4[1]; 
}; 

struct Spot_light_ubo_array {
    int count{};
    float padding1[3];
    Spot_light_ubo spot_light_ubo[MAX_SPOT_LIGHT];
};

}