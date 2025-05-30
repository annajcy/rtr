#pragma once


#include "engine/runtime/tool/event.h"
#include "engine/runtime/tool/logger.h"
#include "engine/runtime/platform/rhi/rhi_imgui.h"
#include <memory>

namespace rtr {

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

enum class Key_mod {
    SHIFT,
    CTRL,
    ALT,
    SUPER
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

enum class Key_action {
    PRESS = 0,
    RELEASE = 1,
    REPEAT = 2,
    UNKNOWN = 0xFF
};


using Window_resize_event = Event<int, int>;
using Mouse_button_event = Event<Mouse_button, Key_action, unsigned int>;
using Mouse_move_event = Event<double, double>;
using Mouse_scroll_event = Event<double, double>;
using Key_event = Event<Key_code, Key_action, unsigned int>;

class RHI_window {

protected:
    int m_width{};
    int m_height{};
    std::string m_title{};
    std::shared_ptr<RHI_imgui> m_imgui{};

    Window_resize_event m_window_resize_event{[&](int width, int height) {
        Log_sys::get_instance()->log(Logging_system::Level::info, "Window resized: {} {}", width, height);
		this->set_viewport(0, 0, width, height);
    }};

    Key_event m_key_event{[&](Key_code key_code, Key_action key_action, unsigned int repeat_count) {
        if (key_code == Key_code::ESCAPE && key_action == Key_action::PRESS) {
            Log_sys::get_instance()->log(Logging_system::Level::info, "Window closed");
            this->deactivate();
        }
    }};

    Mouse_button_event m_mouse_button_event{};
    Mouse_move_event m_mouse_move_event{};
    Mouse_scroll_event m_mouse_scroll_event{};

    Event<RHI_window*> m_frame_end_event{};
    Event<RHI_window*> m_frame_begin_event{};

public:

    RHI_window(
        int width, 
        int height, 
        std::string title
    ) : m_width(width), 
        m_height(height), 
        m_title(title) {}

    virtual ~RHI_window() = default;
    virtual bool is_active() = 0;

    virtual void set_viewport(int x, int y, int width, int height) = 0;
    virtual void deactivate() = 0;

    void on_frame_begin() {
        poll_events();
        swap_buffers();
        m_frame_begin_event.execute(this);
    }

    void on_frame_end() {
        m_frame_end_event.execute(this);
    }

    const int& width() { return m_width; }
    const int& height() { return m_height; }
    const std::string& title() { return m_title; }
    std::shared_ptr<RHI_imgui>& imgui() { return m_imgui; }

    Window_resize_event& window_resize_event() { return m_window_resize_event; }
    Mouse_button_event& mouse_button_event() { return m_mouse_button_event; }
    Mouse_move_event& mouse_move_event() { return m_mouse_move_event; }
    Mouse_scroll_event& mouse_scroll_event() { return m_mouse_scroll_event; }
    Key_event& key_event() { return m_key_event; }

    Event<RHI_window*>& frame_end_event() { return m_frame_end_event; }
    Event<RHI_window*>& frame_begin_event() { return m_frame_begin_event; }

private:
    virtual void poll_events() = 0;
    virtual void swap_buffers() = 0;

};

};


