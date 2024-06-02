#pragma once

#include <DawnStar/Core/Window.hpp>
#include <DawnStar/Core/Core.hpp>
#include <DawnStar/Core/LayerStack.hpp>
#include <DawnStar/Events/Event.hpp>
#include <DawnStar/Events/ApplicationEvent.hpp>

#include <DawnStar/Core/Timestep.hpp>

#include <DawnStar/ImGui/ImGuiLayer.hpp>


int main(int argc, char** argv);

namespace DawnStar
{
    class Application 
    {
    public:
        Application(const std::string& name="DawnStar App",
					uint32_t width=1600,
					uint32_t height=900);
        virtual ~Application();

        void OnEvent(Event& e);

		void PushLayer(Layer* layer) const { _layerStack->PushLayer(layer); }
		void PushOverlay(Layer* overlay) const { _layerStack->PushOverlay(overlay); }
		void PopLayer(Layer* layer) const { _layerStack->PopLayer(layer); }
		void PopOverlay(Layer* overlay) const { _layerStack->PopOverlay(overlay); }

        inline Window& GetWindow() {return *_window;}
    
        void Close();

        ImGuiLayer* GetImGuiLayer() {return _imGuiLayer; }

        static Application& Get() {return *s_Instance; }
        
		void SubmitToMainThread(const std::function<void()>& function);
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

        void Run();
		void ExecuteMainThreadQueue();

    private:
        Scope<Window>   _window;
        ImGuiLayer*     _imGuiLayer;
        bool            _running       = true;
        bool            _minimized     = false;
        LayerStack*     _layerStack;
        float           _lastFrameTime = 0.0f;

        std::vector<std::function<void()>> _mainThreadQueue;
        
		std::mutex _mainThreadQueueMutex;


    private:
        static Application* s_Instance;
		friend int ::main(int argc, char** argv);
    };

    Application* CreateApplication();
}// namespace DawnStar