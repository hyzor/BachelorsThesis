#include <cassert>
#include <cstring>

#include "common/platform.h"
#include "common/util.h"

#include "InputHandler.h"
#include "Window.h"

// Must be included last!
#include "common/debug.h"

InputHandler::InputHandler(Window *window)
{
	m_window = window;
	m_inputDevicesRegistered = false;
	m_mouseCaptured = false;

	for (int i = 0; i < ARRAY_SIZE(m_mouseButtonStates); i++)
	{
		m_mouseButtonStates[i] = false;
	}

	for (int i = 0; i < ARRAY_SIZE(m_keyStates); i++)
	{
		m_keyStates[i] = false;
	}
}

InputHandler::~InputHandler()
{
}

void InputHandler::AddListener(InputListener *listener)
{
	if (!m_inputDevicesRegistered)
	{
		RAWINPUTDEVICE devices[2];
		devices[0].usUsagePage = 0x01; // Generic desktop page
		devices[0].usUsage = 0x06;     // Keyboard
		devices[0].hwndTarget = m_window->GetHandle();
		devices[0].dwFlags = 0;
		devices[1].usUsagePage = 0x01; // Generic desktop page
		devices[1].usUsage = 0x02;     // Mouse
		devices[1].hwndTarget = m_window->GetHandle();
		devices[1].dwFlags = 0;

		BOOL result = RegisterRawInputDevices(devices, 2, sizeof(RAWINPUTDEVICE));

		assert(result == TRUE);

		m_inputDevicesRegistered = true;
	}

	m_listeners.push_back(listener);
}

void InputHandler::RemoveListener(InputListener *listener)
{
	for (auto iter = m_listeners.begin(); iter != m_listeners.end(); iter++)
	{
		if ((*iter) == listener)
		{
			m_listeners.erase(iter);
			break;
		}
	}
}

void InputHandler::Update()
{
	if (m_mouseCaptured && m_window->IsActive()) 
	{
		POINT point;
		point.x = m_window->GetWidth() / 2;
		point.y = m_window->GetHeight() / 2;

		// Convert from window to screen coordinates.
		ClientToScreen(m_window->GetHandle(), &point);
		SetCursorPos(point.x, point.y);
	}
}

void InputHandler::SetMouseCapture(bool capture)
{
	m_mouseCaptured = capture;
}

bool InputHandler::IsMouseCaptured()
{
	return m_mouseCaptured;
}

bool InputHandler::isMouseButtonDown(enum MouseButton button) const
{
	return m_mouseButtonStates[button];
}

bool InputHandler::isKeyDown(unsigned short key) const
{
	return m_keyStates[key];
}

void InputHandler::GetMousePosition(int &x, int &y)
{
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(m_window->GetHandle(), &point);

	x = point.x;
	y = point.y;
}

bool InputHandler::isMouseInWindow()
{
	int x, y;
	GetMousePosition(x, y);

	if (x < 0 || y < 0)
		return false;
	if (x >= (int)m_window->GetWidth() || y >= (int)m_window->GetHeight())
		return false;

	return true;
}

void InputHandler::OnMouseMove(int deltaX, int deltaY)
{
	if (!m_mouseCaptured && !isMouseInWindow())
		return;

	for (auto iter = m_listeners.begin(); iter != m_listeners.end(); iter++)
	{
		(*iter)->OnMouseMove(deltaX, deltaY);
	}
}

void InputHandler::OnMouseButtonDown(enum MouseButton button)
{
	if (!m_mouseCaptured && !isMouseInWindow())
		return;

	m_mouseButtonStates[button] = true;

	for (auto iter = m_listeners.begin(); iter != m_listeners.end(); iter++)
	{
		(*iter)->OnMouseButtonDown(button);
	}
}

void InputHandler::OnMouseButtonUp(enum MouseButton button)
{
	if (!m_mouseCaptured && !isMouseInWindow())
		return;

	m_mouseButtonStates[button] = false;

	for (auto iter = m_listeners.begin(); iter != m_listeners.end(); iter++)
	{
		(*iter)->OnMouseButtonUp(button);
	}
}

void InputHandler::OnMouseWheel(int delta)
{
	if (!m_mouseCaptured && !isMouseInWindow())
		return;

	for (auto iter = m_listeners.begin(); iter != m_listeners.end(); iter++)
	{
		(*iter)->OnMouseWheel(delta);
	}
}

void InputHandler::OnKeyDown(unsigned short key)
{
	// Ignore key-repeat events.
	if (m_keyStates[key])
		return;

	m_keyStates[key] = true;

	for (auto iter = m_listeners.begin(); iter != m_listeners.end(); iter++)
	{
		(*iter)->OnKeyDown(key);
	}
}

void InputHandler::OnKeyUp(unsigned short key)
{
	m_keyStates[key] = false;

	for (auto iter = m_listeners.begin(); iter != m_listeners.end(); iter++)
	{
		(*iter)->OnKeyUp(key);
	}
}
