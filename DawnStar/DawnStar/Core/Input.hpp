#pragma once

#include <DawnStar/Core/Core.hpp>

#include <DawnStar/Core/KeyCodes.hpp>
#include <DawnStar/Core/MouseCodes.hpp>

#include <glm/vec2.hpp>

#include <unordered_map>


namespace DawnStar
{
	class InputState
	{
	public:

		bool GetCurrentState(uint16_t key)
		{        
			auto it = m_CurrentState.find(key);
        	if(it != m_CurrentState.end())
			{
				return it->second;
			}
			else
			{
				m_CurrentState[key] = false;
				return false;
			}
		}
		bool GetLastState(uint16_t key)
		{        
			auto it = m_LastState.find(key);
        	if(it != m_LastState.end())
			{
				return it->second;
			}
			else
			{
				m_LastState[key] = false;
				return false;
			}
		}
	public:
		// Pressed = true, Released = false
		std::unordered_map<KeyCode, bool> m_CurrentState;
		std::unordered_map<KeyCode, bool> m_LastState;
	};

	class Input
	{

	public:
		static bool IsKey(KeyCode keycode);
    	static bool IsKeyDown(const KeyCode key);
    	static bool IsKeyUp(const KeyCode key);

		static bool IsMouseButton(MouseCode button);
		static bool IsMouseButtonDown(MouseCode button);
		static bool IsMouseButtonUp(MouseCode button);

		static glm::vec2 GetMousePosition();
		static void SetMousePosition(const glm::vec2& position);

		static float GetMouseX() { return GetMousePosition().x; }
		static float GetMouseY() { return GetMousePosition().y; }

	public:
		static void OnUpdate();
		static void SetUpKey(KeyCode key, bool pressed);
		static void SetUpMouse(MouseCode key, bool pressed);

	private:
		static InputState m_KeyState;
		static InputState m_MouseState;

	};
}