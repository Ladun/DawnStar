#pragma once

#include <DawnStar/DawnStar.hpp>
#include <DawnStar/Debug/RenderStatPanel.hpp>

namespace UIExample
{
	class ExampleLayer : public DawnStar::Layer
	{
	public:
		ExampleLayer();
		virtual ~ExampleLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(DawnStar::Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(DawnStar::Event& e) override;

	private:
		DawnStar::RenderStatPanel m_StatPanel;

		DawnStar::Ref<DawnStar::Scene> m_Scene;

		DawnStar::Entity m_TestEntity;
		DawnStar::Entity m_TestEntity2;
	};
}