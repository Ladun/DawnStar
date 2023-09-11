#pragma once

#include <DawnStar/Core/Core.hpp>
#include <DawnStar/Core/Timestep.hpp>
#include <DawnStar/Events/Event.hpp>

namespace DawnStar
{
	class Layer
	{
	public :
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName;
	};
} // namespace DawnStar
