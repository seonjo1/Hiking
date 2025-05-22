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
#include <random>

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

	void CameraRotate(int x, int y);
	void SaveCameraCurrentPos(int x, int y);
	void setRayDebugMode(bool);
	void setBoneDebugMode(bool);
	void setRangeDebugMode(bool);
	void createSlope();
	void createGround();
	void createSpheres();
	void createStairs(int num);
	void createRandomTerrain(int num);
	void createSlopeTerrain(int num);
	void createSphere(XMFLOAT3 pos, XMFLOAT4 color);
	bool createAnimModel();
	bool initShaders(HWND hwnd);

private:
	bool Render();

	Timer m_Timer;
	D3D* m_Direct3D;
	Camera* m_Camera;
	Model* m_AnimationModel;
	std::vector<Model*> m_Models;
	
	BoneShader* m_BoneShader;
	JointShader* m_JointShader;
	ModelShader* m_ModelShader;
	TextureShader* m_TextureShader;
	PhysicsManager* m_PhysicsManager;

	bool m_boneDebugMode{ false };
	bool m_rayDebugMode{ false };
	bool m_rangeDebugMode{ false };
};