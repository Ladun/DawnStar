
#include <dspch.hpp>
#include <DawnStar/Core/Application.hpp>

#include <DawnStar/Core/Log.hpp>
#include <DawnStar/Renderer/Renderer.hpp>

#include <DawnStar/Core/Input.hpp>

#include <GLFW/glfw3.h>

namespace DawnStar
{

	Application* Application::s_Instance = nullptr;

    Application::Application(const std::string& name,
							 uint32_t width,
							 uint32_t height)
    {
        DS_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        _window = CreateScope<Window>(WindowProps(name, width, height));
        _window->SetEventCallback(DS_BIND_EVENT_FN(OnEvent));

        Renderer::Init();

		_layerStack = new LayerStack();
        _imGuiLayer = new ImGuiLayer();
        PushOverlay(_imGuiLayer);
    }

	Application::~Application()
	{
		DS_PROFILE_SCOPE();

		PopOverlay(_imGuiLayer);
		delete _imGuiLayer;
		delete _layerStack;

		Renderer::Shutdown();
		
		OPTICK_SHUTDOWN();
	}

    void Application::Close()
	{
		_running = false;
	}

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(DS_BIND_EVENT_FN(OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(DS_BIND_EVENT_FN(OnWindowResize));

        for(auto it = _layerStack->rbegin(); it != _layerStack->rend(); ++it)
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
			DS_PROFILE_FRAME("MainThread")

			const auto time = static_cast<float>(glfwGetTime());
			Timestep timestep = time - _lastFrameTime;
			_lastFrameTime = time;

			ExecuteMainThreadQueue();

			if (!_minimized)
			{
				{
					DS_PROFILE_SCOPE("LayerStack OnUpdate")

					for (Layer* layer : *_layerStack)
						layer->OnUpdate(timestep);
				}

				_imGuiLayer->Begin();
				{
					DS_PROFILE_SCOPE("LayerStack OnImGuiRender")

					for (Layer* layer : *_layerStack)
						layer->OnImGuiRender();
				}
				_imGuiLayer->End();
			}


			Input::OnUpdate();
			_window->OnUpdate();
		}
	}
	
	void Application::SubmitToMainThread(const std::function<void()>& function)
	{
		std::scoped_lock<std::mutex> lock(_mainThreadQueueMutex);

		_mainThreadQueue.emplace_back(function);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		DS_PROFILE_SCOPE()

		_running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		DS_PROFILE_SCOPE()

		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			_minimized = true;
			return false;
		}

		_minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}

	void Application::ExecuteMainThreadQueue()
	{
		std::scoped_lock<std::mutex> lock(_mainThreadQueueMutex);

		for (const auto& func : _mainThreadQueue)
			func();

		_mainThreadQueue.clear();
	}

}// namespace DawnStar