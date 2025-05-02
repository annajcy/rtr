#pragma once

namespace rtr {
    
enum class Key_mod {
    SHIFT,
    CTRL,
    ALT,
    SUPER
};

enum class Light_type {
    AMBIENT,
    DIRECTIONAL,
    POINT,
    SPOT,
    AREA,
    UNKNOWN
};

enum class Node_type {
    NODE,
    MESH,
    SCENE,
    LIGHT,
    CAMERA
};

enum class Key_action {
    PRESS = 0,
    RELEASE = 1,
    REPEAT = 2,
    UNKNOWN = 0xFF
};

enum class Key_code {
    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,
    NUM_0 = 48,
    NUM_1 = 49,
    NUM_2 = 50,
    NUM_3 = 51,
    NUM_4 = 52,
    NUM_5 = 53,
    NUM_6 = 54,
    NUM_7 = 55,
    NUM_8 = 56,
    NUM_9 = 57,
    SPACE = 32,
    APOSTROPHE = 39,
    COMMA = 44,
    MINUS = 45,
    PERIOD = 46,
    SLASH = 47,
    SEMICOLON = 59,
    EQUAL = 61,
    LEFT_BRACKET = 91,
    BACKSLASH = 92,
    RIGHT_BRACKET = 93,
    GRAVE_ACCENT = 96,
    ESCAPE = 256,
    ENTER = 257,
    TAB = 258,
    BACKSPACE = 259,
    INSERT = 260,
    DELETE = 261,
    RIGHT = 262,
    LEFT = 263,
    DOWN = 264,
    UP = 265,
    PAGE_UP = 266,
    PAGE_DOWN = 267,
    HOME = 268,
    END = 269,
    CAPS_LOCK = 280,
    SCROLL_LOCK = 281,
    NUM_LOCK = 282,
    PRINT_SCREEN = 283,
    PAUSE = 284,
    F1 = 290,
    F2 = 291,
    F3 = 292,
    F4 = 293,
    F5 = 294,
    F6 = 295,
    F7 = 296,
    F8 = 297,
    F9 = 298,
    F10 = 299,
    F11 = 300,
    F12 = 301,
    KP_0 = 320,
    KP_1 = 321,
    KP_2 = 322,
    KP_3 = 323,
    KP_4 = 324,
    KP_5 = 325,
    KP_6 = 326,
    KP_7 = 327,
    KP_8 = 328,
    KP_9 = 329,
    KP_DECIMAL = 330,
    KP_DIVIDE = 331,
    KP_MULTIPLY = 332,
    KP_SUBTRACT = 333,
    KP_ADD = 334,
    KP_ENTER = 335,
    KP_EQUAL = 336,
    LEFT_SHIFT = 340,
    LEFT_CONTROL = 341,
    LEFT_ALT = 342,
    LEFT_SUPER = 343,
    RIGHT_SHIFT = 344,
    RIGHT_CONTROL = 345,
    RIGHT_ALT = 346,
    RIGHT_SUPER = 347,
    MENU = 348,
    UNKNOWN = 0xFF
};

enum class Mouse_button {
    LEFT = 0,
    RIGHT = 1,
    MIDDLE = 2,
    BUTTON_4 = 3,
    BUTTON_5 = 4,
    BUTTON_6 = 5,
    UNKNOWN = 0xFF
};

enum class Texture_buffer_type {
    UNSIGNED_BYTE,
    UNSIGNED_INT,
    UNSIGNED_INT_24_8,
    FLOAT
};

enum class Texture_type {
    TEXTURE_2D,
    TEXTURE_CUBEMAP,
    TEXTURE_2D_ARRAY
};

enum class Texture_internal_format {
    RGB,
    RGB_ALPHA,
    RGB_ALPHA_16F,
    RGB_ALPHA_32F,
    DEPTH_STENCIL,
    DEPTH_STENCIL_24_8,
    DEPTH,
    DEPTH_32F,
    SRGB_ALPHA,
    SRGB
};

enum class Texture_external_format {
    RGB,
    RGB_ALPHA,
    DEPTH_STENCIL,
    SRGB_ALPHA,
    SRGB
};

enum class Texture_wrap {
    REPEAT,
    MIRRORED_REPEAT,
    CLAMP_TO_EDGE,
    CLAMP_TO_BORDER,
};

enum class Texture_wrap_target {
    U, V, W
};

enum class Texture_filter {
    NEAREST,
    LINEAR,
    NEAREST_MIPMAP_NEAREST,
    LINEAR_MIPMAP_NEAREST,
    NEAREST_MIPMAP_LINEAR,
    LINEAR_MIPMAP_LINEAR,
};

enum class Texture_filter_target {
    MIN,
    MAG,
};

enum class Texture_cubemap_face {
    RIGHT,
    LEFT,
    TOP,
    BOTTOM,
    BACK,
    FRONT,
};

enum class Depth_function {
    NEVER,
    LESS,
    EQUAL,
    LESS_EQUAL,
    GREATER,
    NOT_EQUAL,
    GREATER_EQUAL,
    ALWAYS,
};

enum class Stencil_operation {
    KEEP,
    ZERO,
    REPLACE,
    INCR,
    DECR,
    INVERT,
    INCR_WRAP,
    DECR_WRAP,  
};

enum class Stencil_function {
    NEVER,
    LESS,
    EQUAL,
    LESS_EQUAL,
    GREATER,
    NOT_EQUAL,
    GREATER_EQUAL,
    ALWAYS
};

enum class Blend_factor {
    ZERO,
    ONE,
    SRC_COLOR,
    ONE_MINUS_SRC_COLOR,
    DST_COLOR,
    ONE_MINUS_DST_COLOR,
    SRC_ALPHA,
    ONE_MINUS_SRC_ALPHA,
    DST_ALPHA,
    ONE_MINUS_DST_ALPHA
};

enum class Blend_operation {
    ADD,
    SUBTRACT,
    REVERSE_SUBTRACT,
    MIN,
    MAX,
};

enum class Cull_mode {
    NONE,
    FRONT,
    BACK,
    FRONT_AND_BACK,
};

enum class Front_face {
    COUNTER_CLOCKWISE,
    CLOCKWISE
};

enum class Uniform_data_type {
    FLOAT, INT, BOOL,
    VEC2, VEC3, VEC4,
    IVEC2, IVEC3, IVEC4,
    MAT2, MAT3, MAT4,
    SAMPLER,
    UNKNOWN,
};

enum class Shader_type {
    VERTEX,
    FRAGMENT,
    GEOMETRY,
    COMPUTE,
    TESSELATION_CONTROL,
    TESSELATION_EVALUATION,
    UNKNOWN,
};

enum class API_type {
    OPENGL,
    DIRECTX,
    VULKAN
};

enum class Buffer_type {
    VERTEX,
    ELEMENT,
    UNIFORM,
    STORAGE,
};

enum class Buffer_usage {
    STATIC,
    DYNAMIC,
    STREAM,
};

enum class Buffer_iterate_type {
    PER_VERTEX,
    PER_INSTANCE,
};

enum class Buffer_data_type {
    FLOAT,
    INT,
    UINT,
    BOOL,
};

enum class Material_type {
    PHONG,
    PBR,
    SKYBOX_CUBEMAP,
    SKYBOX_SPHERICAL,
    GAMMA,
    SHADOW_CASTER,
    TEST
};

enum class Draw_mode {
    POINTS,
    LINES,
    LINE_LOOP,
    LINE_STRIP,
    TRIANGLES,
    TRIANGLE_STRIP,
    TRIANGLE_FAN,
};
    
}