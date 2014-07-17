#ifndef CAMERACONTROLLER_H_
#define CAMERACONTROLLER_H_

#include "Common/platform.h"

class DLL_API CameraController
{
protected:
	struct float3
	{
		float x;
		float y;
		float z;
	};

public:
	virtual ~CameraController() {}
	virtual void SetPosition(float x, float y, float z) = 0;
	virtual void SetDirection(float x, float y, float z) = 0;

	virtual void OnMouseMove(float deltaX, float deltaY) = 0;

	virtual void SetMoveForwards(bool moveForwards) = 0;
	virtual void SetMoveBackwards(bool moveBackwards) = 0;
	virtual void SetStrafeLeft(bool strafeLeft) = 0;
	virtual void SetStrafeRight(bool strafeRight) = 0;

	virtual void SetCameraMovementSpeed(float speed) = 0;

	virtual void Update(float dt) = 0;

	virtual float3 GetPosition() = 0;
	virtual float3 GetLook() = 0;
};

#endif