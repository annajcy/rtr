#pragma once

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
    
}

