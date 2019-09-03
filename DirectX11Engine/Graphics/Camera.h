#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
public:
	Camera();
	void SetProjectionValues(float fovDegrees, float aspecctRatio, float nearZ, float farZ);

	const XMMATRIX & GetViewMatrix() const;
	const XMMATRIX & GetProjectionMatrix() const;

	const XMVECTOR & GetPositionVector() const;
	const XMFLOAT3 & GetPositionFloat3() const;

	const XMVECTOR & GetRotationVector() const;
	const XMFLOAT3 & GetRotationFloat3() const;

	const XMVECTOR & GetForwardVector() const;
	const XMVECTOR & GetBackwardVector() const;
	const XMVECTOR & GetLeftVector() const;
	const XMVECTOR & GetRightVector() const;

	void SetPosition(const XMVECTOR &pos);
	void SetPosition(float x, float y, float z);
	void AdjustPosition(const XMVECTOR &pos);
	void AdjustPosition(float x, float y, float z);

	void SetRotation(const XMVECTOR &rot);
	void SetRotation(float x, float y, float z);
	void AdjustRotation(const XMVECTOR &rot);
	void AdjustRotation(float x, float y, float z);

	void SetLookAtPosition(XMFLOAT3 &lookAtPos);

private:
	void UpdateViewMatrix();

	XMVECTOR posVector;
	XMVECTOR rotVector;
	XMFLOAT3 pos;
	XMFLOAT3 rot;
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;

	const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	// current forward/backward/left/right directions
	XMVECTOR vec_forward;
	XMVECTOR vec_backward;
	XMVECTOR vec_left;
	XMVECTOR vec_right;
};