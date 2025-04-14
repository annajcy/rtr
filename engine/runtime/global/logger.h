#pragma once

#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <cstdint>
#include <stdexcept>

namespace rtr {


class Logging_system {
public:
    enum class Level : uint8_t {
        debug,
        info,
        warn,
        error,
        fatal
    };
    
    Logging_system() {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::trace);
        console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

        spdlog::init_thread_pool(8192, 1);
        
        m_logger = std::make_shared<spdlog::async_logger>(
            "RTLogger",
            console_sink,
            spdlog::thread_pool(),
            spdlog::async_overflow_policy::block
        );
        
        m_logger->set_level(spdlog::level::trace);
        spdlog::register_logger(m_logger);
    }

    ~Logging_system() {
        m_logger->flush();
        spdlog::drop_all();
    }

    template<typename... TARGS>
    void log(Level level, TARGS&&... args) {
        switch(level) {
            case Level::debug:
                m_logger->debug(std::forward<TARGS>(args)...);
                break;
            case Level::info:
                m_logger->info(std::forward<TARGS>(args)...);
                break;
            case Level::warn:
                m_logger->warn(std::forward<TARGS>(args)...);
                break;
            case Level::error:
                m_logger->error(std::forward<TARGS>(args)...);
                break;
            case Level::fatal:
                m_logger->critical(std::forward<TARGS>(args)...);
                fatal(std::forward<TARGS>(args)...);
                break;
        }
    }

private:
    template<typename... TARGS>
    void fatal(TARGS&&... args) {
        const std::string format_str = fmt::format(std::forward<TARGS>(args)...);
        throw std::runtime_error(format_str);
    }

    std::shared_ptr<spdlog::logger> m_logger;
};

}