#include <DawnStar/dspch.hpp>

#include "ExampleLayer.hpp"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

template<>
void DawnStar::Scene::OnComponentAdded<UIExample::CustomComponent>(DawnStar::Entity entity, UIExample::CustomComponent& component)
{

}

namespace UIExample
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

		//m_Scene->AddSystem();
		{ // Camera
			DawnStar::Entity mainCam = m_Scene->CreateEntity("Main Camera");
			mainCam.AddComponent<DawnStar::CameraComponent>();        
		}
		{
			m_TestEntity = m_Scene->CreateEntity("Test object");

			// Texture setting
			auto& sprite = m_TestEntity.AddComponent<DawnStar::UISpriteRendererComponent>();
			sprite.Color = {0.7f, 0.5f, 0.3f, 1.0f};
		}
		{
			m_TestEntity2 = m_Scene->CreateEntity("Test object2");

			// Texture setting
			auto& sprite = m_TestEntity2.AddComponent<DawnStar::SpriteRendererComponent>();
			sprite.Color = {0.7f, 1.0f, 0.3f, 1.0f};

			m_TestEntity2.AddComponent<CustomComponent>();
		}


	}

	void ExampleLayer::OnDetach()
	{
		
	}

	void ExampleLayer::OnUpdate(DawnStar::Timestep ts)
	{
		
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
				auto& sprite = m_TestEntity.GetComponent<DawnStar::UISpriteRendererComponent>();
				ImGui::ColorEdit4("Color", glm::value_ptr(sprite.Color));	
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