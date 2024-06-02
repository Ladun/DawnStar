#include <dspch.hpp>

#if defined(DS_PLATFORM_WINDOWS) || defined(DS_PLATFORM_LINUX)

#include <DawnStar/Core/Input.hpp>
#include <DawnStar/Core/Application.hpp>

#include <GLFW/glfw3.h>
#include "Input.hpp"

namespace DawnStar
{
    InputState Input::_keyState;
    InputState Input::_mouseState;
    unsigned int Input::_currentKeyCode;
    
    bool Input::IsKey(const KeyCode key)
    {
        DS_PROFILE_CATEGORY("Input", Profile::Category::Input);

        return _keyState.GetCurrentState(key);
    }
    bool Input::IsKeyDown(const KeyCode key)
    {
        DS_PROFILE_CATEGORY("Input", Profile::Category::Input);
        return !_keyState.GetLastState(key) && _keyState.GetCurrentState(key);
    }
    bool Input::IsKeyUp(const KeyCode key)
    {
        DS_PROFILE_CATEGORY("Input", Profile::Category::Input);
        return _keyState.GetLastState(key) && !_keyState.GetCurrentState(key);
    }
        
    bool Input::IsMouseButton(MouseCode button)
    {
        DS_PROFILE_CATEGORY("Input", Profile::Category::Input);
        return _keyState.GetCurrentState(button);
    }
    bool Input::IsMouseButtonDown(MouseCode button)
    {
        DS_PROFILE_CATEGORY("Input", Profile::Category::Input);
        return !_mouseState.GetLastState(button) && _mouseState.GetCurrentState(button);
    }

    bool Input::IsMouseButtonUp(MouseCode button)
    {
        DS_PROFILE_CATEGORY("Input", Profile::Category::Input);
        return _mouseState.GetLastState(button) && !_mouseState.GetCurrentState(button);
    }

    glm::vec2 Input::GetMousePosition()
    {
        DS_PROFILE_CATEGORY("Input", Profile::Category::Input);

        auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        double xpos;
        double ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        return { xpos, ypos };
    }

    void Input::SetMousePosition(const glm::vec2& position)
    {
        auto *window = static_cast<GLFWwindow *>(Application::Get().GetWindow().GetNativeWindow());
        glfwSetCursorPos(window, static_cast<double>(position.x), static_cast<double>(position.y));
    }

    void Input::OnUpdate()
    {        
        for(auto p : _keyState._currentState)
        {
            _keyState._lastState[p.first] = p.second;
        }
        for(auto p : _mouseState._currentState)
        {
            _mouseState._lastState[p.first] = p.second;
        }
    }

    void Input::SetUpKey(KeyCode key, bool pressed)
    {
        _keyState.SetCurrentState(key, pressed);
        _keyState.SetCurrentState(Key::Any, pressed);
    }

    void Input::SetUpMouse(MouseCode key, bool pressed)
    {
        _mouseState.SetCurrentState(key, pressed);
        _mouseState.SetCurrentState(Mouse::Any, pressed);
    }
} // namespace DawnStar


#endif
