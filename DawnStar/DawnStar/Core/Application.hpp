#pragma once

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
        Application(const std::string& name="DawnStar App");
        virtual ~Application();

        void Run();

        void OnEvent(Event& e);

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* layer);

        inline Window& GetWindow() {return *m_Window;}

        void Close();

        ImGuiLayer* GetImGuiLayer() {return m_ImGuiLayer; }

        static Application& Get() {return *s_Instance; }
    private:
        Scope<Window> m_Window;
        ImGuiLayer* m_ImGuiLayer;
        bool m_Running = true;
        bool m_Minimized = false;
        LayerStack m_LayerStack ;
        float m_LastFrameTime = 0.0f;

    private:
        static Application* s_Instance;
    };

    Application* CreateApplication();
}// namespace DawnStar