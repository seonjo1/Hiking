#include "application.h"

Application::Application()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_TextureShader = 0;
	m_modelCount = 0;
}


Application::Application(const Application& other)
{
}


Application::~Application()
{
}


bool Application::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// D3D 생성
	m_Direct3D = new D3D();
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// 카메라 생성 및 위치 설정
	m_Camera = new Camera;
	m_Camera->SetPosition(0.0f, 1.0f, -15.0f);

	// 모델 생성 및 초기화
	std::string filename("./Assets/Remy.glb");
	//std::string filename("./Assets/Character/Character.gltf");

	m_AnimationModel = new Model(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), filename);
	if (!m_AnimationModel)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}
	m_AnimationModel->setRotation(XMFLOAT3(-90.0f, 0.0f, 0.0f));
	m_AnimationModel->setScale(XMFLOAT3(0.02f, 0.02f, 0.02f));

	m_modelCount = m_Models.size();
	
	// 셰이더 객체 생성 및 초기화
	m_TextureShader = new TextureShader;
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	m_JointShader = new JointShader;
	result = m_JointShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the joint shader object.", L"Error", MB_OK);
		return false;
	}

	m_BoneShader = new BoneShader;
	result = m_BoneShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the bone shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void Application::Shutdown()
{
	if (m_AnimationModel)
	{
		m_AnimationModel->Shutdown();
		delete m_AnimationModel;
	}

	// Release the texture shader object.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	if (m_JointShader)
	{
		m_JointShader->Shutdown();
		delete m_JointShader;
		m_JointShader = 0;
	}

	if (m_BoneShader)
	{
		m_BoneShader->Shutdown();
		delete m_BoneShader;
		m_BoneShader = 0;
	}

	// Release the model object.
	for (int i = 0; i < m_Models.size(); i++)
	{
		m_Models[i]->Shutdown();
		delete m_Models[i];
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the Direct3D object.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
	return;
}

bool Application::Frame()
{
	float dt = m_Timer.GetDeltaTime();

	// animation
	UpdateAnimation(dt);

	// Render the graphics scene.
	bool result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool Application::Render()
{
	Matrix matrix;

	// 백 버퍼 클리어
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// view Matrix 업데이트
	m_Camera->Render();

	// MVP 변환 준비
	m_Camera->GetViewMatrix(matrix.view);
	m_Direct3D->GetProjectionMatrix(matrix.projection);

	if (m_debugMode == true)
	{
		m_AnimationModel->DrawJointShader(m_Direct3D->GetDeviceContext(), m_JointShader, matrix);
		m_AnimationModel->DrawBoneShader(m_Direct3D->GetDeviceContext(), m_BoneShader, matrix);
	}
	else
	{
		m_AnimationModel->DrawTextureShader(m_Direct3D->GetDeviceContext(), m_TextureShader, matrix);

		for (int i = 0; i < m_Models.size(); i++)
		{
			//if (m_Models[i]->DrawTextureShader(m_Direct3D->GetDeviceContext(), m_TextureShader, matrix) == false)
			//	return false;
		}
	}

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}

void Application::UpdateAnimation(float dt)
{
	m_AnimationModel->UpdateAnimation(dt);
}

void Application::CameraMove(
	bool pressW, bool pressS, bool pressD,
	bool pressA, bool pressE, bool pressQ
){
	m_Camera->Move(pressW, pressS, pressD, pressA, pressE, pressQ);
}

void Application::CameraRotate(int x, int y)
{
	m_Camera->Rotate(x, y);
}

void Application::SaveCameraCurrentPos(int x, int y)
{
	m_Camera->SaveCurrentPos(x, y);
}

XMFLOAT3 Application::getDirection(int& inputState, bool pressUp, bool pressLeft, bool pressDown, bool pressRight) {
	XMFLOAT3 movement = { 0.0f, 0.0f, 0.0f };

	if (!pressUp && !pressLeft && !pressDown && !pressRight) {
		return movement;
	}

	if (pressUp) {
		movement.z += 1.0f;
	}
	if (pressDown) {
		movement.z -= 1.0f;
	}
	if (pressLeft) {
		movement.x -= 1.0f;
	}
	if (pressRight) {
		movement.x += 1.0f;
	}

	XMVECTOR v = XMLoadFloat3(&movement);
	XMVECTOR lengthVec = XMVector3Length(v);
	float length;
	XMStoreFloat(&length, lengthVec);

	if (length < 1.0f) {
		return XMFLOAT3(0.0f, 0.0f, 0.0f);
	}

	XMVECTOR normalizedVec = XMVector3Normalize(v);
	XMStoreFloat3(&movement, normalizedVec);

	inputState = inputState | 1;

	return movement;
}

void Application::ModelControl(Input* input)
{
	int inputState = 0;

	bool pressUp = input->IsKeyDown(VK_UP);
	bool pressLeft = input->IsKeyDown(VK_LEFT);
	bool pressDown = input->IsKeyDown(VK_DOWN);
	bool pressRight = input->IsKeyDown(VK_RIGHT);
	XMFLOAT3 dir = getDirection(inputState, pressUp, pressLeft, pressDown, pressRight);

	if (inputState){
		m_AnimationModel->move(dir);
		//m_Models[0]->setState("Walk");
	}
	else {
		m_AnimationModel->speedDown();
	}
	
}

void Application::setDebugMode(bool flag)
{
	m_debugMode = flag;
}
