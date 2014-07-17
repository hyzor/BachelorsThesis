#include "CameraControllerImpl.h"

CameraControllerImpl::CameraControllerImpl(Camera* cam)
{
	mCamera = cam;

	mMoveForwards = false;
	mMoveBackwards = false;
	mStrafeLeft = false;
	mStrafeRight = false;

	mYaw = 0.0f;
	mPitch = 0.0f;
	mTargetPitch = 0.0f;
	mTargetYaw = 0.0f;

	mCameraMovementSpeed = 1.0f;
	mCameraMouseSensitivity = 1.0f;
}

CameraControllerImpl::~CameraControllerImpl(){}

void CameraControllerImpl::SetPosition(float x, float y, float z)
{
	mCamera->SetPosition(x, y, z);
}

void CameraControllerImpl::SetDirection(float x, float y, float z)
{
	mCamera->SetDirection(DirectX::XMFLOAT3(x, y, z));
}

CameraController::float3 CameraControllerImpl::GetPosition()
{
	float3 pos;
	pos.x = mCamera->GetPosition().x;
	pos.y = mCamera->GetPosition().y;
	pos.z = mCamera->GetPosition().z;

	return pos;
}

void CameraControllerImpl::SetMoveForwards(bool moveForwards)
{
	mMoveForwards = moveForwards;
}

void CameraControllerImpl::SetMoveBackwards(bool moveBackwards)
{
	mMoveBackwards = moveBackwards;
}

void CameraControllerImpl::SetStrafeLeft(bool strafeLeft)
{
	mStrafeLeft = strafeLeft;
}

void CameraControllerImpl::SetStrafeRight(bool strafeRight)
{
	mStrafeRight = strafeRight;
}

void CameraControllerImpl::Update(float dt)
{
	XMVECTOR movement = XMLoadFloat3(&XMFLOAT3(0, 0, 0));

	if (mMoveForwards)
	{
		movement = movement + mCamera->GetLookXM() * mCameraMovementSpeed * dt;
	}
	else if (mMoveBackwards)
	{
		movement = movement - mCamera->GetLookXM() * mCameraMovementSpeed * dt;
	}

	if (mStrafeLeft)
	{
		movement = movement - mCamera->GetRightXM() * mCameraMovementSpeed * dt;
	}
	else if (mStrafeRight)
	{
		movement = movement + mCamera->GetRightXM() * mCameraMovementSpeed * dt;
	}

	mCamera->SetPosition(mCamera->GetPositionXM() + movement);
}

void CameraControllerImpl::SetCameraMovementSpeed(float speed)
{
	mCameraMovementSpeed = speed;
}

void CameraControllerImpl::OnMouseMove(float deltaX, float deltaY)
{
	mYaw += (deltaX * 0.001f) * mCameraMouseSensitivity;
	mPitch += (deltaY * 0.001f) * mCameraMouseSensitivity;

	mCamera->SetYaw(mYaw);
	mCamera->SetPitch(mPitch);

	//mCamera->UpdateViewMatrix();
}

CameraControllerImpl::float3 CameraControllerImpl::GetLook()
{
	float3 look;
	look.x = mCamera->GetLook().x;
	look.y = mCamera->GetLook().y;
	look.z = mCamera->GetLook().z;

	return look;
}
