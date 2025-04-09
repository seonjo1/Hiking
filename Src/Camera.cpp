#include "camera.h"

Camera::Camera()
{

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

XMFLOAT3 Camera::GetFront()
{
	return m_cameraFront;
}

void Camera::Render()
{
	constexpr float DEG_TO_RAD = XM_PI / 180.0f;
	const XMFLOAT3 front = { 0.0f, 0.0f, 1.0f };

	XMVECTOR upVector, positionVector, frontVector;
	XMMATRIX rotationMatrix;

	// upVector load
	upVector = XMLoadFloat3(&m_cameraUp);
	positionVector = XMLoadFloat3(&m_cameraPos);
	frontVector = XMLoadFloat3(&front);

	// Roll, Pitch, Yaw를 통해 회전 행렬 생성
	rotationMatrix = XMMatrixRotationRollPitchYaw(m_cameraPitch * DEG_TO_RAD, m_cameraYaw * DEG_TO_RAD, 0.0f);

	// camera front, up 벡터를 world 좌표계로 변경
	frontVector = XMVector3TransformCoord(frontVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	XMStoreFloat3(&m_cameraFront, frontVector);

	// lookAtVector: 카메라가 실제 바라보는 절대 위치
	frontVector = XMVectorAdd(positionVector, frontVector);

	// View Matrix 생성
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

	// 이동 거리
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

    // XMVECTOR → XMFLOAT3 저장
    XMStoreFloat3(&m_cameraPos, pos);
}

void Camera::Rotate(int x, int y) {
	XMFLOAT2 pos{ static_cast<float>(x), static_cast<float>(y) };
	XMFLOAT2 prevMousePos{ static_cast<float>(m_prevMousePosX), static_cast<float>(m_prevMousePosY) };

	XMVECTOR currentPos = XMLoadFloat2(&pos);
	XMVECTOR prevPos = XMLoadFloat2(&prevMousePos);
	XMVECTOR deltaVec = XMVectorSubtract(prevPos, currentPos);
	deltaVec = XMVectorScale(deltaVec, m_cameraSensitivity);
	
	// deltaPos 저장
	XMFLOAT2 deltaPos;
	XMStoreFloat2(&deltaPos, deltaVec);

	// 회전 적용
	m_cameraYaw -= deltaPos.x * m_cameraRotSpeed;
	m_cameraPitch -= deltaPos.y * m_cameraRotSpeed;

	// Yaw 범위 제한 (0 ~ 360 wrap)
	if (m_cameraYaw < 0.0f)  m_cameraYaw += 360.0f;
	if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;

	// Pitch 제한 (-89 ~ +89도)
	if (m_cameraPitch > 89.0f)  m_cameraPitch = 89.0f;
	if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;

	// 이전 마우스 위치 갱신
	m_prevMousePosX = x;
	m_prevMousePosY = y;
}

void Camera::SaveCurrentPos(int x, int y)
{
	m_prevMousePosX = x;
	m_prevMousePosY = y;
}
