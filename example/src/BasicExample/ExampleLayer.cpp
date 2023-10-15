#include <DawnStar/dspch.hpp>
#include "ExampleLayer.hpp"
#include "TestSystem.hpp"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>


namespace BasicExample
{
	ExampleLayer::ExampleLayer()
		: m_StatPanel()
	{

	}

	void ExampleLayer::OnAttach()
	{
		m_Scene = DawnStar::CreateRef<DawnStar::Scene>();
		m_Scene->OnViewportResize(DawnStar::Application::Get().GetWindow().GetWidth(),
								DawnStar::Application::Get().GetWindow().GetHeight());

		m_Scene->AddSystem(DawnStar::CreateRef<TestSystem>());
		{ // Camera
			DawnStar::Entity mainCam = m_Scene->CreateEntity("Main Camera");
			mainCam.AddComponent<DawnStar::CameraComponent>();        
		}
		{
			m_TestEntity = m_Scene->CreateEntity("Test object");
			// Texture setting
			auto& sprite = m_TestEntity.AddComponent<DawnStar::SpriteRendererComponent>();
			sprite.Color = {0.7f, 0.5f, 0.3f, 1.0f};
		}

	}

	void ExampleLayer::OnDetach()
	{
		
	}

	void ExampleLayer::OnUpdate(DawnStar::Timestep ts)
	{

		auto camEntity = m_Scene->GetPrimaryCameraEntity();
		if(camEntity)
		{
			auto& transform = camEntity.GetTransform();
			
			if (DawnStar::Input::IsKeyPressed(DawnStar::Key::W))
				transform.Translation.z -= ts * 2;
			if (DawnStar::Input::IsKeyPressed(DawnStar::Key::S))
				transform.Translation.z += ts * 2;
		}
		
		// Update and Rendering scene
		DawnStar::Renderer2D::ResetStats();
		DawnStar::RenderCommand::SetClearColor({0.0f, 0.0f, 0.0f, 1.0f});
		DawnStar::RenderCommand::Clear();

		m_Scene->OnUpdate(ts);
	}

	void ExampleLayer::OnImGuiRender()
	{
		m_StatPanel.OnImGuiRender();

		ImGui::Begin("Object Setting");
		{
			{
				auto& transform = m_TestEntity.GetTransform();
				DawnStar::ImGuiUI::DrawVec3Control("Translation", transform.Translation);
				DawnStar::ImGuiUI::DrawVec3Control("Rotation", transform.Rotation);
				DawnStar::ImGuiUI::DrawVec3Control("Scale", transform.Scale);
			}	
		}
		ImGui::End();

			

		ImGui::Begin("Camera Setting");
		{
			auto camEntity = m_Scene->GetPrimaryCameraEntity();
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