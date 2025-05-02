#pragma once

#include "engine/runtime/global/base.h" 

#include "../rhi_window.h"
#include "rhi_cast_opengl.h"


namespace rtr {

class RHI_window_OpenGL : public RHI_window {

protected:
    GLFWwindow* m_window{};

public:
    RHI_window_OpenGL(
        unsigned int width,
        unsigned int height,
        std::string title
    ) : RHI_window(width, height, title) {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
        
        if (m_window == nullptr) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }

        glfwSetWindowUserPointer(m_window, this);
        glfwMakeContextCurrent(m_window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return;
        }

        glfwSetFramebufferSizeCallback(m_window, window_resize_callback);
        glfwSetMouseButtonCallback(m_window, mouse_button_callback);
        glfwSetCursorPosCallback(m_window, mouse_move_callback);
        glfwSetScrollCallback(m_window, mouse_scroll_callback);
        glfwSetKeyCallback(m_window, key_callback);

        glfwGetFramebufferSize(window(), &m_width, &m_height);
        set_viewport(0, 0, m_width, m_height);
        
    }

    ~RHI_window_OpenGL() override {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    void set_viewport(int x, int y, int width, int height) override {
        glViewport(x, y, width, height);
    }

    void deactivate() override {
        glfwSetWindowShouldClose(m_window, true);
    }

    GLFWwindow* window() {
        return m_window;
    }
    void poll_events() override {
        glfwPollEvents();
    }

    void swap_buffers() override {
        glfwSwapBuffers(m_window);
    }

    bool is_active() override {
        return !glfwWindowShouldClose(m_window);
    }

    static void window_resize_callback(GLFWwindow* window, int width, int height) {
        auto* self = static_cast<RHI_window_OpenGL*>(glfwGetWindowUserPointer(window));
        if (self) {
            self->m_width = width;
            self->m_height = height;
            self->m_window_resize_event.execute(width, height);
        }
    }

    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        auto* self = static_cast<RHI_window_OpenGL*>(glfwGetWindowUserPointer(window));
        if (self) {
            self->m_mouse_button_event.execute(gl_to_rhi_mouse_button_map(button), gl_to_rhi_key_action_map(action), mods);
        }
    }

    static void mouse_move_callback(GLFWwindow* window, double xpos, double ypos) {
        auto* self = static_cast<RHI_window_OpenGL*>(glfwGetWindowUserPointer(window));
        if (self) {
            self->m_mouse_move_event.execute(xpos, ypos);
        }   
    }

    static void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        auto* self = static_cast<RHI_window_OpenGL*>(glfwGetWindowUserPointer(window));
        if (self) {
            self->m_mouse_scroll_event.execute(xoffset, yoffset);
        }
    }

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto* self = static_cast<RHI_window_OpenGL*>(glfwGetWindowUserPointer(window));
        if (self) {
            self->m_key_event.execute(gl_to_rhi_key_map(key), gl_to_rhi_key_action_map(action), mods);
        }
    }

};

};


