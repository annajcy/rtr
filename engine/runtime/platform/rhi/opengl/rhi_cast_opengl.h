#pragma once

#include "engine/runtime/global/base.h" 
#include "engine/runtime/global/enum.h"

namespace rtr {

inline constexpr unsigned int gl_usage(Buffer_usage usage) {
    switch (usage) {
    case Buffer_usage::STATIC:
        return GL_STATIC_DRAW;
    case Buffer_usage::DYNAMIC:
        return GL_DYNAMIC_DRAW;
    case Buffer_usage::STREAM:
        return GL_STREAM_DRAW;
    default:
        return GL_STATIC_DRAW;
    }
}

inline constexpr unsigned int gl_buffer_type(Buffer_type type) {
    switch(type) {
        case Buffer_type::VERTEX: return GL_ARRAY_BUFFER;
        case Buffer_type::ELEMENT: return GL_ELEMENT_ARRAY_BUFFER;
        case Buffer_type::UNIFORM: return GL_UNIFORM_BUFFER;
        case Buffer_type::STORAGE: return GL_SHADER_STORAGE_BUFFER;
        default: return GL_ARRAY_BUFFER;
    }
}

inline constexpr unsigned int gl_buffer_data_type(Buffer_data_type type) {
    switch (type) {
    case Buffer_data_type::FLOAT: return GL_FLOAT;
    case Buffer_data_type::INT: return GL_INT;
    case Buffer_data_type::UINT: return GL_UNSIGNED_INT;
    case Buffer_data_type::BOOL: return GL_BOOL;
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
        case Texture_type::TEXTURE_CUBEMAP:
            return GL_TEXTURE_CUBE_MAP;
        case Texture_type::TEXTURE_2D_ARRAY:
            return GL_TEXTURE_2D_ARRAY;
        default:
            return GL_TEXTURE_2D;
    }
}

inline constexpr unsigned int gl_texture_internal_format(Texture_internal_format format) {
    switch (format) {
        case Texture_internal_format::RGB:
            return GL_RGB8;  // 添加位数
        case Texture_internal_format::RGB_ALPHA:
            return GL_RGBA8;
        case Texture_internal_format::RGB_ALPHA_16F:
            return GL_RGBA16F;
        case Texture_internal_format::RGB_ALPHA_32F:
            return GL_RGBA32F;
        case Texture_internal_format::DEPTH_STENCIL:
            return GL_DEPTH24_STENCIL8; // 更明确的深度格式
        case rtr::Texture_internal_format::DEPTH:
            return GL_DEPTH_COMPONENT24;
        case Texture_internal_format::DEPTH_32F:
            return GL_DEPTH_COMPONENT32F;
        case Texture_internal_format::SRGB_ALPHA:
            return GL_SRGB8_ALPHA8;  // 添加位数
        case Texture_internal_format::SRGB:
            return GL_SRGB8;         // 添加位数
        case Texture_internal_format::DEPTH_STENCIL_24_8:
            return GL_DEPTH24_STENCIL8;
        default:
            return GL_RGB8;  // 默认也带位数
    }
}

inline constexpr unsigned int gl_texture_external_format(Texture_external_format format) {
    switch (format) {
        case Texture_external_format::RGB:
            return GL_RGB;
        case Texture_external_format::RGB_ALPHA:
            return GL_RGBA;
        case Texture_external_format::DEPTH_STENCIL:
            return GL_DEPTH_STENCIL;
        case Texture_external_format::SRGB_ALPHA:
            return GL_SRGB_ALPHA;
        case Texture_external_format::SRGB:
            return GL_SRGB;
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
        case Texture_buffer_type::FLOAT:
            return GL_FLOAT;
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
        case Texture_cubemap_face::RIGHT:
            return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
        case Texture_cubemap_face::LEFT:
            return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
        case Texture_cubemap_face::TOP:
            return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
        case Texture_cubemap_face::BOTTOM:
            return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
        case Texture_cubemap_face::FRONT:
            return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
        case Texture_cubemap_face::BACK:
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

inline constexpr Key_code gl_to_rhi_key_map(unsigned int key) {
    switch (key) {
        case GLFW_KEY_A: return Key_code::A;
        case GLFW_KEY_B: return Key_code::B;
        case GLFW_KEY_C: return Key_code::C;
        case GLFW_KEY_D: return Key_code::D;
        case GLFW_KEY_E: return Key_code::E;
        case GLFW_KEY_F: return Key_code::F;
        case GLFW_KEY_G: return Key_code::G;
        case GLFW_KEY_H: return Key_code::H;
        case GLFW_KEY_I: return Key_code::I;
        case GLFW_KEY_J: return Key_code::J;
        case GLFW_KEY_K: return Key_code::K;
        case GLFW_KEY_L: return Key_code::L;
        case GLFW_KEY_M: return Key_code::M;
        case GLFW_KEY_N: return Key_code::N;
        case GLFW_KEY_O: return Key_code::O;
        case GLFW_KEY_P: return Key_code::P;
        case GLFW_KEY_Q: return Key_code::Q;
        case GLFW_KEY_R: return Key_code::R;
        case GLFW_KEY_S: return Key_code::S;
        case GLFW_KEY_T: return Key_code::T;
        case GLFW_KEY_U: return Key_code::U;
        case GLFW_KEY_V: return Key_code::V;
        case GLFW_KEY_W: return Key_code::W;
        case GLFW_KEY_X: return Key_code::X;
        case GLFW_KEY_Y: return Key_code::Y;
        case GLFW_KEY_Z: return Key_code::Z;
        case GLFW_KEY_0: return Key_code::NUM_0;
        case GLFW_KEY_1: return Key_code::NUM_1;
        case GLFW_KEY_2: return Key_code::NUM_2;
        case GLFW_KEY_3: return Key_code::NUM_3;
        case GLFW_KEY_4: return Key_code::NUM_4;
        case GLFW_KEY_5: return Key_code::NUM_5;
        case GLFW_KEY_6: return Key_code::NUM_6;
        case GLFW_KEY_7: return Key_code::NUM_7;
        case GLFW_KEY_8: return Key_code::NUM_8;
        case GLFW_KEY_9: return Key_code::NUM_9;
        case GLFW_KEY_SPACE: return Key_code::SPACE;
        case GLFW_KEY_APOSTROPHE: return Key_code::APOSTROPHE;
        case GLFW_KEY_COMMA: return Key_code::COMMA;
        case GLFW_KEY_MINUS: return Key_code::MINUS;
        case GLFW_KEY_PERIOD: return Key_code::PERIOD;
        case GLFW_KEY_SLASH: return Key_code::SLASH;
        case GLFW_KEY_SEMICOLON: return Key_code::SEMICOLON;
        case GLFW_KEY_EQUAL: return Key_code::EQUAL;
        case GLFW_KEY_LEFT_BRACKET: return Key_code::LEFT_BRACKET;
        case GLFW_KEY_BACKSLASH: return Key_code::BACKSLASH;
        case GLFW_KEY_RIGHT_BRACKET: return Key_code::RIGHT_BRACKET;
        case GLFW_KEY_GRAVE_ACCENT: return Key_code::GRAVE_ACCENT;
        case GLFW_KEY_ESCAPE: return Key_code::ESCAPE;
        case GLFW_KEY_ENTER: return Key_code::ENTER;
        case GLFW_KEY_TAB: return Key_code::TAB;
        case GLFW_KEY_BACKSPACE: return Key_code::BACKSPACE;
        case GLFW_KEY_INSERT: return Key_code::INSERT;
        case GLFW_KEY_DELETE: return Key_code::DELETE;
        case GLFW_KEY_RIGHT: return Key_code::RIGHT;
        case GLFW_KEY_LEFT: return Key_code::LEFT;
        case GLFW_KEY_DOWN: return Key_code::DOWN;
        case GLFW_KEY_UP: return Key_code::UP;
        case GLFW_KEY_PAGE_UP: return Key_code::PAGE_UP;
        case GLFW_KEY_PAGE_DOWN: return Key_code::PAGE_DOWN;
        case GLFW_KEY_HOME: return Key_code::HOME;
        case GLFW_KEY_END: return Key_code::END;
        case GLFW_KEY_CAPS_LOCK: return Key_code::CAPS_LOCK;
        case GLFW_KEY_SCROLL_LOCK: return Key_code::SCROLL_LOCK;
        case GLFW_KEY_NUM_LOCK: return Key_code::NUM_LOCK;
        case GLFW_KEY_PRINT_SCREEN: return Key_code::PRINT_SCREEN;
        case GLFW_KEY_PAUSE: return Key_code::PAUSE;
        case GLFW_KEY_F1: return Key_code::F1;
        case GLFW_KEY_F2: return Key_code::F2;
        case GLFW_KEY_F3: return Key_code::F3;
        case GLFW_KEY_F4: return Key_code::F4;
        case GLFW_KEY_F5: return Key_code::F5;
        case GLFW_KEY_F6: return Key_code::F6;
        case GLFW_KEY_F7: return Key_code::F7;
        case GLFW_KEY_F8: return Key_code::F8;
        case GLFW_KEY_F9: return Key_code::F9;
        case GLFW_KEY_F10: return Key_code::F10;
        case GLFW_KEY_F11: return Key_code::F11;
        case GLFW_KEY_F12: return Key_code::F12;
        case GLFW_KEY_KP_0: return Key_code::KP_0;
        case GLFW_KEY_KP_1: return Key_code::KP_1;
        case GLFW_KEY_KP_2: return Key_code::KP_2;
        case GLFW_KEY_KP_3: return Key_code::KP_3;
        case GLFW_KEY_KP_4: return Key_code::KP_4;
        case GLFW_KEY_KP_5: return Key_code::KP_5;
        case GLFW_KEY_KP_6: return Key_code::KP_6;
        case GLFW_KEY_KP_7: return Key_code::KP_7;
        case GLFW_KEY_KP_8: return Key_code::KP_8;
        case GLFW_KEY_KP_9: return Key_code::KP_9;
        case GLFW_KEY_KP_DECIMAL: return Key_code::KP_DECIMAL;
        case GLFW_KEY_KP_DIVIDE: return Key_code::KP_DIVIDE;
        case GLFW_KEY_KP_MULTIPLY: return Key_code::KP_MULTIPLY;
        case GLFW_KEY_KP_SUBTRACT: return Key_code::KP_SUBTRACT;
        case GLFW_KEY_KP_ADD: return Key_code::KP_ADD;
        case GLFW_KEY_KP_ENTER: return Key_code::KP_ENTER;
        case GLFW_KEY_KP_EQUAL: return Key_code::KP_EQUAL;
        case GLFW_KEY_LEFT_SHIFT: return Key_code::LEFT_SHIFT;
        case GLFW_KEY_LEFT_CONTROL: return Key_code::LEFT_CONTROL;
        case GLFW_KEY_LEFT_ALT: return Key_code::LEFT_ALT;
        case GLFW_KEY_LEFT_SUPER: return Key_code::LEFT_SUPER;
        case GLFW_KEY_RIGHT_SHIFT: return Key_code::RIGHT_SHIFT;
        case GLFW_KEY_RIGHT_CONTROL: return Key_code::RIGHT_CONTROL;
        case GLFW_KEY_RIGHT_ALT: return Key_code::RIGHT_ALT;
        case GLFW_KEY_RIGHT_SUPER: return Key_code::RIGHT_SUPER;
        case GLFW_KEY_MENU: return Key_code::MENU;
        default: return Key_code::UNKNOWN;
    }
}

inline constexpr Key_action gl_to_rhi_key_action_map(unsigned int action) {
    switch (action) {
        case GLFW_PRESS: return Key_action::PRESS;
        case GLFW_RELEASE: return Key_action::RELEASE;
        case GLFW_REPEAT: return Key_action::REPEAT;
        default: return Key_action::UNKNOWN;
    }
}

inline constexpr Mouse_button gl_to_rhi_mouse_button_map(unsigned int button) {
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT: return Mouse_button::LEFT;
        case GLFW_MOUSE_BUTTON_RIGHT: return Mouse_button::RIGHT;
        case GLFW_MOUSE_BUTTON_MIDDLE: return Mouse_button::MIDDLE;
        case GLFW_MOUSE_BUTTON_4: return Mouse_button::BUTTON_4;
        case GLFW_MOUSE_BUTTON_5: return Mouse_button::BUTTON_5;
        case GLFW_MOUSE_BUTTON_6: return Mouse_button::BUTTON_6;
        default: return Mouse_button::UNKNOWN;
    }
}

inline unsigned int gl_memory_buffer_alignment_type(Buffer_type type) {
    switch (type) {
        case Buffer_type::UNIFORM:
            return GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT;
        case Buffer_type::STORAGE:
            return GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT;
        default:
            return GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT;
    }    
};

};