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
	m_cameraPos = { x, y, z };
	return;
}


void Camera::SetRotation(float x, float y)
{
	m_cameraPitch = x;
	m_cameraYaw = y;
	return;
}

XMFLOAT3 Camera::GetPosition()
{
	return m_cameraPos;
}


XMFLOAT3 Camera::GetRotation()
{
	return XMFLOAT3(m_cameraPitch, m_cameraYaw, 0.0f);
}

void Camera::Render()
{
	XMVECTOR upVector, positionVector, frontVector;
	XMMATRIX rotationMatrix;

	// upVector load
	upVector = XMLoadFloat3(&m_cameraUp);

	// position load
	positionVector = XMLoadFloat3(&m_cameraPos);

	// camera front load
	frontVector = XMLoadFloat3(&m_cameraFront);

	// Roll, Pitch, Yaw�� ���� ȸ�� ��� ����
	rotationMatrix = XMMatrixRotationRollPitchYaw(m_cameraPitch, m_cameraYaw, 0.0f);

	// camera front, up ���͸� world ��ǥ��� ����
	frontVector = XMVector3TransformCoord(frontVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// lookAtVector: ī�޶� ���� �ٶ󺸴� ���� ��ġ
	frontVector = XMVectorAdd(positionVector, frontVector);

	// View Matrix ����
	m_viewMatrix = XMMatrixLookAtLH(positionVector, frontVector, upVector);

	return;
}

void Camera::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}

void Camera::Move(
	bool pressW, bool pressS, bool pressD,
	bool pressA, bool pressE, bool pressQ
){

	XMVECTOR pos = XMLoadFloat3(&m_cameraPos);
	XMVECTOR front = XMLoadFloat3(&m_cameraFront);
	XMVECTOR up = XMLoadFloat3(&m_cameraUp);

	XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, front));
	XMVECTOR camUp = XMVector3Normalize(XMVector3Cross(front, right));

	// �̵� �Ÿ�
    XMVECTOR moveStep;

    if (pressW) {
        moveStep = XMVectorScale(front, m_cameraSpeed);
        pos = XMVectorAdd(pos, moveStep);
    }
    if (pressS) {
        moveStep = XMVectorScale(front, m_cameraSpeed);
        pos = XMVectorSubtract(pos, moveStep);
    }
    if (pressD) {
        moveStep = XMVectorScale(right, m_cameraSpeed);
        pos = XMVectorAdd(pos, moveStep);
    }
    if (pressA) {
        moveStep = XMVectorScale(right, m_cameraSpeed);
        pos = XMVectorSubtract(pos, moveStep);
    }
    if (pressE) {
        moveStep = XMVectorScale(camUp, m_cameraSpeed);
        pos = XMVectorAdd(pos, moveStep);
    }
    if (pressQ) {
        moveStep = XMVectorScale(camUp, m_cameraSpeed);
        pos = XMVectorSubtract(pos, moveStep);
    }

    // XMVECTOR �� XMFLOAT3 ����
    XMStoreFloat3(&m_cameraPos, pos);
}

