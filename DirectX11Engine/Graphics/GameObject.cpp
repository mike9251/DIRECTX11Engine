#include "GameObject.h"

#include "Model.h"

const XMVECTOR & GameObject::GetPositionVector() const
{
	return this->posVector;
}

const XMFLOAT3 & GameObject::GetPositionFloat3() const
{
	return this->pos;
}

const XMVECTOR & GameObject::GetRotationVector() const
{
	return this->rotVector;
}

const XMFLOAT3 & GameObject::GetRotationFloat3() const
{
	return this->rot;
}

const XMVECTOR & GameObject::GetForwardVector(bool omitY) const
{
	if (omitY)
		return this->vec_forward_noY;
	return this->vec_forward;
}

const XMVECTOR & GameObject::GetBackwardVector(bool omitY) const
{
	if (omitY)
		return this->vec_backward_noY;
	return this->vec_backward;
}

const XMVECTOR & GameObject::GetLeftVector(bool omitY) const
{
	if (omitY)
		return this->vec_left_noY;
	return this->vec_left;
}

const XMVECTOR & GameObject::GetRightVector(bool omitY) const
{
	if (omitY)
		return this->vec_right_noY;
	return this->vec_right;
}

void GameObject::SetPosition(const XMVECTOR & pos)
{
	this->posVector = pos;
	XMStoreFloat3(&this->pos, pos);
	UpdateMatrix();
}

void GameObject::SetPosition(const XMFLOAT3 & pos)
{
	this->pos = pos;
	UpdateMatrix();
}

void GameObject::SetPosition(float x, float y, float z)
{
	this->pos = XMFLOAT3(x, y, z);
	this->posVector = XMLoadFloat3(&this->pos);
	UpdateMatrix();
}

void GameObject::AdjustPosition(const XMVECTOR & pos)
{
	this->posVector += pos;
	XMStoreFloat3(&this->pos, this->posVector);
	UpdateMatrix();
}

void GameObject::AdjustPosition(const XMFLOAT3 & pos)
{
	this->pos.x += pos.x;
	this->pos.y += pos.y;
	this->pos.z += pos.z;
	this->posVector = XMLoadFloat3(&this->pos);
	UpdateMatrix();
}

void GameObject::AdjustPosition(float x, float y, float z)
{
	this->pos.x += x;
	this->pos.y += y;
	this->pos.z += z;
	this->posVector = XMLoadFloat3(&this->pos);
	UpdateMatrix();
}

void GameObject::SetRotation(const XMVECTOR & rot)
{
	this->rotVector = rot;
	XMStoreFloat3(&this->rot, this->rotVector);
	UpdateMatrix();
}

void GameObject::SetRotation(const XMFLOAT3 & rot)
{
	this->rot = rot;
	this->rotVector = XMLoadFloat3(&this->rot);
	UpdateMatrix();
}

void GameObject::SetRotation(float x, float y, float z)
{
	this->rot = XMFLOAT3(x, y, z);
	this->rotVector = XMLoadFloat3(&this->rot);
	UpdateMatrix();
}

void GameObject::AdjustRotation(const XMVECTOR & rot)
{
	this->rotVector += rot;
	XMStoreFloat3(&this->rot, this->rotVector);
	UpdateMatrix();
}

void GameObject::AdjustRotation(const XMFLOAT3 & rot)
{
	this->rot.x += rot.x;
	this->rot.y += rot.y;
	this->rot.z += rot.z;
	this->rotVector = XMLoadFloat3(&this->rot);
	UpdateMatrix();
}

void GameObject::AdjustRotation(float x, float y, float z)
{
	this->rot.x += x;
	this->rot.y += y;
	this->rot.z += z;
	this->rotVector = XMLoadFloat3(&this->rot);
	UpdateMatrix();
}

void GameObject::SetLookAtPosition(XMFLOAT3 & lookAtPos)
{
	if ((lookAtPos.x == this->pos.x) && (lookAtPos.y == this->pos.y) && (lookAtPos.z == this->pos.z))
		return;

	// get vector from Model position to lookAt position
	lookAtPos.x = this->pos.x - lookAtPos.x;
	lookAtPos.y = this->pos.y - lookAtPos.y;
	lookAtPos.z = this->pos.z - lookAtPos.z;

	float pitch = 0.0f;
	// if the Model is on the same Y value with Y lookAtPos value then pitch = 0 (no rotation along X-axis)
	if (lookAtPos.y != 0.0f)
	{
		float distance = sqrtf(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
		pitch = atan(lookAtPos.y / distance);
	}

	float yaw = 0.0f;
	// calculate yaw angle if the Model is not at the same X position of the lookAtPosition
	if (lookAtPos.x != 0.0f)
	{
		yaw = atan(lookAtPos.x / lookAtPos.z);
	}
	// rotate 180 degree if the Model
	if (lookAtPos.z > 0.0f)
	{
		yaw += XM_PI;
	}

	this->SetRotation(pitch, yaw, 0.0f);
}

void GameObject::UpdateMatrix()
{
	assert("Must be overriden!");
}

void GameObject::UpdateDirectionVectors()
{
	// Get rotation matrix for direction vectors
	XMMATRIX vec_RotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, 0.0f);
	this->vec_forward = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vec_RotationMatrix);
	this->vec_backward = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vec_RotationMatrix);
	this->vec_left = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vec_RotationMatrix);
	this->vec_right = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vec_RotationMatrix);

	XMMATRIX vec_RotationMatrix_noY = XMMatrixRotationRollPitchYaw(0.0f, this->rot.y, 0.0f);
	this->vec_forward_noY = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vec_RotationMatrix_noY);
	this->vec_backward_noY = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vec_RotationMatrix_noY);
	this->vec_left_noY = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vec_RotationMatrix_noY);
	this->vec_right_noY = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vec_RotationMatrix_noY);
}
