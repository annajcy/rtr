#pragma once

#include "engine/runtime/global/base.h" 

namespace rtr {
    
inline constexpr unsigned int sizeof_buffer_data(Buffer_data_type type) {
    switch (type) {
    case Buffer_data_type::FLOAT: return 4;
    case Buffer_data_type::INT: return 4;
    case Buffer_data_type::UINT: return 4;
    case Buffer_data_type::BOOL: return 1;
    default: return 0;
    }
}

template<typename U>
inline constexpr Uniform_data_type get_uniform_data_type() {
    if constexpr (std::is_same_v<U, float>) {
        return Uniform_data_type::FLOAT;
    } else if constexpr (std::is_same_v<U, int>) {
        return Uniform_data_type::INT;
    } else if constexpr (std::is_same_v<U, bool>) {
        return Uniform_data_type::BOOL;
    } else if constexpr (std::is_same_v<U, glm::vec2>) {
        return Uniform_data_type::VEC2;
    } else if constexpr (std::is_same_v<U, glm::vec3>) {
        return Uniform_data_type::VEC3;
    } else if constexpr (std::is_same_v<U, glm::vec4>) {
        return Uniform_data_type::VEC4;
    } else if constexpr (std::is_same_v<U, glm::mat2>) {
        return Uniform_data_type::MAT2;
    } else if constexpr (std::is_same_v<U, glm::mat3>) {
        return Uniform_data_type::MAT3;
    } else if constexpr (std::is_same_v<U, glm::mat4>) {
        return Uniform_data_type::MAT4;
    } else {
        return Uniform_data_type::UNKNOWN;
    }
}

    

};