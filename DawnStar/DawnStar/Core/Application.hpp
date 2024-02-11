#pragma once

#include <DawnStar/Core/Window.hpp>
#include <DawnStar/Core/Core.hpp>
#include <DawnStar/Core/LayerStack.hpp>
#include <DawnStar/Events/Event.hpp>
#include <DawnStar/Events/ApplicationEvent.hpp>

#include <DawnStar/Core/Timestep.hpp>

#include <DawnStar/ImGui/ImGuiLayer.hpp>

namespace DawnStar
{
    class Application 
    {
    public:
        Application(const std::string& name="DawnStar App",
					uint32_t width=1600,
					uint32_t height=900);
        virtual ~Application();

        void Run();

        void OnEvent(Event& e);

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* layer);

        inline Window& GetWindow() {return *_window;}
    
        void Close();

        ImGuiLayer* GetImGuiLayer() {return _imGuiLayer; }

        static Application& Get() {return *s_Instance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
    private:
        Scope<Window> _window;
        ImGuiLayer* _imGuiLayer;
        bool _running = true;
        bool _minimized = false;
        LayerStack _layerStack ;
        float _lastFrameTime = 0.0f;

    private:
        static Application* s_Instance;
    };

    Application* CreateApplication();
}// namespace DawnStar