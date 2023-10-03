#pragma once

#include <DawnStar/Core/Core.hpp>

#include <glm/vec2.hpp>

#include <DawnStar/Core/KeyCodes.hpp>
#include <DawnStar/Core/MouseCodes.hpp>

namespace DawnStar
{
	class Input
	{
	public:
		[[nodiscard]] static bool IsKeyPressed(KeyCode keycode);

		[[nodiscard]] static bool IsMouseButtonPressed(MouseCode button);
		[[nodiscard]] static glm::vec2 GetMousePosition();
		static void SetMousePosition(const glm::vec2& position);

		[[nodiscard]] static float GetMouseX() { return GetMousePosition().x; }
		[[nodiscard]] static float GetMouseY() { return GetMousePosition().y; }

	};
}