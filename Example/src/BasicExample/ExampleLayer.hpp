#pragma once

#include <DawnStar.hpp>
#include <DawnStar/Debug/RenderStatPanel.hpp>
#include <DawnStar/Debug/ObjectListPanel.hpp>

using namespace DawnStar;

namespace BasicExample
{

	class ExampleLayer : public Layer
	{
	public:
		ExampleLayer();
		virtual ~ExampleLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;

	private:
		RenderStatPanel _statPanel;
		ObjectListPanel _listPanel;
		
		Ref<RenderGraphData>_renderGraphData = nullptr;
		Ref<Scene> _scene;

		Entity _testEntity;
	};
}