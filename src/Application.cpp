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

	mResourceDir = "..\\..\\content\\";

	m_inputHandler = m_window->GetInputHandler();

	// Create graphics engine
	m_graphicsEngine = CreateGraphicsEngine();
	m_graphicsEngine->Init(m_window->GetHandle(), m_window->GetWidth(), m_window->GetHeight(), mResourceDir);
	m_graphicsEngine->LoadFont("Fonts\\buxton.spritefont", "Buxton");
	m_graphicsEngine->SetFont("Buxton");
	m_graphicsEngine->SetSkyTexture("Textures\\SkyBox_Space.dds");

	mCameraController = m_graphicsEngine->CreateCameraController();
	mCameraController->SetPosition(0.0f, 100.0f, -300.0f);
	mCameraController->SetCameraMovementSpeed(100.0f);

	// Don't start listening to input events until everything has been initialized.
	m_inputHandler->AddListener(this);

	mGameTime = 0.0;
	m_oldTime = Utils::GetTime();
	mStartTime = Utils::GetTime();
	m_quit = false;

	// Capture the mouse when starting the application
	m_inputHandler->SetMouseCapture(true);
	m_window->SetCursorVisibility(false);

	double time, deltaTime;

	// Main loop
	while (!m_quit)
	{
		time = Utils::GetTime();
		deltaTime = time - m_oldTime;
		m_oldTime = time;

		mGameTime = time - mStartTime;

		Update((float)deltaTime, (float)mGameTime);
		m_graphicsEngine->Present();
		m_window->PumpMessages();
	}
	
	m_inputHandler->RemoveListener(this);

	// Application clean-up
	m_graphicsEngine->DeleteCameraController(mCameraController);
	DestroyGraphicsEngine(m_graphicsEngine);
	delete m_window;
}

void Application::Update(float dt, float gameTime)
{
	m_graphicsEngine->UpdateScene(dt, gameTime);
	m_graphicsEngine->DrawScene();
	mCameraController->Update(dt);

	/*
	m_graphicsEngine->PrintText("Frame rate: " + std::to_string(m_graphicsEngine->GetFramerate()), 0, 0, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

	std::string camPosStr = "Pos: " + std::to_string(mCameraController->GetPosition().x) + " "
		+ std::to_string(mCameraController->GetPosition().y) + " " + std::to_string(mCameraController->GetPosition().z);
	m_graphicsEngine->PrintText(camPosStr, 0, 20, 1.0f, 1.0f, 1.0f, 1.0, 1.0f);

	std::string camLookStr = "Dir: " + std::to_string(mCameraController->GetLook().x) + " "
		+ std::to_string(mCameraController->GetLook().y) + " " + std::to_string(mCameraController->GetLook().z);
	m_graphicsEngine->PrintText(camLookStr, 0, 40, 1.0f, 1.0f, 1.0f, 1.0, 1.0f);
	*/
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
	mCameraController->OnMouseMove((float)deltaX, (float)deltaY);
}

void Application::OnMouseButtonDown(enum MouseButton button)
{
	if (!m_inputHandler->IsMouseCaptured())
	{
		m_inputHandler->SetMouseCapture(true);
		m_window->SetCursorVisibility(false);
	}
}

void Application::OnMouseButtonUp(enum MouseButton button)
{
}

void Application::OnMouseWheel(int delta)
{
}

void Application::OnKeyUp(unsigned short key)
{
	switch (key)
	{
	case 'W':
		mCameraController->SetMoveForwards(false); // Send stop move forward signal to camera controller
		break;
	case 'S':
		mCameraController->SetMoveBackwards(false);
		break;
	case 'A':
		mCameraController->SetStrafeLeft(false);
		break;
	case 'D':
		mCameraController->SetStrafeRight(false);
		break;
	default:
		break;
	}
}

void Application::OnKeyDown(unsigned short key)
{
	switch (key)
	{
	case VK_ESCAPE:
		m_graphicsEngine->SetFullscreen(false);
		m_quit = true;
		break;
	case 'W':
		mCameraController->SetMoveForwards(true); // Send move forward signal to camera controller
		break;
	case 'S':
		mCameraController->SetMoveBackwards(true);
		break;
	case 'A':
		mCameraController->SetStrafeLeft(true);
		break;
	case 'D':
		mCameraController->SetStrafeRight(true);
		break;
	default:
		break;
	}
}