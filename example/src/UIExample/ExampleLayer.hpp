#pragma once

#include <DawnStar/DawnStar.hpp>
#include <DawnStar/Debug/RenderStatPanel.hpp>
#include <DawnStar/Debug/ObjectListPanel.hpp>

using namespace DawnStar;

namespace UIExample
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

		Ref<Scene> _scene;

		Entity _testEntity;
	};
}