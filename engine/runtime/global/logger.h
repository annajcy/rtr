#pragma once

#include "engine/runtime/global/singleton.h"
#include "spdlog/logger.h"
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <cstdint>
#include <stdexcept>
#include <format>

namespace rtr {

class Logging_system {
private:
    std::shared_ptr<spdlog::logger> m_logger{};

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
    
        m_logger = std::make_shared<spdlog::logger>(
            "logger",
            spdlog::sinks_init_list{console_sink}
        );
        
        m_logger->set_level(spdlog::level::trace);
        spdlog::register_logger(m_logger);
    }

    ~Logging_system() {}

    template<typename... TARGS>
    void log(Level level, std::format_string<TARGS...> fmt, TARGS&&... args) {
        // 先统一格式化消息
        std::string message = std::format(fmt, std::forward<TARGS>(args)...);
        
        switch(level) {
            case Level::debug:
                m_logger->debug(message);
                break;
            case Level::info:
                m_logger->info(message);
                break;
            case Level::warn:
                m_logger->warn(message);
                break;
            case Level::error:
                m_logger->error(message);
                break;
            case Level::fatal: {
                std::string error_msg = std::format("FATAL: {}", message);
                m_logger->critical(error_msg);
                throw std::runtime_error(error_msg);
                break;
            }
        }
    }

};

using Log_sys = Singleton<Logging_system>;

}