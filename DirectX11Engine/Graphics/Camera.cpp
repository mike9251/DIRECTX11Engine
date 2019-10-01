#include "Camera.h"

Camera::Camera()
{
	this->posVector = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMStoreFloat3(&this->pos, this->posVector);
	this->rotVector = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMStoreFloat3(&this->rot, this->rotVector);
	UpdateMatrix();
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

void Camera::UpdateMatrix()
{
	XMMATRIX cameraRotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z);
	// modify the direction in which the camera is looking
	XMVECTOR cameraTarget = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, cameraRotationMatrix);
	cameraTarget += this->posVector;
	// update up direction based on current rotation
	XMVECTOR upDirection = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, cameraRotationMatrix);

	this->viewMatrix = XMMatrixLookAtLH(this->posVector, cameraTarget, upDirection); // eyePos, lookAt, up direction

	this->UpdateDirectionVectors();
}