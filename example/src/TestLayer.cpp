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

	m_MainScene->OnViewportResize(DawnStar::Application::Get().GetWindow().GetWidth(),
								  DawnStar::Application::Get().GetWindow().GetHeight());

	{
		DawnStar::Entity testObj = m_MainScene->CreateEntity("Test object");
		auto transform = testObj.GetComponent<DawnStar::TransformComponent>();
		auto sprite = testObj.AddComponent<DawnStar::SpriteRendererComponent>();
		// sprite.Texture
	}

	{
		DawnStar::Entity cameraObj = m_MainScene->CreateEntity("Camera");
		auto transform = cameraObj.GetComponent<DawnStar::TransformComponent>();
		transform.Translation = {0.0f, 0.0f, -5.0f};
		auto cam = cameraObj.AddComponent<DawnStar::CameraComponent>();
	}

}

void TestLayer::OnDetach()
{
    
}

void TestLayer::OnUpdate(DawnStar::Timestep ts)
{
    m_Fps = (int)(1 / ts.GetSeconds());

	DawnStar::Renderer2D::ResetStats();
	DawnStar::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 0.1f});
	DawnStar::RenderCommand::Clear();
	m_MainScene->OnUpdate(ts);
}

void TestLayer::OnImGuiRender()
{

	ImGui::Begin("Settings");
    ImGui::Text("Hello World");
    ImGui::Text("FPS: %d", m_Fps);
	
	{
		const auto stats = DawnStar::Renderer2D::GetStats();
		ImGui::Text("2D");

		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		ImGui::Text("Tris: %d", stats.GetTotalTriangleCount());

		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
	}
	ImGui::End();


}

void TestLayer::OnEvent(DawnStar::Event& e)
{
}