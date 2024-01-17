#pragma once

#include <memory>

#include <DawnStar/Core/PlatformDetection.hpp>

#ifdef DS_DEBUG
	#if defined(DS_PLATFORM_WINDOWS)
		#define DS_DEBUGBREAK() __debugbreak()
	#elif defined(DS_PLATFORM_LINUX)
		#include <signal.h>
		#define DS_DEBUGBREAK() raise(SIGTRAP)
	#endif
	#define DS_ENALBE_ASSERTS
#endif

#ifdef DS_ENALBE_ASSERTS
	#define DS_ASSERT(x, ...) { if(!(x)) { DS_APP_ERROR("Assertion Failed: {0}", __VA_ARGS__); DS_DEBUGBREAK();}}
	#define DS_CORE_ASSERT(x, ...) { if(!(x)) { DS_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); DS_DEBUGBREAK();}}
#else
	#define DS_ASSERT(x, ...) 
	#define DS_CORE_ASSERT(x, ...) 
#endif

#define BIT(x) (1 << x)

#define DS_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

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
