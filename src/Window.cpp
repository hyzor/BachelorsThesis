#include <cstring>

#include "common/platform.h"

#include "Window.h"

// Must be included last!
#include "common/debug.h"

static const wchar_t *windowClassName = L"Thesis";

static LRESULT CALLBACK GlobalWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Window *window = (Window *)GetWindowLongPtr(hWnd, 0);

	if (window)
	{
		return window->WindowProc(hWnd, uMsg, wParam, lParam);
	}
	else
	{
		switch (uMsg)
		{
		case WM_CREATE:
			SetWindowLongPtr(hWnd, 0, (LONG_PTR)((CREATESTRUCT*)lParam)->lpCreateParams);
			break;
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

Window::Window(const unsigned int width, const unsigned int height, const wchar_t *title)
{
	m_listener = NULL;

	m_instanceHandle = GetModuleHandle(NULL);

	WNDCLASSEX windowClass;
	memset(&windowClass, 0, sizeof(windowClass));
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = GlobalWindowProc;
	windowClass.cbWndExtra = sizeof(void *); // Extra space for the Window pointer.
	windowClass.hInstance = m_instanceHandle;
	//windowClass.hIcon = LoadIcon(m_instanceHandle, MAKEINTRESOURCE(IDI_APP));
	windowClass.hIcon = NULL;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = NULL;
	windowClass.lpszClassName = windowClassName;

	RegisterClassEx(&windowClass);

	DWORD windowStyle = WS_OVERLAPPEDWINDOW;

	// The size passed to CreateWindow includes the size of the borders, so take the borders
	// in account so that windowWidth and windowHeight will create a window with a drawable
	// surface of the specified size.
	RECT rect;
	SetRect(&rect, 0, 0, width, height);
	AdjustWindowRect(&rect, windowStyle, false);

	unsigned int windowWidth = rect.right - rect.left;
	unsigned int windowHeight = rect.bottom - rect.top;

	// Clamp windowWidth and windowHeight to the desktop dimensions.
	unsigned int screenWidth = (unsigned int)GetSystemMetrics(SM_CXSCREEN);
	unsigned int screenHeight = (unsigned int)GetSystemMetrics(SM_CYSCREEN);

	if (windowWidth > screenWidth)
		windowWidth = screenWidth;
	if (windowHeight > screenHeight)
		windowHeight = screenHeight;

	m_window = CreateWindow(windowClassName,     // window class
		title,               // title-bar string
		windowStyle,         // style
		CW_USEDEFAULT,       // horizontal position
		CW_USEDEFAULT,       // vertical position
		windowWidth,		 // width
		windowHeight,		 // height
		NULL,				 // parent window
		NULL,				 // menu
		m_instanceHandle,    // handle to application instance
		this);		         // window-creation data (this will be passed to the WM_CREATE message)

	ShowWindow(m_window, SW_SHOW);

	m_inputHandler = new InputHandler(this);
	m_liveResize = false;
	m_isResizing = false;
	m_didResize = false;

	SetForegroundWindow(m_window);
	SetFocus(m_window);
	m_active = true;
}

Window::~Window()
{
	delete m_inputHandler;
	DestroyWindow(m_window);
	UnregisterClass(windowClassName, m_instanceHandle);
}

void Window::PumpMessages()
{
	MSG msg;

	while (PeekMessage(&msg, m_window, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	m_inputHandler->Update();
}

void Window::SetListener(WindowListener *listener)
{
	m_listener = listener;
}

void Window::SetLiveResize(bool liveResize)
{
	m_liveResize = liveResize;
}

void Window::SetCursorVisibility(bool visible)
{
	if (visible) {
		while (ShowCursor(true) < 0) {
		}
	}
	else {
		while (ShowCursor(false) >= 0) {
		}
	}
}

bool Window::IsActive() const
{
	return m_active;
}

HWND Window::GetHandle() const
{
	return m_window;
}

InputHandler *Window::GetInputHandler() const
{
	return m_inputHandler;
}

WindowListener *Window::GetListener() const
{
	return m_listener;
}

unsigned int Window::GetWidth() const
{
	RECT rect;
	GetClientRect(m_window, &rect);

	return rect.right;
}

unsigned int Window::GetHeight() const
{
	RECT rect;
	GetClientRect(m_window, &rect);

	return rect.bottom;
}

LRESULT Window::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// FIXME: Handle WM_CHAR for text input.
	switch (uMsg)
	{
	case WM_ENTERSIZEMOVE:
		if (m_listener)
			m_listener->OnWindowResizeStart();

		m_isResizing = true;

		break;
	case WM_EXITSIZEMOVE:
		if (m_listener && !m_liveResize && m_didResize)
			m_listener->OnWindowResized(GetWidth(), GetHeight());

		m_isResizing = false;
		m_didResize = false;

		if (m_listener)
			m_listener->OnWindowResizeEnd();

		break;
	case WM_SIZE:
		if (m_isResizing)
			m_didResize = true;

		if (m_listener && (m_liveResize || (!m_liveResize && !m_isResizing)))
			m_listener->OnWindowResized((unsigned int)LOWORD(lParam), (unsigned int)HIWORD(lParam));

		break;
	case WM_CLOSE:
		if (m_listener)
			m_listener->OnWindowShouldClose();

		return 0;
	case WM_NCACTIVATE:
		if (wParam == TRUE) {
			m_active = true;

			if (m_listener)
				m_listener->OnWindowActivate();
		}
		else {
			m_active = false;

			if (m_listener)
				m_listener->OnWindowDeactivate();
		}

		break;
	case WM_INPUT:
		RAWINPUT input;
		UINT dwSize = sizeof(input);

		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &input, &dwSize, sizeof(RAWINPUTHEADER));

		if (input.header.dwType == RIM_TYPEMOUSE)
		{
			RAWMOUSE *mouse = &input.data.mouse;

			if ((mouse->usFlags & MOUSE_MOVE_RELATIVE) == MOUSE_MOVE_RELATIVE)
				m_inputHandler->OnMouseMove((int)mouse->lLastX, (int)mouse->lLastY);

			if ((mouse->usButtonFlags & RI_MOUSE_WHEEL) == RI_MOUSE_WHEEL)
				m_inputHandler->OnMouseWheel((int)((short)mouse->usButtonData / WHEEL_DELTA));

			if ((mouse->usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) == RI_MOUSE_LEFT_BUTTON_DOWN)
				m_inputHandler->OnMouseButtonDown(MouseButtonLeft);
			if ((mouse->usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) == RI_MOUSE_LEFT_BUTTON_UP)
				m_inputHandler->OnMouseButtonUp(MouseButtonLeft);

			if ((mouse->usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) == RI_MOUSE_RIGHT_BUTTON_DOWN)
				m_inputHandler->OnMouseButtonDown(MouseButtonRight);
			if ((mouse->usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) == RI_MOUSE_RIGHT_BUTTON_UP)
				m_inputHandler->OnMouseButtonUp(MouseButtonRight);

			if ((mouse->usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) == RI_MOUSE_MIDDLE_BUTTON_DOWN)
				m_inputHandler->OnMouseButtonDown(MouseButtonMiddle);
			if ((mouse->usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP) == RI_MOUSE_MIDDLE_BUTTON_UP)
				m_inputHandler->OnMouseButtonUp(MouseButtonMiddle);
		}
		else if (input.header.dwType == RIM_TYPEKEYBOARD)
		{
			RAWKEYBOARD *keyboard = &input.data.keyboard;

			if ((keyboard->Flags & RI_KEY_MAKE) == RI_KEY_MAKE)
				m_inputHandler->OnKeyDown(keyboard->VKey);
			if ((keyboard->Flags & RI_KEY_BREAK) == RI_KEY_BREAK)
				m_inputHandler->OnKeyUp(keyboard->VKey);
		}

		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}