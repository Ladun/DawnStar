#pragma once

#include <memory>

#include <DawnStar/Core/PlatformDetection.hpp>


// #define DS_DEBUG

#ifdef DS_DEBUG
	#if defined(DS_PLATFORM_WINDOWS)
		#define DS_DEBUGBREAK() __debugbreak()
	#elif defined(DS_PLATFORM_LINUX)
		#include <signal.h>
		#define DS_DEBUGBREAK() raise(SIGTRAP)
	#endif
	#define DS_ENABLE_ASSERTS
#endif

#define DS_EXPAND_MACRO(x) x
#define DS_STRINGIFY_MACRO(x) #x

#ifdef DS_ENABLE_ASSERTS

	// Alternatively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
	#define DS_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { DS##type##ERROR(msg, __VA_ARGS__); DS_DEBUGBREAK(); } }
	#define DS_INTERNAL_ASSERT_WITH_MSG(type, check, ...) DS_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define DS_INTERNAL_ASSERT_NO_MSG(type, check) DS_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", DS_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define DS_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define DS_INTERNAL_ASSERT_GET_MACRO(...) DS_EXPAND_MACRO( DS_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, DS_INTERNAL_ASSERT_WITH_MSG, DS_INTERNAL_ASSERT_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define DS_ASSERT(...) DS_EXPAND_MACRO( DS_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define DS_CORE_ASSERT(...) DS_EXPAND_MACRO( DS_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define DS_ASSERT(...)
	#define DS_CORE_ASSERT(...)
#endif

#define BIT(x) (1 << x)

#define DS_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace DawnStar
{
    template<typename T>
    using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
} // DawnStar
