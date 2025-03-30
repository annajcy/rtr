#pragma once

#include "engine/global/base.h" 
#include "engine/global/event.h"
#include "engine/runtime/enum.h"
#include <memory>

namespace rtr {

using Window_resize_event = Event<int, int>;
using Mouse_button_event = Event<Mouse_button, Key_action, unsigned int>;
using Mouse_move_event = Event<double, double>;
using Mouse_scroll_event = Event<double, double>;
using Key_event = Event<Key_code, Key_action, unsigned int>;

struct Clear_state {
    bool color{true};
    bool depth{true};
    bool stencil{true};

    float depth_clear_value{1.0f};
    unsigned int stencil_clear_value{0x00};
    glm::vec4 color_clear_value{0.0f, 0.0f, 0.0f, 1.0f};

    static Clear_state enabled() {
        return {
            true,
            true,
            true,
            1.0f,
            0x00,
            glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)
        };
    }
    
};


class RHI_window {
protected:
    int m_width{};
    int m_height{};
    std::string m_title{};
    Clear_state m_clear_state{};

    Window_resize_event m_window_resize_event{[&](int width, int height) {
        std::cout << "Viewport resized to: " << width << " " << height << std::endl;
		this->set_viewport(0, 0, width, height);
    }};

    Key_event m_key_event{[&](Key_code key_code, Key_action key_action, unsigned int repeat_count) {
        if (key_code == Key_code::ESCAPE && key_action == Key_action::PRESS) {
            std::cout << "Window close" << std::endl;
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
        std::string title,
        Clear_state clear_state
    ) : m_width(width), 
        m_height(height), 
        m_title(title),
        m_clear_state(clear_state) {}

    using Ptr = std::shared_ptr<RHI_window>;

    virtual ~RHI_window() = default;
    virtual void init() = 0;
    virtual void destroy() = 0;
    virtual void poll_events() = 0;
    virtual void swap_buffers() = 0;
    virtual bool is_active() = 0;

    virtual void set_viewport(int x, int y, int width, int height) = 0;
    virtual void deactivate() = 0;
    virtual void apply_clear_state() = 0;
    virtual void clear() = 0;

    void change_clear_state(std::function<void(Clear_state&)> changer) {
        changer(m_clear_state);
        apply_clear_state();
    }

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

    Window_resize_event& window_resize_event() { return m_window_resize_event; }
    Mouse_button_event& mouse_button_event() { return m_mouse_button_event; }
    Mouse_move_event& mouse_move_event() { return m_mouse_move_event; }
    Mouse_scroll_event& mouse_scroll_event() { return m_mouse_scroll_event; }
    Key_event& key_event() { return m_key_event; }

    Event<RHI_window*>& frame_end_event() { return m_frame_end_event; }
    Event<RHI_window*>& frame_begin_event() { return m_frame_begin_event; }

};

};


