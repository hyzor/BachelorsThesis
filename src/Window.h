#ifndef GRAPHICS_WINDOW_H
#define GRAPHICS_WINDOW_H

#include "common/platform.h"

#include "InputHandler.h"

class WindowListener
{
public:
	virtual void OnWindowShouldClose() = 0;
	virtual void OnWindowResizeStart() {};
	virtual void OnWindowResizeEnd() {};
	virtual void OnWindowResized(unsigned int width, unsigned int height) {};
	virtual void OnWindowActivate() {};
	virtual void OnWindowDeactivate() {};
};

class Window
{
public:
	Window(const unsigned int width, const unsigned int height, const wchar_t *title);
	virtual ~Window();

	void PumpMessages();

	void SetListener(WindowListener *listener);
	void SetLiveResize(bool liveResize);
	void SetCursorVisibility(bool visible);

	bool IsActive() const;
	HWND GetHandle() const;
	InputHandler *GetInputHandler() const;
	WindowListener *GetListener() const;
	unsigned int GetWidth() const;
	unsigned int GetHeight() const;

	// Don't call this.
	LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	HINSTANCE m_instanceHandle;
	HWND m_window;

	WindowListener *m_listener;
	InputHandler *m_inputHandler;

	bool m_liveResize;
	bool m_isResizing;
	bool m_didResize;
	bool m_active;
};

#endif