#pragma once

#include "engine/runtime/tool/base.h" 

#include "../rhi_window.h"
#include "engine/runtime/platform/rhi/opengl/rhi_imgui_opengl.h"

namespace rtr {

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

        m_imgui = RHI_imgui_OpenGL::create(m_window);
        
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
            if (self->imgui()->is_io_captured()) {
                return;
            }
            self->m_mouse_button_event.execute(gl_to_rhi_mouse_button_map(button), gl_to_rhi_key_action_map(action), mods);
        }
    }

    static void mouse_move_callback(GLFWwindow* window, double xpos, double ypos) {
        auto* self = static_cast<RHI_window_OpenGL*>(glfwGetWindowUserPointer(window));
        if (self) {
            if (self->imgui()->is_io_captured()) {
                return;
            }
            self->m_mouse_move_event.execute(xpos, ypos);
        }   
    }

    static void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        auto* self = static_cast<RHI_window_OpenGL*>(glfwGetWindowUserPointer(window));
        if (self) {
            if (self->imgui()->is_io_captured()) {
                return;
            }
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


