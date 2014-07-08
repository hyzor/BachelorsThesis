#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "common/util.h"

#include "Window.h"
#include "InputHandler.h"
#include "GraphicsEngine/GraphicsEngine.h"

class Application : WindowListener, InputListener
{
public:

	Application();
	virtual ~Application();

	void Start();

private:

	void Update(float dt, float gameTime);

public: // WindowListener
	void OnWindowShouldClose();
	void OnWindowResized(unsigned int width, unsigned int height);
	void OnWindowResizeEnd();
	void OnWindowActivate();
	void OnWindowDeactivate();

public: // InputListener
	void OnMouseMove(int deltaX, int deltaY);
	void OnMouseButtonDown(enum MouseButton button);
	void OnMouseButtonUp(enum MouseButton button);
	void OnMouseWheel(int delta);

	void OnKeyDown(unsigned short key);
	void OnKeyUp(unsigned short key);

private:
	double m_oldTime;
	bool m_quit;

	double mGameTime;
	double mStartTime;

	Window* m_window;
	InputHandler* m_inputHandler;

	GraphicsEngine* m_graphicsEngine;

	std::string mResourceDir;
};

#endif
