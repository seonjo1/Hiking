#pragma once

#include <Directxmath.h>
using namespace DirectX;

class Camera
{
public:
	Camera();
	Camera(const Camera&);
	~Camera();

	void SetPosition(float, float, float);
	void SetRotation(float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(XMMATRIX&);
	void Move(
		bool pressW, bool pressS, bool pressD,
		bool pressA, bool pressE, bool pressQ
	);

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	XMMATRIX m_viewMatrix;

	float m_cameraPitch{ 0.0f };
	float m_cameraYaw{ 0.0f };
	XMFLOAT2 m_prevMousePos{ 0.0f, 0.0f };
	XMFLOAT3 m_cameraPos{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_cameraFront{ 0.0f, 0.0f, 1.0f };
	XMFLOAT3 m_cameraUp{ 0.0f, 1.0f, 0.0f };
	const float m_cameraSpeed{ 0.04f };
	const float m_cameraRotSpeed{ 0.2f };
};
