#pragma once

#include "engine/global/base.h" 
#include "engine/global/event.h"
#include "engine/runtime/enum.h"
#include "engine/runtime/rhi/opengl/rhi_cast_opengl.h"
#include "engine/runtime/rhi/rhi_window.h"


namespace rtr {

class RHI_window_OpenGL : public RHI_window {

protected:
    GLFWwindow* m_window{};

public:
    RHI_window_OpenGL(
        unsigned int width,
        unsigned int height,
        std::string title,
        const Clear_state& clear_state
    ) : RHI_window(width, height, title, clear_state) {
        init();
    }

    ~RHI_window_OpenGL() override {
        destroy();
    }

    void set_viewport(int x, int y, int width, int height) override {
        glViewport(x, y, width, height);
    }

    void deactivate() override {
        glfwSetWindowShouldClose(m_window, true);
    }

    void init() override {
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

    GLFWwindow* window() {
        return m_window;
    }

    void destroy() override {
        glfwDestroyWindow(m_window);
        glfwTerminate();
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

    void apply_clear_state() override {
        glClearColor(
            m_clear_state.color_clear_value.r,
            m_clear_state.color_clear_value.g,
            m_clear_state.color_clear_value.b,
            m_clear_state.color_clear_value.a
        );

        glClearDepth(m_clear_state.depth_clear_value);
        glClearStencil(m_clear_state.stencil_clear_value);
    }

    void clear() override {

        auto clear_mask = 0;
        if (m_clear_state.color) clear_mask |= GL_COLOR_BUFFER_BIT;
        if (m_clear_state.depth) clear_mask |= GL_DEPTH_BUFFER_BIT;
        if (m_clear_state.stencil) clear_mask |= GL_STENCIL_BUFFER_BIT;

        glClear(clear_mask);
        
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


