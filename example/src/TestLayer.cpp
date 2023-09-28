#include <DawnStar/dspch.hpp>
#include "TestLayer.hpp"

#include <imgui.h>

TestLayer::TestLayer()
    : m_Fps(0)
{

}

void TestLayer::OnAttach()
{
	m_MainScene = DawnStar::CreateRef<DawnStar::Scene>();

	DawnStar::Entity entity = m_MainScene->CreateEntity("Test object");
	auto transform = entity.GetComponent<DawnStar::TransformComponent>();
	auto sprite = entity.AddComponent<DawnStar::SpriteRendererComponent>();
	// sprite.Texture

}

void TestLayer::OnDetach()
{
    
}

void TestLayer::OnUpdate(DawnStar::Timestep ts)
{
    m_Fps = (int)(1 / ts.GetSeconds());

	m_MainScene->OnUpdate(ts);
}

void TestLayer::OnImGuiRender()
{

	ImGui::Begin("Settings");
    ImGui::Text("Hello World");
    ImGui::Text("FPS: %d", m_Fps);

	// auto stats = DawnStar::Renderer2D::GetStats();
	// ImGui::Text("Renderer2D Stats:");
	// ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	// ImGui::Text("Quad: %d", stats.QuadCount);
	// ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	// ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

	// ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	ImGui::End();


}

void TestLayer::OnEvent(DawnStar::Event& e)
{
}