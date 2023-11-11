#include <DawnStar/dspch.hpp>

#if defined(DS_PLATFORM_WINDOWS) || defined(DS_PLATFORM_LINUX)

#include <DawnStar/Core/Input.hpp>
#include <DawnStar/Core/Application.hpp>

#include <GLFW/glfw3.h>
#include "Input.hpp"

namespace DawnStar
{
    InputState Input::m_KeyState;
    InputState Input::m_MouseState;
    
    bool Input::IsKey(const KeyCode key)
    {
        DS_PROFILE_CATEGORY("Input", Profile::Category::Input);

        return m_KeyState.GetCurrentState(key);
    }
    bool Input::IsKeyDown(const KeyCode key)
    {
        DS_PROFILE_CATEGORY("Input", Profile::Category::Input);
        return !m_KeyState.GetLastState(key) && m_KeyState.GetCurrentState(key);
    }
    bool Input::IsKeyUp(const KeyCode key)
    {
        DS_PROFILE_CATEGORY("Input", Profile::Category::Input);
        return m_KeyState.GetLastState(key) && !m_KeyState.GetCurrentState(key);
    }
        
    bool Input::IsMouseButton(MouseCode button)
    {
        DS_PROFILE_CATEGORY("Input", Profile::Category::Input);
        return m_KeyState.GetCurrentState(button);
    }
    bool Input::IsMouseButtonDown(MouseCode button)
    {
        DS_PROFILE_CATEGORY("Input", Profile::Category::Input);
        return !m_MouseState.GetLastState(button) && m_MouseState.GetCurrentState(button);
    }

    bool Input::IsMouseButtonUp(MouseCode button)
    {
        DS_PROFILE_CATEGORY("Input", Profile::Category::Input);
        return m_MouseState.GetLastState(button) && !m_MouseState.GetCurrentState(button);
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
        for(auto p : m_KeyState.m_CurrentState)
        {
            m_KeyState.m_LastState[p.first] = p.second;
        }
        for(auto p : m_MouseState.m_CurrentState)
        {
            m_MouseState.m_LastState[p.first] = p.second;
        }
    }

    void Input::SetUpKey(KeyCode key, bool pressed)
    {
        m_KeyState.SetCurrentState(key, pressed);
        m_KeyState.SetCurrentState(Key::Any, pressed);
    }

    void Input::SetUpMouse(MouseCode key, bool pressed)
    {
        m_MouseState.SetCurrentState(key, pressed);
        m_MouseState.SetCurrentState(Mouse::Any, pressed);
    }
} // namespace DawnStar


#endif
