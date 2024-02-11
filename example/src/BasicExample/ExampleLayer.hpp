#pragma once

#include <DawnStar/DawnStar.hpp>
#include <DawnStar/Debug/RenderStatPanel.hpp>
#include <DawnStar/Debug/ObjectListPanel.hpp>


namespace BasicExample
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
		DawnStar::RenderStatPanel _statPanel;
		DawnStar::ObjectListPanel _listPanel;

		DawnStar::Ref<DawnStar::Scene> _scene;

		DawnStar::Entity _testEntity;
	};
}