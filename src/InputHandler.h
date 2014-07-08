#ifndef GRAPHICS_INPUTHANDLER_H
#define GRAPHICS_INPUTHANDLER_H

#include <climits>
#include <vector>

#include "common/platform.h"

class Window;

enum MouseButton
{
	MouseButtonLeft = 0,
	MouseButtonRight,
	MouseButtonMiddle,
	MouseButtonCount
};

class InputListener
{
public:
	virtual void OnMouseMove(int deltaX, int deltaY) {};
	virtual void OnMouseButtonDown(enum MouseButton button) {};
	virtual void OnMouseButtonUp(enum MouseButton button) {};
	virtual void OnMouseWheel(int delta) {};

	// key is one of the virtual-key codes listed at http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
	virtual void OnKeyDown(unsigned short key) {};
	virtual void OnKeyUp(unsigned short key) {};
};

class InputHandler
{
public:
	virtual ~InputHandler();

	void AddListener(InputListener *listener);
	void RemoveListener(InputListener *listener);

	void Update();

	void SetMouseCapture(bool capture);
	bool IsMouseCaptured();

	bool isMouseButtonDown(enum MouseButton button) const;
	bool isKeyDown(unsigned short key) const;
	// The returned values are relative to the upper-left corner of the window.
	void GetMousePosition(int &x, int &y);
	bool isMouseInWindow();

	// Only WindowProc should call these.
	void OnMouseMove(int deltaX, int deltaY);
	void OnMouseButtonDown(enum MouseButton button);
	void OnMouseButtonUp(enum MouseButton button);
	void OnMouseWheel(int delta);

	void OnKeyDown(unsigned short key);
	void OnKeyUp(unsigned short key);

private:
	friend class Window;

	InputHandler(Window *window);

private:
	Window *m_window;
	std::vector<InputListener *> m_listeners;
	bool m_inputDevicesRegistered;
	bool m_mouseCaptured;

	bool m_mouseButtonStates[MouseButtonCount];
	bool m_keyStates[USHRT_MAX];
};

#endif
