#pragma once

#include <dspch.hpp>

#include <DawnStar/Core/Core.hpp>
#include <DawnStar/Events/Event.hpp>
#include <DawnStar/Renderer/GraphicsContext.hpp>

#include <GLFW/glfw3.h>

namespace DawnStar
{
    struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "DawnStar Simple Game",
					uint32_t width = 1600,
					uint32_t height = 900)
			: Title(title), Width(width), Height(height)
		{

		}
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		Window(const WindowProps& props);
		~Window();

		void OnUpdate();
		
		inline uint32_t GetWidth() const { return _data.Width; }
		inline uint32_t GetHeight() const { return _data.Height; }

		inline void SetEventCallback(const EventCallbackFn& callback) { _data.EventCallback = callback; }
		void SetVSync(bool enabled);
		bool IsVSync() const;

		inline void* GetNativeWindow() const { return _window; }

	private:
		void Init(const WindowProps& props);
		void Shutdown();
	private:
		GLFWwindow* _window;
		Scope<GraphicsContext> _context;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData _data;
	};
} // namespace DawnStar
