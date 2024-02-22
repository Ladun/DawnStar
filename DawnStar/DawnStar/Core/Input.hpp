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
			auto it = _currentState.find(key);
        	if(it != _currentState.end())
			{
				return it->second;
			}
			else
			{
				_currentState[key] = false;
				return false;
			}
		}
		bool GetLastState(uint16_t key)
		{        
			auto it = _lastState.find(key);
        	if(it != _lastState.end())
			{
				return it->second;
			}
			else
			{
				_lastState[key] = false;
				return false;
			}
		}

		void SetCurrentState(uint16_t key, bool pressed)
		{
			_currentState[key] = pressed;

			auto it = _lastState.find(key);
			if(it ==  _lastState.end())
			{
				_lastState[key] = !pressed;
			}
		}
	public:
		// Pressed = true, Released = false
		std::unordered_map<uint16_t, bool> _currentState;
		std::unordered_map<uint16_t, bool> _lastState;
	};

	class Input
	{

	public:
		static bool 		IsKey(KeyCode keycode);
    	static bool 		IsKeyDown(const KeyCode key);
    	static bool 		IsKeyUp(const KeyCode key);

		static bool 		IsMouseButton(MouseCode button);
		static bool 		IsMouseButtonDown(MouseCode button);
		static bool 		IsMouseButtonUp(MouseCode button);

		static glm::vec2 	GetMousePosition();
		static void 		SetMousePosition(const glm::vec2& position);

		static float 		GetMouseX() 						{ return GetMousePosition().x; }
		static float 		GetMouseY() 						{ return GetMousePosition().y; }

		static unsigned int GetKeyCode()						{ return _currentKeyCode; }
		static void 		SetKeyCode(unsigned int keyCode) 	{ _currentKeyCode = keyCode; }
	public:
		static void 		OnUpdate();
		static void 		SetUpKey(KeyCode key, bool pressed);
		static void 		SetUpMouse(MouseCode key, bool pressed);

	private:
		static InputState _keyState;
		static InputState _mouseState;
		static unsigned int _currentKeyCode;

	};
}