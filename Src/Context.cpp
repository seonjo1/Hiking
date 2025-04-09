#include "Context.h"

Context::Context()
{
	m_Input = 0;
	m_Application = 0;
}

Context::Context(const Context& other)
{
}

Context::~Context()
{
}


bool Context::Initialize()
{
	int screenWidth, screenHeight;
	bool result;


	// Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight = 0;

	// Initialize the windows api.
	InitializeWindows(screenWidth, screenHeight);

	// Create and initialize the input object.  This object will be used to handle reading the keyboard input from the user.
	m_Input = new Input;

	m_Input->Initialize();
	// Create and initialize the application class object.  This object will handle rendering all the graphics for this application.
	m_Application = new Application;

	result = m_Application->Initialize(screenWidth, screenHeight, m_hwnd);
	if(!result)
	{
		return false;
	}
	
	return true;
}


void Context::Shutdown()
{
	// Release the application class object.
	if(m_Application)
	{
		m_Application->Shutdown();
		delete m_Application;
		m_Application = 0;
	}

	// Release the input object.
	if(m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	// Shutdown the window.
	ShutdownWindows();
	
	return;
}


void Context::Run()
{
	MSG msg;
	bool done, result;


	// �޽��� ����ü �ʱ�ȭ
	ZeroMemory(&msg, sizeof(MSG));

	// ���� ����
	done = false;
	while (!done)
	{
		// ������ �޽��� ó��
		// PeekMessage : ������ �޽��� ť���� �޽����� ������ 
		// PM_REMOVE   : ť���� �޽��� ������ ť���� ���� �޽��� ����
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// �޽����� Ű���� �Է��� ��� Ű���� �Է��� ���� �޽����� ��ȯ
			TranslateMessage(&msg);
			// �޽����� ������ ���ν����� ����
			DispatchMessage(&msg);
		}

		// ���� �޽��� ó�� (â ������, ���α׷� ����)
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// ������ ó��
			result = Frame();
			if (!result)
			{
				// ������ ó�� ���н� ����
				done = true;
			}
		}

	}

	return;
}

bool Context::Frame()
{
	bool result;

	// ESC �Է½� ����
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	if (m_cameraControl == true)
	{
		m_Application->CameraMove(
			m_Input->IsKeyDown('W'), m_Input->IsKeyDown('S'),
			m_Input->IsKeyDown('D'), m_Input->IsKeyDown('A'),
			m_Input->IsKeyDown('E'), m_Input->IsKeyDown('Q')
		);
	}

	m_Application->ModelControl(m_Input);
	m_Application->setDebugMode(m_Input->IsKeyDown('P'));

	// ���ø����̼� Ŭ������ ������ ó��
	result = m_Application->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}


LRESULT CALLBACK Context::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	/*
	hwnd: ������ â�� �ڵ�
	umsg: �޽��� �ĺ���
	wparam: �޽����� ���� �߰� ���� (Ű �ڵ� or ���콺 ��ư ���� ��)
	lparam: �޽����� ���� �߰����� ������ (Ű�� ���� Ÿ�̹�, �ݺ� ����, ���콺�� x/y��ǥ ��)
	*/
	switch (umsg)
	{
		case WM_MOUSEMOVE:
		{
			if (m_cameraControl == true)
			{
				m_Application->CameraRotate(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
			}
			break;
		}
		case WM_RBUTTONDOWN:
		{
			m_Application->SaveCameraCurrentPos(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
			m_cameraControl = true;
			SetCapture(hwnd);
			break;
		}

		case WM_RBUTTONUP:
		{
			m_cameraControl = false;
			ReleaseCapture();
			break;
		}
		// key ���� ��� �̺�Ʈ
		case WM_KEYDOWN:
		{
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}

		// key �� ��� �̺�Ʈ
		case WM_KEYUP:
		{
			// If a key is released then send it to the input object so it can unset the state for that key.
			m_Input->KeyUp((unsigned int)wparam);
			return 0;
		}

		// key down �� key up ������ ó�� (������ ũ�� ����, ���콺 Ŭ�� ��)
		default:
		{
			// DefWindowProc : ó������ ���� �޽����� �⺻������ ó�����ִ� �Լ� (ex: ������ ũ�� ����, �ý��� �޴�, ���콺 Ŭ�� �� �������� �⺻ ����)
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}


void Context::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	int posX, posY;


	// ���ø����̼� �ڵ� ���� (ContextClass pointer)
	ApplicationHandle = this;

	// ���� ���ø����̼��� �ν��Ͻ� �ڵ� ����
	m_hinstance = GetModuleHandle(NULL);

	// ���ø����̼� �̸� ����
	m_applicationName = L"Engine";

	// ������ Ŭ���� ���� (WNDCLASSEX ����ü �ʱ�ȭ)
	/*
		������ ��Ÿ�� ����
		CS_HREDRAW: ���� ũ�� ����� ������ �ٽ� �׸���
		CS_VREDRAW: ���� ũ�� ����� ������ �ٽ� �׸���
		CS_OWNDC:   ���� Device Context ���
	*/
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	// ������ ���ν���(WndProc)�� �����Ͽ� �޽��� ó��
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	// ������ ������ ����
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	// ������ Ŀ�� ����
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	// ������ ���� ����
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	// ������ Ŭ���� �̸� ����
	wc.lpszClassName = m_applicationName;
	// ����ü ũ��
	wc.cbSize = sizeof(WNDCLASSEX);

	// ������ Ŭ���� ���
	RegisterClassEx(&wc);

	// GetSystemMetrics()�� ���� ����� �ý����� ȭ�� �ػ� ũ�� ��������
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	DEVMODE dmScreenSettings;

	if (FULL_SCREEN)
	{
		// ��ü ȭ�� ����
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// ��üȭ�� ����
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// ȭ�� ���� ���� ��ġ
		posX = posY = 0;
	}
	else
	{
		// â ��� ����
		screenWidth = 1280;
		screenHeight = 720;

		// ȭ���� �߾ӿ� â ��ġ
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// ������ â ����
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_OVERLAPPEDWINDOW, posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// ShowWindow(): �����츦 ȭ�鿡 ǥ��
	ShowWindow(m_hwnd, SW_SHOW);

	// �����츦 ���� �տ� ��ġ
	SetForegroundWindow(m_hwnd);

	// �����쿡 ��Ŀ���� �����Ͽ� Ű���� �Է� ���� �ް� ��
	SetFocus(m_hwnd);

	// ���콺 Ŀ���� ȭ�鿡�� ����
	ShowCursor(true);

	return;
}


void Context::ShutdownWindows()
{
	// ���÷��� ���� ����
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// ������ ����
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// ������ Ŭ���� ��� ����
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	ApplicationHandle = NULL;

	return;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// �����찡 ������ �����ϴ� ��� (���ø����̼� ���� �� ���ҽ� �����ϰų� ������ �۾���)
		case WM_DESTROY:
		{
			// WM_QUIT ���� �޽����� �޽��� ť�� �߰��ϴ� �Լ�
			PostQuitMessage(0);
			return 0;
		}

		// ������ x ��ư Ŭ�� or Alt + F4
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		default:
		{
			// �޽��� �ڵ鷯�� ����
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}
