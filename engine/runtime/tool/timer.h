#pragma once
#include <chrono>
#include <string>

namespace rtr {
class Timer {
private:
    using Clock = std::chrono::high_resolution_clock;
    Clock::time_point m_start;
    Clock::time_point m_pause_point;
    std::chrono::microseconds m_paused_total{0};
    bool m_is_paused = false;

public:
    // 启动/重置计时器
    void start() {
        m_start = Clock::now();
        m_paused_total = std::chrono::microseconds(0);
        m_is_paused = false;
    }

    // 暂停计时
    void pause() {
        if(!m_is_paused) {
            m_pause_point = Clock::now();
            m_is_paused = true;
        }
    }

    // 继续计时
    void resume() {
        if(m_is_paused) {
            m_paused_total += std::chrono::duration_cast<std::chrono::microseconds>(
                Clock::now() - m_pause_point);
            m_is_paused = false;
        }
    }

    // 获取经过时间（毫秒）
    template<typename T = float>
    T elapsed_ms() const {
        auto duration = Clock::now() - m_start - m_paused_total;
        return std::chrono::duration_cast<std::chrono::duration<T, std::milli>>(duration).count();
    }

    // 获取经过时间（秒）
    template<typename T = float>
    T elapsed_sec() const {
        auto duration = Clock::now() - m_start - m_paused_total;
        return std::chrono::duration_cast<std::chrono::duration<T>>(duration).count();
    }

    // 获取带单位的字符串表示
    std::string format() const {
        const auto ms = elapsed_ms();
        if(ms < 1000) return std::to_string(ms) + "ms";
        return std::to_string(ms/1000) + "s"; 
    }
};
}