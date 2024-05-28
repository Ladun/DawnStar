#include <dspch.hpp>

#include <DawnStar/Core/Window.hpp>
#include <DawnStar/Core/Input.hpp>

#include <DawnStar/Events/ApplicationEvent.hpp>
#include <DawnStar/Events/KeyEvent.hpp>
#include <DawnStar/Events/MouseEvent.hpp>

namespace DawnStar
{
    static uint8_t s_GLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char* description)
	{
		DS_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window::Window(const WindowProps& props)
	{
		DS_PROFILE_SCOPE();

		Init(props);
	}

	Window::~Window()
	{
		DS_PROFILE_SCOPE();

		Shutdown();
	}

	void Window::Init(const WindowProps& props)
	{
		DS_PROFILE_SCOPE();

		_data.Title = props.Title;
		_data.Width = props.Width;
		_data.Height = props.Height;


		DS_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);


		if (s_GLFWWindowCount == 0)
		{
			DS_PROFILE_SCOPE("glfwInit");
			int success = glfwInit();
			DS_CORE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		{
			DS_PROFILE_SCOPE("glfwCreateWindow");
			_window = glfwCreateWindow((int)props.Width, (int)props.Height, _data.Title.c_str(), nullptr, nullptr);
			++s_GLFWWindowCount;
		}

		_context = CreateScope<GraphicsContext>(_window);
		_context->Init();

		glfwSetWindowUserPointer(_window, &_data);
		SetVSync(true);

		// Set GLFW callback;
		glfwSetWindowSizeCallback(_window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;

				WindowResizeEvent event(width, height);
				data.EventCallback(event);
			});

		glfwSetWindowCloseCallback(_window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

		glfwSetKeyCallback(_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				
				Input::SetUpKey(key, action!=GLFW_RELEASE);

				switch (action)
				{
					case GLFW_PRESS: 
					{
						KeyPressedEvent event(key, 0);
						data.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						KeyReleasedEvent event(key);
						data.EventCallback(event);
						break;
					}
					case GLFW_REPEAT:
					{
						KeyPressedEvent event(key, 1);
						data.EventCallback(event);
						break;
					}
				}
			});

		glfwSetCharCallback(_window, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				Input::SetKeyCode(keycode);

				KeyTypedEvent event(keycode);
				data.EventCallback(event);
			});

		glfwSetMouseButtonCallback(_window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				Input::SetUpMouse(button, action==GLFW_PRESS);

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetScrollCallback(_window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);

			});

		glfwSetCursorPosCallback(_window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);
			});

	}

	void Window::Shutdown()
	{
		DS_PROFILE_SCOPE();

		glfwDestroyWindow(_window);

		--s_GLFWWindowCount;
		if (s_GLFWWindowCount == 0)
		{
			glfwTerminate();
		}
	}

	void Window::OnUpdate()
	{
		DS_PROFILE_SCOPE();

		glfwPollEvents();
		_context->SwapBuffer();
	}

	void Window::SetVSync(bool enabled)
	{
		DS_PROFILE_SCOPE();

		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		_data.VSync = enabled;
	}

	bool Window::IsVSync() const
	{
		return _data.VSync;
	}
} // namespace DawnStar
