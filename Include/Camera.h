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
	XMFLOAT3 GetFront();

	void Render();
	void GetViewMatrix(XMMATRIX&);
	void Move(
		bool pressW, bool pressS, bool pressD,
		bool pressA, bool pressE, bool pressQ
	);
	void Rotate(int x, int y);
	void SaveCurrentPos(int x, int y);

private:
	float m_cameraPitch{ 0.0f };
	float m_cameraYaw{ 0.0f };
	int m_prevMousePosX{ 0 };
	int m_prevMousePosY{ 0 }; 
	XMFLOAT3 m_cameraPos{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_cameraFront{ 0.0f, 0.0f, 1.0f };
	XMFLOAT3 m_cameraUp{ 0.0f, 1.0f, 0.0f };
	XMMATRIX m_viewMatrix;

	const float m_cameraSpeed{ 0.2f };
	const float m_cameraSensitivity{ 0.5f };
	const float m_cameraRotSpeed{ 0.4f };
};
