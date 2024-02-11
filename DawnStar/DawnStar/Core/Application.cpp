
#include <DawnStar/dspch.hpp>
#include <DawnStar/Core/Application.hpp>

#include <DawnStar/Core/Log.hpp>
#include <DawnStar/Renderer/Renderer.hpp>

#include <DawnStar/Core/Input.hpp>

#include <GLFW/glfw3.h>

namespace DawnStar
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

    Application::Application(const std::string& name,
							 uint32_t width,
							 uint32_t height)
    {
        DS_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        _window = CreateScope<Window>(WindowProps(name, width, height));
        _window->SetEventCallback(BIND_EVENT_FN(OnEvent));

        Renderer::Init();

        _imGuiLayer = new ImGuiLayer();
        PushOverlay(_imGuiLayer);
    }

	Application::~Application()
	{

		Renderer::Shutdown();
	}
	void Application::PushLayer(Layer* layer)
	{

		_layerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{

		_layerStack.PushOverlay(layer);
		layer->OnAttach();
	}

    void Application::Close()
	{
		_running = false;
	}

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

        for(auto it = _layerStack.rbegin(); it != _layerStack.rend(); ++it)
        {
            if(e.Handled)
                break;
            (*it)->OnEvent(e);
        }
    }

	void Application::Run()
	{

		while (_running)
		{
			float time = (float)glfwGetTime();
			Timestep timestep = time - _lastFrameTime;
			_lastFrameTime = time;

			if (!_minimized)
			{
				{

					for (Layer* layer : _layerStack)
						layer->OnUpdate(timestep);
				}

				_imGuiLayer->Begin();
				{

					for (Layer* layer : _layerStack)
						layer->OnImGuiRender();
				}
				_imGuiLayer->End();
			}


			Input::OnUpdate();
			_window->OnUpdate();
		}
	}
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		_running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			_minimized = true;
			return false;
		}

		_minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}

}// namespace DawnStar