#include "Camera.h"

Camera::Camera()
{
	this->posVector = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMStoreFloat3(&this->pos, this->posVector);
	this->rotVector = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMStoreFloat3(&this->rot, this->rotVector);
	UpdateViewMatrix();
}

void Camera::SetProjectionValues(float fovDegrees, float aspecctRatio, float nearZ, float farZ)
{
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	this->projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspecctRatio, nearZ, farZ);
}

const XMMATRIX & Camera::GetViewMatrix() const
{
	return this->viewMatrix;
}

const XMMATRIX & Camera::GetProjectionMatrix() const
{
	return this->projectionMatrix;
}

const XMVECTOR & Camera::GetPositionVector() const
{
	return this->posVector;
}

const XMFLOAT3 & Camera::GetPositionFloat3() const
{
	return this->pos;
}

const XMVECTOR & Camera::GetRotationVector() const
{
	return this->rotVector;
}

const XMFLOAT3 & Camera::GetRotationFloat3() const
{
	return this->rot;
}

const XMVECTOR & Camera::GetForwardVector() const
{
	return this->vec_forward;
}

const XMVECTOR & Camera::GetBackwardVector() const
{
	return this->vec_backward;
}

const XMVECTOR & Camera::GetLeftVector() const
{
	return this->vec_left;
}

const XMVECTOR & Camera::GetRightVector() const
{
	return this->vec_right;
}

void Camera::SetPosition(const XMVECTOR & pos)
{
	this->posVector = pos;
	XMStoreFloat3(&this->pos, pos);
	UpdateViewMatrix();
}

void Camera::SetPosition(float x, float y, float z)
{
	this->pos = XMFLOAT3(x, y, z);
	this->posVector = XMLoadFloat3(&this->pos);
	UpdateViewMatrix();
}

void Camera::AdjustPosition(const XMVECTOR & pos)
{
	this->posVector += pos;
	XMStoreFloat3(&this->pos, this->posVector);
	UpdateViewMatrix();
}

void Camera::AdjustPosition(float x, float y, float z)
{
	this->pos.x += x;
	this->pos.y += y;
	this->pos.z += z;
	this->posVector = XMLoadFloat3(&this->pos);
	UpdateViewMatrix();
}

void Camera::SetRotation(const XMVECTOR & rot)
{
	this->rotVector = rot;
	XMStoreFloat3(&this->rot, this->rotVector);
	UpdateViewMatrix();
}

void Camera::SetRotation(float x, float y, float z)
{
	this->rot = XMFLOAT3(x, y, z);
	this->rotVector = XMLoadFloat3(&this->rot);
	UpdateViewMatrix();
}

void Camera::AdjustRotation(const XMVECTOR & rot)
{
	this->rotVector += rot;
	XMStoreFloat3(&this->rot, this->rotVector);
	UpdateViewMatrix();
}

void Camera::AdjustRotation(float x, float y, float z)
{
	this->rot.x += x;
	this->rot.y += y;
	this->rot.z += z;
	this->rotVector = XMLoadFloat3(&this->rot);
	UpdateViewMatrix();
}

void Camera::SetLookAtPosition(XMFLOAT3 & lookAtPos)
{
	if ((lookAtPos.x == this->pos.x) && (lookAtPos.y == this->pos.y) && (lookAtPos.z == this->pos.z))
		return;

	// get vector from camera position to lookAt position
	lookAtPos.x = this->pos.x - lookAtPos.x;
	lookAtPos.y = this->pos.y - lookAtPos.y;
	lookAtPos.z = this->pos.z - lookAtPos.z;

	float pitch = 0.0f;
	// if the camera is on the same Y value with Y lookAtPos value then pitch = 0 (no rotation along X-axis)
	if (lookAtPos.y != 0.0f)
	{
		float distance = sqrtf(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
		pitch = atan(lookAtPos.y / distance);
	}

	float yaw = 0.0f;
	// calculate yaw angle if the camera is not at the same X position of the lookAtPosition
	if (lookAtPos.x != 0.0f)
	{
		yaw = atan(lookAtPos.x / lookAtPos.z);
	}
	// rotate 180 degree if the camera
	if (lookAtPos.z > 0.0f)
	{
		yaw += XM_PI;
	}

	this->SetRotation(pitch, yaw, 0.0f);
}

void Camera::UpdateViewMatrix()
{
	XMMATRIX cameraRotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z);
	// modify the direction in which the camera is looking
	XMVECTOR cameraTarget = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, cameraRotationMatrix);
	cameraTarget += this->posVector;
	// update up direction based on current rotation
	XMVECTOR upDirection = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, cameraRotationMatrix);

	this->viewMatrix = XMMatrixLookAtLH(this->posVector, cameraTarget, upDirection); // eyePos, lookAt, up direction

	// Get rotation matrix for direction vectors
	XMMATRIX vec_RotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, this->rot.y, 0.0f);

	this->vec_forward = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vec_RotationMatrix);
	this->vec_backward = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vec_RotationMatrix);
	this->vec_left = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vec_RotationMatrix);
	this->vec_right = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vec_RotationMatrix);
}
