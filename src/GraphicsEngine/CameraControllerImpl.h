#ifndef CAMERACONTROLLERIMPL_H_
#define CAMERACONTROLLERIMPL_H_

#include "CameraController.h"
#include "Camera.h"
#include <DirectXMath.h>

class CameraControllerImpl : public CameraController
{
public:
	CameraControllerImpl(Camera* cam);
	virtual ~CameraControllerImpl();

	void SetPosition(float x, float y, float z);
	void SetDirection(float x, float y, float z);

	void SetMoveForwards(bool moveForwards);
	void SetMoveBackwards(bool moveBackwards);
	void SetStrafeLeft(bool strafeLeft);
	void SetStrafeRight(bool strafeRight);

	void Update(float dt);

	float3 GetPosition();

	void SetCameraMovementSpeed(float speed);

	virtual void OnMouseMove(float deltaX, float deltaY);

	virtual float3 GetLook();

private:
	Camera* mCamera;

	bool mMoveForwards;
	bool mMoveBackwards;
	bool mStrafeLeft;
	bool mStrafeRight;

	float mCameraMovementSpeed;

	float mYaw;
	float mPitch;
	float mTargetYaw;
	float mTargetPitch;

	float mCameraMouseSensitivity;
};

#endif