#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <string>

namespace rtr {

template <typename ...Args>
class Event {
private:
    std::vector<std::function<void(Args...)>> m_events{};

public:

    Event() = default;
    Event(const std::vector<std::function<void(Args...)>> &actions) : m_events(actions) {}

    Event(const std::function<void(Args...)> &action) {
        m_events.push_back(action);
    }

    void add(const std::function<void(Args...)>& action) {
        m_events.push_back(action);
    }

    void clear() {
        m_events.clear();
    }

    void remove(const std::function<void(Args...)> &action) {
        m_events.erase(std::remove(m_events.begin(), m_events.end(), action), m_events.end());
    }

    void execute(Args... args) {
        for (const auto& action : m_events) {
            action(args...);
        }
    }
};

template <typename...Args>
class Event_center {
private:
    Event_center() = default;
    std::unordered_map<std::string, Event<Args...>> event_map{};
public:
    void register_event(const std::string& event_name, Event<Args...> event) {
        event_map[event_name] = event;
    }

    void unregister_event(const std::string& event_name) {
        event_map.erase(event_name);
    }

    void add_action(const std::string& event_name, const std::function<void(Args...)>& action) {
        if (event_map.contains(event_name)) {
            event_map[event_name].add(action);
        } else {
            throw std::runtime_error("Event not found: " + event_name);
        }
    }

    void remove_action(const std::string& event_name, const std::function<void(Args...)>& action) {
        if (event_map.contains(event_name)) {
            event_map[event_name].remove(action);
        } else {
            throw std::runtime_error("Event not found: " + event_name);
        }

    }

    void trigger_event(const std::string& event_name, Args... args) {
        if (event_map.contains(event_name)) {
            event_map[event_name].execute(args...);
        } else {
            throw std::runtime_error("Event not found: " + event_name);
        }
    }
};


    
}

