#ifndef CAMERA_H
#define CAMERA_H

#include <DirectXCollision.h>
#include <DirectXMath.h>
#include "common/util.h"

const float fovY = 0.785398f; // 0.25f * MathHelper::pi
const float zNear = 1.0f;
const float zFar = 1000.0f;

using namespace DirectX;

class Camera
{
public:
	Camera(void);
	~Camera(void);

	// Camera position functions
	XMVECTOR GetPositionXM() const;
	XMFLOAT3 GetPosition() const;

	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3& v);
	void SetPosition(const XMVECTOR& v);
	void SetDirection(XMFLOAT3 direction);
	void Rotate(float yaw, float pitch);

	void SetPrevViewProj(XMMATRIX& prevViewProj);

	// Get functions
	XMVECTOR GetRightXM() const;
	XMFLOAT3 GetRight() const;
	XMVECTOR GetUpXM() const;
	XMFLOAT3 GetUp() const;
	XMVECTOR GetLookXM() const;
	XMFLOAT3 GetLook() const;

	// Get frustum (lens) properties
	float GetNearZ() const;
	float GetFarZ() const;
	float GetAspect() const;
	float GetFovY() const;
	float GetFovX() const;

	// Set frustum (lens)
	void SetLens(float fovY, float aspect, float zn, float zf);
	void UpdateOrthoMatrix(float screenWidth, float screenHeight, float zn, float zf);
	void Update();

	// Get matrices
	XMMATRIX GetViewMatrix() const;
	XMMATRIX GetProjMatrix() const;
	XMMATRIX GetViewProjMatrix() const;
	XMMATRIX GetOrthoMatrix() const;
	XMMATRIX GetBaseViewMatrix() const;
	XMMATRIX GetWorldMatrix() const;

	XMMATRIX GetPreviousViewProj() const;

	// Get near and far plane dimensions in view space coordinates
	float GetNearWindowWidth() const;
	float GetNearWindowHeight() const;
	float GetFarWindowWidth() const;
	float GetFarWindowHeight() const;

	void UpdateViewMatrix();
	void UpdateBaseViewMatrix();

	void SetYaw(float yaw);
	void SetPitch(float pitch);
	void SetRoll(float roll);

	float GetYaw();
	float GetPitch();
	float GetRoll();

	DirectX::BoundingFrustum GetFrustum() const;

	void ComputeFrustum();

	void LookAt(XMFLOAT3 at);

private:
	// Coordinate system relative to world space
	XMFLOAT3 mPosition; // view space origin
	XMFLOAT3 mRight; // view space x-axis
	XMFLOAT3 mUp; // view space y-axis
	XMFLOAT3 mLook; // view space z-axis

	// Matrix cache
	XMFLOAT4X4 mView; // View matrix
	XMFLOAT4X4 mProj; // Projection matrix
	XMFLOAT4X4 mOrthographicProj; // 2D Projection matrix
	XMFLOAT4X4 mBaseView; // Base view matrix
	XMFLOAT4X4 mWorld;

	XMFLOAT4X4 mPrevViewProj;

	// Frustum cache
	float mNearZ;
	float mFarZ;
	float mAspect;
	float mFovY;
	float mNearWindowHeight;
	float mFarWindowHeight;

	float mYaw;
	float mPitch;
	float mRoll;

	DirectX::BoundingFrustum mFrustum;

};

#endif