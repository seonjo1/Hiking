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
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	// 모델 생성 및 초기화
	std::string filename("./Assets/Character/Character.gltf");

	Model* model = new Model(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), filename);
	if (!model)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_Models.push_back(model);


	m_modelCount = m_Models.size();
	
	// 셰이더 객체 생성 및 초기화
	m_TextureShader = new TextureShader;
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void Application::Shutdown()
{
	// Release the texture shader object.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
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

	// 모델 버텍스, 인덱스 프리미티브 바인딩
	for (int i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i]->Draw(m_Direct3D->GetDeviceContext(), m_TextureShader, matrix) == false)
			return false;
	}

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}

void Application::UpdateAnimation(float dt)
{
	//p(to_string(dt) + "\n");
	for (int i = 0; i < m_modelCount; i++)
	{
		m_Models[i]->UpdateAnimation(dt);
	}
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