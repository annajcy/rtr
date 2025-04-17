#pragma once

#include "engine/runtime/global/base.h" 
#include "engine/runtime/platform/rhi/rhi_window.h"
#include <memory>

namespace rtr {

struct Input_system_state {

    std::unordered_map<Key_mod, Key_action> key_mods{};
    std::unordered_map<Key_code, Key_action> keys{};
    std::unordered_map<Mouse_button, Key_action> mouse_buttons{};

    double mouse_x{};
    double mouse_y{};

    double mouse_dx{};
    double mouse_dy{};

    double mouse_scroll_dx{};
    double mouse_scroll_dy{};

    Input_system_state() = default;
    ~Input_system_state() = default;

    Key_action key_mod(Key_mod mod) const {
        if (key_mods.find(mod) == key_mods.end()) {
            return Key_action::RELEASE;
        }
        return key_mods.at(mod);
    }

    Key_action key(Key_code key) const {
        if (keys.find(key) == keys.end()) {
            return Key_action::RELEASE;
        }
        return keys.at(key);
    }

    Key_action mouse_button(Mouse_button button) const {
        if (mouse_buttons.find(button) == mouse_buttons.end()) {
            return Key_action::RELEASE;
        }
        return mouse_buttons.at(button);
    }

};

class Input_system {

private:
    Input_system_state m_state{};

public:
    Input_system(const std::shared_ptr<RHI_window>& window) {
        window->mouse_move_event().add([&](double x, double y) {
            this->update_mouse_position(x, y);
        });

        window->mouse_scroll_event().add([&](double x, double y) {
            this->update_mouse_scroll(x, y);
        });

        window->key_event().add([&](Key_code key, Key_action action, int mod) {
            this->update_key(key, action, mod);
        });

        window->mouse_button_event().add([&](Mouse_button button, Key_action action, int mod) {
            this->update_mouse_button(button, action);
        });

        window->frame_end_event().add([&](RHI_window* window){
            this->reset_deltas();
        });
    }

    ~Input_system() = default;

    static std::shared_ptr<Input_system> create(const std::shared_ptr<RHI_window>& window) {
        return std::make_shared<Input_system>(window);
    }
    
    void update_mouse_position(double x, double y) {
        m_state.mouse_dx = x - m_state.mouse_dx;
        m_state.mouse_dy = y - m_state.mouse_dy;
        m_state.mouse_x = x;
        m_state.mouse_y = y;
    }

    void update_mouse_scroll(double x, double y) {
        m_state.mouse_scroll_dx = x;
        m_state.mouse_scroll_dy = y;
    }

    void update_key(Key_code key, Key_action action, int mod) {
        
        for (int i = mod, cnt = 0; i > 0; i >>= 1, cnt ++) {
            if (i & 1) {
                m_state.key_mods[static_cast<Key_mod>(cnt)] = action;
            }
        }

        m_state.keys[key] = action;
    }

    void update_mouse_button(Mouse_button button, Key_action action) { 
        m_state.mouse_buttons[button] = action; 
    }

    void reset_deltas() {
        m_state.mouse_dx = 0.0;
        m_state.mouse_dy = 0.0;
        m_state.mouse_scroll_dx = 0.0;
        m_state.mouse_scroll_dy = 0.0;
    }
    
    const Input_system_state& state() const {
        return m_state;
    }

};

};