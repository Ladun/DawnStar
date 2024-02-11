#include <DawnStar/dspch.hpp>
#include "ExampleLayer.hpp"
#include "TestSystem.hpp"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>


namespace BasicExample
{
	ExampleLayer::ExampleLayer()
		: _statPanel(), _listPanel()
	{

	}

	void ExampleLayer::OnAttach()
	{
		_scene = DawnStar::CreateRef<DawnStar::Scene>();
		_scene->OnViewportResize(DawnStar::Application::Get().GetWindow().GetWidth(),
								DawnStar::Application::Get().GetWindow().GetHeight());
		_listPanel.SetContext(_scene);

		_scene->AddSystem(DawnStar::CreateRef<TestSystem>(_scene));
		{ // Camera
			DawnStar::Entity mainCam = _scene->CreateEntity("Main Camera");
			mainCam.AddComponent<DawnStar::CameraComponent>();        
		}
		{
			_testEntity = _scene->CreateEntity("Test object");
			// Texture setting
			auto& sprite = _testEntity.AddComponent<DawnStar::SpriteRendererComponent>();
			sprite.Color = {0.7f, 0.5f, 0.3f, 1.0f};

			auto& text = _testEntity.AddComponent<DawnStar::TextComponent>();
			text.TextString = "Hello";
			text.Color = glm::vec4(1.0f);
		}

	}

	void ExampleLayer::OnDetach()
	{
		
	}

	void ExampleLayer::OnUpdate(DawnStar::Timestep ts)
	{

		auto camEntity = _scene->GetPrimaryCameraEntity();
		if(camEntity)
		{
			auto& transform = camEntity.GetTransform();
			
			if (DawnStar::Input::IsKey(DawnStar::Key::W))
				transform.Translation.z -= ts * 2;
			if (DawnStar::Input::IsKey(DawnStar::Key::S))
				transform.Translation.z += ts * 2;
		}
		
		// Update and Rendering scene
		DawnStar::Renderer2D::ResetStats();
		DawnStar::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
		DawnStar::RenderCommand::Clear();

		_scene->OnUpdate(ts);
	}

	void ExampleLayer::OnImGuiRender()
	{
		_statPanel.OnImGuiRender();
		_listPanel.OnImGuiRender();
			

		ImGui::Begin("Camera Setting");
		{
			auto camEntity = _scene->GetPrimaryCameraEntity();
			if(camEntity)
			{
				auto& transform = camEntity.GetTransform();
				DawnStar::ImGuiUI::DrawVec3Control("Translation", transform.Translation);
				DawnStar::ImGuiUI::DrawVec3Control("Rotation", transform.Rotation);
				DawnStar::ImGuiUI::DrawVec3Control("Scale", transform.Scale);
			}
		}
		ImGui::End();
	}

	void ExampleLayer::OnEvent(DawnStar::Event& e)
	{
	}
}