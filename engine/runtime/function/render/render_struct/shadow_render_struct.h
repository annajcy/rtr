#pragma once

#include "engine/runtime/function/render/render_struct/camera_render_struct.h"


namespace rtr {

//SHADWOW
struct CSM_Shadow_map_ubo {
    int layer_count{};
    float padding1[3];
    Orthographic_camera_ubo shadow_camera[MAX_CAMERA];
    float split_near_plane[MAX_CAMERA];
    float split_far_plane[MAX_CAMERA];
};

}