#include <DawnStar/dspch.hpp>

#include "ExampleLayer.hpp"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <DawnStar/Game/UI/UISystem.hpp>
#include <DawnStar/Game/UI/UIContents.hpp>

namespace UIExample
{

	ExampleLayer::ExampleLayer()
		: _statPanel(), _listPanel()
	{

	}

	void ExampleLayer::OnAttach()
	{
		_scene = CreateRef<Scene>();
		_listPanel.SetContext(_scene);
		_scene->OnViewportResize(Application::Get().GetWindow().GetWidth(),
								  Application::Get().GetWindow().GetHeight());

		_scene->AddSystem(CreateRef<UI::UISystem>(_scene));
		{ // Camera
			Entity mainCam = _scene->CreateEntity("Main Camera");
			mainCam.AddComponent<CameraComponent>();  

			auto& transform = mainCam.GetTransform();
			transform.Translation.z = 10.0f;        
		}
		// { // None UI object
		// 	_testEntity = _scene->CreateEntity("Test object");
		// 	// Texture setting
		// 	auto& sprite = _testEntity.AddComponent<SpriteRendererComponent>();
		// 	sprite.Color = {0.7f, 0.5f, 0.3f, 1.0f};
		// }
		// { // Button
		// 	auto entity = _scene->CreateEntity("Connect Button");
		// 	auto& layout = entity.AddComponent<UI::LayoutComponent>();
		// 	layout.AnchorMax = layout.AnchorMin = {0.5, 0.5};
		// 	layout.Pivot = {0.5, 0.5};
		// 	layout.Box = {0, 0, 80, 40};
			
		// 	auto& sprite = entity.AddComponent<UI::SpriteRendererComponent>();
		// 	sprite.SortingOrder = 8;

		// 	auto& button = entity.AddComponent<UI::ButtonComponent>();
		// 	button.normalColor  = {0.3f, 0.8f, 0.3f, 0.5f};
		// 	button.overColor    = {0.3f, 0.8f, 0.3f, 0.7f};
		// 	button.pressColor   = {0.3f, 0.8f, 0.3f, 0.3f};
		// }
		{ // Input Box
			auto entity = DawnStar::UI::CreateInputText(_scene);
		}


	}

	void ExampleLayer::OnDetach()
	{
		
	}

	void ExampleLayer::OnUpdate(Timestep ts)
	{
		
		// Update and Rendering scene
		Renderer2D::ResetStats();
		RenderCommand::SetClearColor({0.0f, 0.0f, 0.0f, 1.0f});
		RenderCommand::Clear();

		_scene->OnUpdate(ts);
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