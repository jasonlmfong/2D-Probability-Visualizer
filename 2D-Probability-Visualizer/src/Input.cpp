#include "Input.h"

GLFWwindow* Input::m_WindowID;
float Input::m_ScrollX = 0.0f;
float Input::m_ScrollY = 0.0f;

void Input::Init(GLFWwindow* window)
{
    m_WindowID = window;
    SetCallbacks();
}

void Input::SetCallbacks()
{
    glfwSetScrollCallback(m_WindowID, Input::ScrollCallback);
}

bool Input::IsKeyDown(int keycode)
{
    return glfwGetKey(m_WindowID, keycode) == GLFW_PRESS;
}

bool Input::IsMouseButtonDown(int button)
{
    return glfwGetMouseButton(m_WindowID, button) == GLFW_PRESS;
}

float Input::GetScrollX()
{
    return m_ScrollX;
}

float Input::GetScrollY()
{
    return m_ScrollY;
}

void Input::ResetScroll()
{
    m_ScrollX = 0.0f;
    m_ScrollY = 0.0f;
}

void Input::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	m_ScrollX = (float)xoffset;
	m_ScrollY = (float)yoffset;
}