#include "GraphicsEngine.h"
#include "GraphicsEngineImpl.h"

// Must be included last!
#include "common/debug.h"

DLL_API GraphicsEngine* CreateGraphicsEngine()
{
	GraphicsEngine* engine = (GraphicsEngine*) new GraphicsEngineImpl();

	return engine;
}

DLL_API void DestroyGraphicsEngine(GraphicsEngine* engine)
{
	if (engine)
		delete engine;
}