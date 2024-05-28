#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace DawnStar
{
class Log
{
public:
    static void Init();

    inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return _coreLogger; }
    inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return _clientLogger; }

private:
    static std::shared_ptr<spdlog::logger> _coreLogger;
    static std::shared_ptr<spdlog::logger> _clientLogger;
};
} // namespace DawnStar

// Core log macros
#define DS_CORE_TRACE(...)		::DawnStar::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define DS_CORE_INFO(...)		::DawnStar::Log::GetCoreLogger()->info(__VA_ARGS__)
#define DS_CORE_DEBUG(...)		::DawnStar::Log::GetCoreLogger()->debug(__VA_ARGS__)
#define DS_CORE_WARN(...)		::DawnStar::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define DS_CORE_ERROR(...)		::DawnStar::Log::GetCoreLogger()->error(__VA_ARGS__)
#define DS_CORE_FATAL(...)		::DawnStar::Log::GetCoreLogger()->fatal(__VA_ARGS__)
#define DS_CORE_CRITICAL(...)		::DawnStar::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define DS_APP_TRACE(...)			::DawnStar::Log::GetClientLogger()->trace(__VA_ARGS__)
#define DS_APP_INFO(...)			::DawnStar::Log::GetClientLogger()->info(__VA_ARGS__)
#define DS_APP_DEBUG(...)			::DawnStar::Log::GetClientLogger()->debug(__VA_ARGS__)
#define DS_APP_WARN(...)			::DawnStar::Log::GetClientLogger()->warn(__VA_ARGS__)
#define DS_APP_ERROR(...)			::DawnStar::Log::GetClientLogger()->error(__VA_ARGS__)
#define DS_APP_FATAL(...)			::DawnStar::Log::GetClientLogger()->fatal(__VA_ARGS__)
#define DS_APP_CRITICAL(...)			::DawnStar::Log::GetClientLogger()->critical(__VA_ARGS__)

#include <DawnStar/Core/Core.hpp>