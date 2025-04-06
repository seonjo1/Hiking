#pragma once
#include <windows.h>
#include "d3d.h"
#include "camera.h"
#include "model.h"
#include "textureshader.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;

class Application
{
public:
	Application();
	Application(const Application&);
	~Application();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();
	void CameraControl(
		bool pressW, bool pressS, bool pressD,
		bool pressA, bool pressE, bool pressQ
	);

private:
	bool Render();

	D3D* m_Direct3D;
	Camera* m_Camera;
	Model* m_Model;
	TextureShader* m_TextureShader;
};