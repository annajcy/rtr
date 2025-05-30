#pragma once

#include "engine/runtime/tool/base.h" 

namespace rtr {

inline void gl_check_error() {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        if (error == GL_INVALID_ENUM) {
            std::cerr << "OpenGL error: GL_INVALID_ENUM" << std::endl;
        } else if (error == GL_INVALID_VALUE) {
            std::cerr << "OpenGL error: GL_INVALID_VALUE" << std::endl;
        } else if (error == GL_INVALID_OPERATION) {
            std::cerr << "OpenGL error: GL_INVALID_OPERATION" << std::endl;
        } else if (error == GL_STACK_OVERFLOW) {
            std::cerr << "OpenGL error: GL_STACK_OVERFLOW" << std::endl;
        } else if (error == GL_STACK_UNDERFLOW) {
            std::cerr << "OpenGL error: GL_STACK_UNDERFLOW" << std::endl;
        } else if (error == GL_OUT_OF_MEMORY) {
            std::cerr << "OpenGL error: GL_OUT_OF_MEMORY" << std::endl;
        } else if (error == GL_INVALID_FRAMEBUFFER_OPERATION) {
            std::cerr << "OpenGL error: GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl; 
        } else if (error == GL_CONTEXT_LOST) {
            std::cerr << "OpenGL error: GL_CONTEXT_LOST" << std::endl;
        } else {
            std::cerr << "OpenGL error: Unknown error code" << std::endl;
        }
    }
    assert(error == GL_NO_ERROR);
}

};