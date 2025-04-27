#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"

#define MAX_SPOT_LIGHT 8
#define MAX_POINT_LIGHT 8

namespace rtr {

struct Light_matrix_ubo {
    glm::mat4 light_matrix{};
    glm::vec3 light_camera_direction{};
};

struct Camera_ubo {
    glm::mat4 view_matrix{};      
    glm::mat4 projection_matrix{}; 

    glm::vec3 camera_position{}; 
    float padding1[1];
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
    Directional_light_ubo directional_light_ubo[MAX_SPOT_LIGHT];
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