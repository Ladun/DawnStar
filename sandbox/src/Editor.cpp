#include <DawnStar/dspch.hpp>
#include "Editor.hpp"

#include <imgui.h>

TestLayer::TestLayer()
    : m_Fps(0)
{

}

void TestLayer::OnAttach()
{

}
void TestLayer::OnDetach()
{
    
}

void TestLayer::OnUpdate(DawnStar::Timestep ts)
{
    m_Fps = (int)(1 / ts.GetSeconds());
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