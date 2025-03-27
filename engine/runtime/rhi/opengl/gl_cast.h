#pragma once

#include "engine/global/base.h"
#include "engine/runtime/enum.h"

namespace rtr {

inline constexpr unsigned int gl_usage(Buffer_usage usage) {
    switch (usage) {
    case Buffer_usage::STATIC:
        return GL_STATIC_DRAW;
    case Buffer_usage::DYNAMIC:
        return GL_DYNAMIC_DRAW;
    default:
        return GL_STATIC_DRAW;
    }
}

inline constexpr unsigned int gl_buffer_type(Buffer_type type) {
    switch(type) {
        case Buffer_type::VERTEX: return GL_ARRAY_BUFFER;
        case Buffer_type::INDEX: return GL_ELEMENT_ARRAY_BUFFER;
        case Buffer_type::UNIFORM: return GL_UNIFORM_BUFFER;
        case Buffer_type::STORAGE: return GL_SHADER_STORAGE_BUFFER;
        default: return GL_ARRAY_BUFFER;
    }
}

inline constexpr unsigned int gl_atribute_type(Buffer_attribute_type type) {
    switch (type) {
    case Buffer_attribute_type::FLOAT: return GL_FLOAT;
    case Buffer_attribute_type::INT: return GL_INT;
    case Buffer_attribute_type::UINT: return GL_UNSIGNED_INT;
    case Buffer_attribute_type::BOOL: return GL_BOOL;
    default: return 0;
    }
}

inline constexpr unsigned int gl_draw_mode(Draw_mode mode) {
    switch (mode) {
    case Draw_mode::TRIANGLES: return GL_TRIANGLES;
    case Draw_mode::POINTS: return GL_POINTS;
    case Draw_mode::LINES: return GL_LINES;
    default: return GL_TRIANGLES;
    }
}

inline constexpr unsigned int gl_blend_factor(Blend_factor factor) {
    switch (factor) {
        case Blend_factor::ZERO: return GL_ZERO;
        case Blend_factor::ONE: return GL_ONE;
        case Blend_factor::SRC_COLOR: return GL_SRC_COLOR;
        case Blend_factor::ONE_MINUS_SRC_COLOR: return GL_ONE_MINUS_SRC_COLOR;
        case Blend_factor::DST_COLOR: return GL_DST_COLOR;
        case Blend_factor::ONE_MINUS_DST_COLOR: return GL_ONE_MINUS_DST_COLOR;
        case Blend_factor::SRC_ALPHA: return GL_SRC_ALPHA;
        case Blend_factor::ONE_MINUS_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
        case Blend_factor::DST_ALPHA: return GL_DST_ALPHA;
        case Blend_factor::ONE_MINUS_DST_ALPHA: return GL_ONE_MINUS_DST_ALPHA;
        default: return GL_ZERO;
    }
}

inline constexpr unsigned int gl_blend_operation(Blend_operation operation) {
    switch (operation) {
        case Blend_operation::ADD: return GL_FUNC_ADD;
        case Blend_operation::SUBTRACT: return GL_FUNC_SUBTRACT;
        case Blend_operation::REVERSE_SUBTRACT: return GL_FUNC_REVERSE_SUBTRACT;
        case Blend_operation::MIN: return GL_MIN;
        case Blend_operation::MAX: return GL_MAX;
        default: return GL_FUNC_ADD;
    }
}

inline constexpr unsigned int gl_cull_mode(Cull_mode mode) {
    switch (mode) {
        case Cull_mode::FRONT: return GL_FRONT;
        case Cull_mode::BACK: return GL_BACK;
        case Cull_mode::FRONT_AND_BACK: return GL_FRONT_AND_BACK;
        default: return GL_BACK;
    }
}

inline constexpr unsigned int gl_front_face(Front_face face) {
    switch (face) {
        case Front_face::CLOCKWISE: return GL_CW;
        case Front_face::COUNTER_CLOCKWISE : return GL_CCW;
        default: return GL_CCW;
    }
}

inline constexpr unsigned int gl_depth_function(Depth_function function) {
    switch (function) {
        case Depth_function::NEVER: return GL_NEVER;
        case Depth_function::LESS: return GL_LESS;
        case Depth_function::EQUAL: return GL_EQUAL;
        case Depth_function::LESS_EQUAL : return GL_LEQUAL;
        case Depth_function::GREATER: return GL_GREATER;
        case Depth_function::NOT_EQUAL: return GL_NOTEQUAL;
        case Depth_function::GREATER_EQUAL : return GL_GEQUAL;
        case Depth_function::ALWAYS: return GL_ALWAYS;
        default: return GL_LESS;
    }
}

inline constexpr unsigned int gl_stencil_operation(Stencil_operation operation) {
    switch (operation) {
        case Stencil_operation::KEEP: return GL_KEEP;
        case Stencil_operation::ZERO: return GL_ZERO;
        case Stencil_operation::REPLACE: return GL_REPLACE;
        case Stencil_operation::INCR: return GL_INCR;
        case Stencil_operation::INCR_WRAP: return GL_INCR_WRAP;
        case Stencil_operation::DECR: return GL_DECR;
        case Stencil_operation::DECR_WRAP: return GL_DECR_WRAP;
        case Stencil_operation::INVERT: return GL_INVERT;
        default: return GL_KEEP;
    }
}

inline constexpr unsigned int gl_stencil_function(Stencil_function function) {
    switch (function) {
        case Stencil_function::NEVER: return GL_NEVER;
        case Stencil_function::LESS: return GL_LESS;
        case Stencil_function::EQUAL: return GL_EQUAL;
        case Stencil_function::LESS_EQUAL : return GL_LEQUAL;
        case Stencil_function::GREATER: return GL_GREATER;
        case Stencil_function::NOT_EQUAL: return GL_NOTEQUAL;
        case Stencil_function::GREATER_EQUAL : return GL_GEQUAL;
        case Stencil_function::ALWAYS: return GL_ALWAYS;
        default: return GL_LESS;
    }
}

inline constexpr unsigned int gl_shader_type(Shader_type type) {
    switch (type) {
        case Shader_type::VERTEX:
            return GL_VERTEX_SHADER;
        case Shader_type::FRAGMENT:
            return GL_FRAGMENT_SHADER;
        case Shader_type::GEOMETRY:
            return GL_GEOMETRY_SHADER;
        case Shader_type::COMPUTE:
            return GL_COMPUTE_SHADER;
        case Shader_type::TESSELATION_CONTROL:
            return GL_TESS_CONTROL_SHADER;
        case Shader_type::TESSELATION_EVALUATION:
            return GL_TESS_EVALUATION_SHADER;
        default:
            return 0;
    }
}

inline constexpr std::string gl_shader_type_str(Shader_type type) {
    switch (type) {
        case Shader_type::VERTEX:
            return "GL_VERTEX_SHADER";
        case Shader_type::FRAGMENT:
            return "GL_FRAGMENT_SHADER";
        case Shader_type::GEOMETRY:
            return "GL_GEOMETRY_SHADER";
        case Shader_type::COMPUTE:
            return "GL_COMPUTE_SHADER";
        case Shader_type::TESSELATION_CONTROL:
            return "GL_TESS_CONTROL_SHADER";
        case Shader_type::TESSELATION_EVALUATION:
            return "GL_TESS_EVALUATION_SHADER";
        default:
            return "";
    }
}

inline constexpr unsigned int gl_texture_type(Texture_type type) {
    switch (type) {
        case Texture_type::TEXTURE_2D:
            return GL_TEXTURE_2D;
        case Texture_type::TEXTURE_CUBE_MAP:
            return GL_TEXTURE_CUBE_MAP;
        default:
            return GL_TEXTURE_2D;
    }
}

inline constexpr unsigned int gl_texture_format(Texture_format format) {
    switch (format) {
        case Texture_format::RGB:
            return GL_RGB;
        case Texture_format::RGB_ALPHA:
            return GL_RGBA;
        case Texture_format::DEPTH_STENCIL:
            return GL_DEPTH_STENCIL;
        case Texture_format::SRGB_ALPHA:
            return GL_SRGB_ALPHA;
        case Texture_format::SRGB:
            return GL_SRGB;
        case Texture_format::DEPTH_STENCIL_24_8:
            return GL_DEPTH24_STENCIL8;
        default:
            return GL_RGB;
    }
}

inline constexpr unsigned int gl_texture_buffer_type(Texture_buffer_type type) {
    switch (type) {
        case Texture_buffer_type::UNSIGNED_BYTE:
            return GL_UNSIGNED_BYTE;
        case Texture_buffer_type::UNSIGNED_INT:
            return GL_UNSIGNED_INT;
        case Texture_buffer_type::UNSIGNED_INT_24_8:
            return GL_UNSIGNED_INT_24_8;
        default:
            return GL_UNSIGNED_BYTE;
    }
}

inline constexpr unsigned int gl_texture_wrap(Texture_wrap wrap) {
    switch (wrap) {
        case Texture_wrap::REPEAT:
            return GL_REPEAT;
        case Texture_wrap::MIRRORED_REPEAT:
            return GL_MIRRORED_REPEAT;
        case Texture_wrap::CLAMP_TO_EDGE:
            return GL_CLAMP_TO_EDGE;
        case Texture_wrap::CLAMP_TO_BORDER:
            return GL_CLAMP_TO_BORDER;
        default:
            return GL_REPEAT;
    }
}

inline constexpr unsigned int gl_texture_filter(Texture_filter filter) {
    switch (filter) {
        case Texture_filter::NEAREST:
            return GL_NEAREST;
        case Texture_filter::LINEAR:
            return GL_LINEAR;
        case Texture_filter::NEAREST_MIPMAP_NEAREST:
            return GL_NEAREST_MIPMAP_NEAREST;
        case Texture_filter::LINEAR_MIPMAP_NEAREST:
            return GL_LINEAR_MIPMAP_NEAREST;
        case Texture_filter::NEAREST_MIPMAP_LINEAR:
            return GL_NEAREST_MIPMAP_LINEAR;
        case Texture_filter::LINEAR_MIPMAP_LINEAR:
            return GL_LINEAR_MIPMAP_LINEAR;
        default:
            return GL_NEAREST;
    }
}

inline constexpr unsigned int gl_texture_cube_map_face(Texture_cubemap_face face) {
    switch (face) {
        case Texture_cubemap_face::POSITIVE_X:
            return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
        case Texture_cubemap_face::NEGATIVE_X:
            return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
        case Texture_cubemap_face::POSITIVE_Y:
            return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
        case Texture_cubemap_face::NEGATIVE_Y:
            return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
        case Texture_cubemap_face::POSITIVE_Z:
            return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
        case Texture_cubemap_face::NEGATIVE_Z:
            return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
        default:
            return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    }
}

inline constexpr unsigned int gl_texture_warp_target(Texture_wrap_target target) {
    switch (target) {
        case Texture_wrap_target::U:
            return GL_TEXTURE_WRAP_S;
        case Texture_wrap_target::V:
            return GL_TEXTURE_WRAP_T;
        case Texture_wrap_target::W:
            return GL_TEXTURE_WRAP_R;
        default:
            return GL_TEXTURE_WRAP_S;
    }
}

inline constexpr unsigned int gl_texture_filter_target(Texture_filter_target target) {
    switch (target) {
        case Texture_filter_target::MIN:
            return GL_TEXTURE_MIN_FILTER;
        case Texture_filter_target::MAG:
            return GL_TEXTURE_MAG_FILTER;
        default:
            return GL_TEXTURE_MIN_FILTER;
    }
}

};