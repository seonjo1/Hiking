#pragma once
#include <windows.h>
#include "d3d.h"
#include "camera.h"
#include "model.h"
#include "Timer.h"
#include "jointShader.h"
#include "boneShader.h"
#include "modelShader.h"
#include "input.h"

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

	void CameraMove(
		bool pressW, bool pressS, bool pressD,
		bool pressA, bool pressE, bool pressQ
	);
	void ModelControl(Input*);
	XMFLOAT3 getDirection(int& inputState, bool pressUp, bool pressLeft, bool pressDown, bool pressRight);

	void setBoneDebugMode(bool);
	void setRangeDebugMode(bool);
	void setRayDebugMode(bool);
	void CameraRotate(int x, int y);
	void SaveCameraCurrentPos(int x, int y);

private:
	bool Render();

	D3D* m_Direct3D;
	Camera* m_Camera;
	Timer m_Timer;
	Model* m_AnimationModel;
	std::vector<Model*> m_Models;
	
	TextureShader* m_TextureShader;
	JointShader* m_JointShader;
	BoneShader* m_BoneShader;
	ModelShader* m_ModelShader;
	PhysicsManager* m_PhysicsManager;
	HeightMap m_heightMap;

	int m_modelCount;
	bool m_boneDebugMode{ false };
	bool m_rayDebugMode{ false };
	bool m_rangeDebugMode{ false };
};