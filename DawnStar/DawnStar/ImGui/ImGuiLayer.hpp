#pragma once

#include <DawnStar/Core/Layer.hpp>

#include <DawnStar/Events/KeyEvent.hpp>
#include <DawnStar/Events/MouseEvent.hpp>

namespace DawnStar
{
	class ImGuiLayer :public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();

		void SetBlockEvents(bool block) { m_BlockEvents = block; }
		void SetDarkThemeColors();

	private:
		bool m_BlockEvents = true;
		float m_Time = 0.0f;
	};
} // namespace DawnStar
