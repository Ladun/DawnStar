#include <dspch.hpp>
#include "ExampleLayer.hpp"
#include "TestSystem.hpp"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <DawnStar/Game/UI/UISystem.hpp>


namespace BasicExample
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

		_scene->AddSystem(CreateRef<TestSystem>(_scene));
		{ // Camera
			Entity mainCam = _scene->CreateEntity("Main Camera");
			mainCam.AddComponent<CameraComponent>();      

			auto& transform = mainCam.GetTransform();
			transform.Translation.z = 10.0f;  
		}
		{
			_testEntity = _scene->CreateEntity("Test object");
			// Texture setting
			auto& sprite = _testEntity.AddComponent<SpriteRendererComponent>();
			sprite.Color = {0.7f, 0.5f, 0.3f, 1.0f};

			// auto& text = _testEntity.AddComponent<TextComponent>();
			// text.TextString = "Hello";
			// text.FontAsset = CreateRef<Font>("assets/fonts/godoRounded-L.ttf");
			// text.Color = glm::vec4(1.0f);
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
		// _renderGraphData->RenderPassTarget->Bind();
		Renderer2D::ResetStats();
		RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
		RenderCommand::Clear();

		_scene->OnUpdate(ts, _renderGraphData);
		// _renderGraphData->RenderPassTarget->Unbind();
	}

	void ExampleLayer::OnImGuiRender()
	{
		_statPanel.OnImGuiRender();
		_listPanel.OnImGuiRender();
	}

	void ExampleLayer::OnEvent(Event& e)
	{
	}
}