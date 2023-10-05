#include <DawnStar/dspch.hpp>
#include "TestLayer.hpp"
#include "TestScript.hpp"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

TestLayer::TestLayer()
    : m_StatPanel()
{

}

void TestLayer::OnAttach()
{
	m_World = DawnStar::CreateRef<DawnStar::World>();
	{
		m_TestEntity = m_World->GetActiveScene()->CreateEntity("Test object");

		auto& script = m_TestEntity.AddComponent<DawnStar::ScriptComponent>();
		script.Bind<TestScript>();

		// Texture setting
		auto& sprite = m_TestEntity.AddComponent<DawnStar::SpriteRendererComponent>();
		sprite.Color = {0.7f, 0.5f, 0.3f, 1.0f};
	}

}

void TestLayer::OnDetach()
{
    
}

void TestLayer::OnUpdate(DawnStar::Timestep ts)
{

	auto camEntity = m_World->GetActiveScene()->GetPrimaryCameraEntity();
	if(camEntity)
	{
		auto& transform = camEntity.GetTransform();
		
		if (DawnStar::Input::IsKeyPressed(DawnStar::Key::W))
			transform.Translation.z -= ts * 1;
		if (DawnStar::Input::IsKeyPressed(DawnStar::Key::S))
			transform.Translation.z += ts * 1;
	}
	
	m_World->OnUpdate(ts);
}

void TestLayer::OnImGuiRender()
{
	m_StatPanel.OnImGuiRender();

	ImGui::Begin("Object Setting");
	{
		auto& transform = m_TestEntity.GetTransform();
		DawnStar::ImGuiUI::DrawVec3Control("Translation", transform.Translation);

		auto& sprite = m_TestEntity.GetComponent<DawnStar::SpriteRendererComponent>();
		ImGui::ColorEdit4("Color", glm::value_ptr(sprite.Color));
	}
	ImGui::End();
}

void TestLayer::OnEvent(DawnStar::Event& e)
{
}