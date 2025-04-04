#include "camera.h"

Camera::Camera()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}


Camera::Camera(const Camera& other)
{
}


Camera::~Camera()
{
}

void Camera::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}


void Camera::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}

XMFLOAT3 Camera::GetPosition()
{
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}


XMFLOAT3 Camera::GetRotation()
{
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}

void Camera::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;


	// upVector 설정
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// upVector load
	upVector = XMLoadFloat3(&up);

	// position 설정
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// position load
	positionVector = XMLoadFloat3(&position);

	// camera front 설정
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// camera front load
	lookAtVector = XMLoadFloat3(&lookAt);

	// degree인 m_rotation을 Radian으로 변경 (0.0174532925 == pi / 180)
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// Roll, Pitch, Yaw를 통해 회전 행렬 생성
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// camera front, up 벡터를 world 좌표계로 변경
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// lookAtVector: 카메라가 실제 바라보는 절대 위치
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// View Matrix 생성
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;
}

void Camera::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}
