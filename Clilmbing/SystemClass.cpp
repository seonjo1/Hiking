#include "systemclass.h"

SystemClass::SystemClass()
{
	m_Input = 0;
	m_Application = 0;
}

SystemClass::SystemClass(const SystemClass& other)
{
}


SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;


	// 화면 크기 초기화
	screenWidth = 0;
	screenHeight = 0;

	// 윈도우 창 초기화
	InitializeWindows(screenWidth, screenHeight);

	// 입력 객체 초기화
	m_Input = new InputClass;
	m_Input->Initialize();

	// 애플리케이션 객체 초기화
	m_Application = new ApplicationClass;
	result = m_Application->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}

void SystemClass::Shutdown()
{
	// 애플리케이션 객체 존재시 해제
	if (m_Application)
	{
		m_Application->Shutdown();
		delete m_Application;
		m_Application = 0;
	}

	// 인풋 객체 존재시 해제
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	// 윈도우 창 제거
	ShutdownWindows();

	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;


	// 메시지 구조체 초기화
	ZeroMemory(&msg, sizeof(MSG));

	// 루프 시작
	done = false;
	while (!done)
	{
		// 윈도우 메시지 처리
		// PeekMessage : 윈도우 메시지 큐에서 메시지를 가져옴 
		// PM_REMOVE   : 큐에서 메시지 꺼낼때 큐에서 꺼낸 메시지 제거
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// 메시지가 키보드 입력일 경우 키보드 입력을 문자 메시지로 변환
			TranslateMessage(&msg);
			// 메시지를 윈도우 프로시저로 전달
			DispatchMessage(&msg);
		}

		// 종료 메시지 처리 (창 닫을때, 프로그램 종료)
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// 프레임 처리
			result = Frame();
			if (!result)
			{
				// 프레임 처리 실패시 종료
				done = true;
			}
		}

	}

	return;
}

bool SystemClass::Frame()
{
	bool result;

	// ESC 입력시 종료
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// 애플리케이션 클래스의 프레임 처리
	result = m_Application->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	/*
	hwnd: 윈도우 창의 핸들
	umsg: 메시지 식별자
	wparam: 메시지에 대한 추가 정보 (키 코드 or 마우스 버튼 상태 등)
	lparam: 메시지에 대한 추가적인 데이터 (키가 눌린 타이밍, 반복 여부, 마우스의 x/y좌표 등)

	*/
	switch (umsg)
	{
		// key 눌린 경우 이벤트
		case WM_KEYDOWN:
		{
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}

		// key 뗀 경우 이벤트
		case WM_KEYUP:
		{
			// If a key is released then send it to the input object so it can unset the state for that key.
			m_Input->KeyUp((unsigned int)wparam);
			return 0;
		}

		// key down 과 key up 제외한 처리 (윈도우 크기 변경, 마우스 클릭 등)
		default:
		{
			// DefWindowProc : 처리되지 않은 메시지를 기본적으로 처리해주는 함수 (ex: 윈도우 크기 변경, 시스템 메뉴, 마우스 클릭 등 윈도우의 기본 동작)
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	int posX, posY;


	// 애플리케이션 핸들 설정 (SystemClass pointer)
	ApplicationHandle = this;

	// 현재 애플리케이션의 인스턴스 핸들 저장
	m_hinstance = GetModuleHandle(NULL);

	// 애플리케이션 이름 설정
	m_applicationName = L"Engine";

	// 윈도우 클래스 설정 (WNDCLASSEX 구조체 초기화)
	/*
		윈도우 스타일 설정
		CS_HREDRAW: 세로 크기 변경시 윈도우 다시 그리기
		CS_VREDRAW: 가로 크기 변경시 윈도우 다시 그리기
		CS_OWNDC:   고유 Device Context 사용
	*/
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	// 윈도우 프로시저(WndProc)를 지정하여 메시지 처리
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	// 윈도우 아이콘 설정
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	// 윈도우 커서 설정
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	// 윈도우 배경색 설정
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	// 윈도우 클래스 이름 설정
	wc.lpszClassName = m_applicationName;
	// 구조체 크기
	wc.cbSize = sizeof(WNDCLASSEX);

	// 윈도우 클래스 등록
	RegisterClassEx(&wc);

	// GetSystemMetrics()를 통해 사용자 시스템의 화면 해상도 크기 가져오기
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	DEVMODE dmScreenSettings;

	if (FULL_SCREEN)
	{
		// 전체 화면 설정
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// 전체화면 설정
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// 화면 왼쪽 위에 배치
		posX = posY = 0;
	}
	else
	{
		// 창 모드 설정
		screenWidth = 1920;
		screenHeight = 1080;

		// 화면의 중앙에 창 배치
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// 윈도우 창 생성
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// ShowWindow(): 윈도우를 화면에 표시
	ShowWindow(m_hwnd, SW_SHOW);

	// 윈도우를 가장 앞에 배치
	SetForegroundWindow(m_hwnd);

	// 윈도우에 포커스를 설정하여 키보드 입력 등을 받게 함
	SetFocus(m_hwnd);

	// 마우스 커서를 화면에서 숨김
	ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows()
{
	// 마우스 커서 다시 표시
	ShowCursor(true);

	// 디스플레이 설정 복원
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// 윈도우 제거
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// 윈도우 클래스 등록 해제
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// 윈도우가 실제로 종료하는 경우 (애플리케이션 종료 후 리소스 해제하거나 마지막 작업시)
		case WM_DESTROY:
		{
			// WM_QUIT 종료 메시지를 메시지 큐에 추가하는 함수
			PostQuitMessage(0);
			return 0;
		}

		// 윈도우 x 버튼 클릭 or Alt + F4
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		default:
		{
			// 메시지 핸들러로 전달
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}