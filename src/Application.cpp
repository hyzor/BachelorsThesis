#include <string>
#include <iostream>
#include <cassert>

#include "Application.h"

#include "Common/debug.h"

Application::Application()
{
}

Application::~Application()
{
}

void Application::Start()
{
	m_window = new Window(1024, 768, L"Thesis");
	m_window->SetListener(this);

	mResourceDir = "../content";

	m_inputHandler = m_window->GetInputHandler();

	// Create graphics engine
	m_graphicsEngine = CreateGraphicsEngine();
	m_graphicsEngine->Init(m_window->GetHandle(), m_window->GetWidth(), m_window->GetHeight(), mResourceDir);

	// Don't start listening to input events until everything has been initialized.
	m_inputHandler->AddListener(this);

	mGameTime = 0.0;
	m_oldTime = Utils::GetTime();
	mStartTime = Utils::GetTime();
	m_quit = false;

	// Main loop
	while (!m_quit)
	{
		double time, deltaTime;

		time = Utils::GetTime();
		deltaTime = time - m_oldTime;
		m_oldTime = time;

		mGameTime = time - mStartTime;

		Update((float)deltaTime, (float)mGameTime);
		m_graphicsEngine->Present();
		m_window->PumpMessages();

		m_oldTime = Utils::GetTime();
	}
	
	m_inputHandler->RemoveListener(this);

	// Application clean-up
	DestroyGraphicsEngine(m_graphicsEngine);
	delete m_window;
}

void Application::Update(float dt, float gameTime)
{
	m_graphicsEngine->UpdateScene(dt, gameTime);
	m_graphicsEngine->DrawScene();
}

void Application::OnWindowShouldClose()
{
	m_quit = true;
}

void Application::OnWindowResized(unsigned int width, unsigned int height)
{
	m_graphicsEngine->OnResize(width, height);
	m_oldTime = Utils::GetTime();
}

void Application::OnWindowResizeEnd()
{
	// While resizing/moving the window PumpMessages will block and cause a
	// huge delta time when the window has stopped resizing/moving. Set
	// m_oldTime to the current time to prevent that from happening.
	m_oldTime = Utils::GetTime();
}

void Application::OnWindowActivate()
{
	m_inputHandler->SetMouseCapture(true);
	m_window->SetCursorVisibility(false);
	m_oldTime = Utils::GetTime();
}

void Application::OnWindowDeactivate()
{
	m_window->SetCursorVisibility(true);
}

void Application::OnMouseMove(int deltaX, int deltaY)
{	
}

void Application::OnMouseButtonDown(enum MouseButton button)
{
}

void Application::OnMouseButtonUp(enum MouseButton button)
{
}

void Application::OnMouseWheel(int delta)
{
}

void Application::OnKeyUp(unsigned short key)
{
}

void Application::OnKeyDown(unsigned short key)
{
	switch (key)
	{
	case VK_ESCAPE:
		m_quit = true;
		break;
	default:
		break;
	}
}