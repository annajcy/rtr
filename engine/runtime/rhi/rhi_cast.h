#pragma once

#include "engine/runtime/enum.h"
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
    

};