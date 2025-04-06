#include "application.h"

Application::Application()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_TextureShader = 0;
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

	// D3D ����
	m_Direct3D = new D3D();
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// ī�޶� ���� �� ��ġ ����
	m_Camera = new Camera;
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	// �� ���� �� �ʱ�ȭ
	std::string filename("./Assets/backpack/backpack.obj");

	m_Model = new Model(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), filename);
	if (!m_Model)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// ���̴� ��ü ���� �� �ʱ�ȭ
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
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
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
	bool result;


	// Render the graphics scene.
	result = Render();
	if (!result)
	{
		return false;
	}
	return true;
}


bool Application::Render()
{
	Matrix matrix;

	// �� ���� Ŭ����
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// view Matrix ������Ʈ
	m_Camera->Render();

	// MVP ��ȯ �غ�
	m_Camera->GetViewMatrix(matrix.view);
	m_Direct3D->GetProjectionMatrix(matrix.projection);

	// �� ���ؽ�, �ε��� ������Ƽ�� ���ε�
	if (m_Model->Draw(m_Direct3D->GetDeviceContext(), m_TextureShader, matrix) == false)
		return false;

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
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