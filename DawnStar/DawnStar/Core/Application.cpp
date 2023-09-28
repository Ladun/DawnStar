
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

        m_Window = CreateScope<Window>(WindowProps(name, width, height));
        m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

        Renderer::Init();

        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);
    }

	Application::~Application()
	{

		Renderer::Shutdown();
	}
	void Application::PushLayer(Layer* layer)
	{

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{

		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

    void Application::Close()
	{
		m_Running = false;
	}

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

        for(auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
        {
            if(e.Handled)
                break;
            (*it)->OnEvent(e);
        }
    }

	void Application::Run()
	{

		while (m_Running)
		{
			float time = (float)glfwGetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				{

					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timestep);
				}

				m_ImGuiLayer->Begin();
				{

					for (Layer* layer : m_LayerStack)
						layer->OnImGuiRender();
				}
				m_ImGuiLayer->End();
			}


			m_Window->OnUpdate();
		}
	}
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}

}// namespace DawnStar