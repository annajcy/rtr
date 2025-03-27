#pragma once

#include "engine/runtime/enum.h"
namespace rtr {
    
inline constexpr unsigned int get_buffer_attribute_size(Buffer_attribute_type type) {
    switch (type) {
    case Buffer_attribute_type::FLOAT: return 4;
    case Buffer_attribute_type::INT: return 4;
    case Buffer_attribute_type::UINT: return 4;
    case Buffer_attribute_type::BOOL: return 1;
    default: return 0;
    }
}
    

};