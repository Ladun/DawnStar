#include <dspch.hpp>
#include "ExampleLayer.hpp"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <DawnStar/Game/UI/UISystem.hpp>

namespace Example3D
{
	ExampleLayer::ExampleLayer()
		: _statPanel(), _listPanel()
	{

	}

	void ExampleLayer::OnAttach()
	{
		auto width  = Application::Get().GetWindow().GetWidth();
		auto height = Application::Get().GetWindow().GetHeight();

		_renderGraphData = CreateRef<RenderGraphData>(width, height);

		_scene = CreateRef<Scene>();
		_scene->OnViewportResize(width, height);
		_listPanel.SetContext(_scene);

		{ // Camera
			Entity mainCam = _scene->CreateEntity("Main Camera");
			mainCam.AddComponent<CameraComponent>();      

			auto& transform = mainCam.GetTransform();
			transform.Translation.z = 10.0f;  
		}
		{
			_testEntity = _scene->CreateEntity("Test object");
            auto& mesh = _testEntity.AddComponent<MeshComponent>();
            mesh.MeshGeometry = CreateRef<Mesh>("assets/resources/models/sponza/sponza.obj");
		}

		{
			Entity ent = _scene->CreateEntity("Dir Light");
			auto& light = ent.AddComponent<LightComponent>();
			light.Type = LightComponent::LightType::Directional;

		}
	}

	void ExampleLayer::OnDetach()
	{
		
	}

	void ExampleLayer::OnUpdate(Timestep ts)
	{

		auto camEntity = _scene->GetPrimaryCameraEntity();
		if(camEntity)
		{
			auto& transform = camEntity.GetTransform();
			
			if (Input::IsKey(Key::W))
				transform.Translation.z -= ts * 2;
			if (Input::IsKey(Key::S))
				transform.Translation.z += ts * 2;
		}
		
		// Update and Rendering scene
		_renderGraphData->RenderPassTarget->Bind();
		Renderer2D::ResetStats();
		Renderer3D::ResetStats();

		_scene->OnUpdate(ts, _renderGraphData);
		_renderGraphData->RenderPassTarget->Unbind();
	}

	void ExampleLayer::OnImGuiRender()
	{
		_statPanel.OnImGuiRender();
		_listPanel.OnImGuiRender();
	}

    bool ExampleLayer::OnWindowResize(WindowResizeEvent &e)
    {

        _scene->OnViewportResize(e.GetWidth(), e.GetHeight());
        Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
        
        return false;
    }

	void ExampleLayer::OnEvent(Event& e)
	{
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowResizeEvent>(DS_BIND_EVENT_FN(ExampleLayer::OnWindowResize));
	}
}