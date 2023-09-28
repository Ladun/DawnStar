#pragma once

#include <DawnStar/DawnStar.hpp>

class TestLayer : public DawnStar::Layer
{
public:
	TestLayer();
	virtual ~TestLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(DawnStar::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(DawnStar::Event& e) override;

private:
	int m_Fps;

	DawnStar::Ref<DawnStar::Scene> m_MainScene;
};