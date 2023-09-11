#pragma once

#include <DawnStar/Core/Core.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace DawnStar
{
class Log
{
public:
    static void Init();

    inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
    inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

private:
    static std::shared_ptr<spdlog::logger> s_CoreLogger;
    static std::shared_ptr<spdlog::logger> s_ClientLogger;
};
} // namespace DawnStar

// Core log macros
#define DS_CORE_TRACE(...)		::DawnStar::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define DS_CORE_INFO(...)		::DawnStar::Log::GetCoreLogger()->info(__VA_ARGS__)
#define DS_CORE_WARN(...)		::DawnStar::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define DS_CORE_ERROR(...)		::DawnStar::Log::GetCoreLogger()->error(__VA_ARGS__)
#define DS_CORE_FATAL(...)		::DawnStar::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client log macros
#define DS_TRACE(...)			::DawnStar::Log::GetClientLogger()->trace(__VA_ARGS__)
#define DS_INFO(...)			::DawnStar::Log::GetClientLogger()->info(__VA_ARGS__)
#define DS_WARN(...)			::DawnStar::Log::GetClientLogger()->warn(__VA_ARGS__)
#define DS_ERROR(...)			::DawnStar::Log::GetClientLogger()->error(__VA_ARGS__)
#define DS_FATAL(...)			::DawnStar::Log::GetClientLogger()->fatal(__VA_ARGS__)
