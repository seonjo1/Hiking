#pragma once

/*
Windows API header
최적화 (windows.h 포함시 불필요한 매크로와 정의를 제외하여 컴파일 시간을 줄이고 파일 크기를 줄여 성능 개선)
*/
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include "application.h"

/*
애플리케이션의 메인 시스템을 담당하는 클래스
윈도우 초기화, 메시지 처리, 프레임 관리, 애플리케이션 실행 등의 기능 제공
*/
class Context
{
public:
	Context();
	Context(const Context& other);
	~Context();

	// 시스템 초기화 함수 (윈도우 초기화, 입력 처리, 애플리케이션 클래스 초기화)
	bool Initialize();

	// 시스템 종료 함수 (윈도우 창 닫고 자원 해제)
	void Shutdown();

	// 애플리케이션의 주 실행 루프를 처리하는 함수 (애플리케이션이 계속 실행되는 동안 프레임을 처리하고 메시지 처리)
	void Run();

	// 윈도우 메시지 처리 함수 (윈도우 창에서 발생하는 각종 입력이나 시스템 메시지 처리)
	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

private:
	// 애플리케이션의 프레임을 처리 (게임 로직이나 그래픽 처리)
	bool Frame();

	// 윈도우 창을 초기화하는 함수
	void InitializeWindows(int& screenWidth, int& screenHeight);
	
	// 윈도우 창을 종료하고 해제하는 함수
	void ShutdownWindows();

private:
	// 애플리케이션의 이름
	LPCWSTR m_applicationName;

	// 애플리케이션의 인스턴스 핸들
	HINSTANCE m_hinstance;

	// 윈도우 창의 핸들
	HWND m_hwnd;

	// 입력 처리 클래스
	Input* m_Input;

	// 애플리케이션 클래스
	Application* m_Application;

	bool m_cameraControl{ false };
};

/*
윈도우 프로시저 함수(윈도우 메시지를 처리하며, 기본적인 메시지 루프에서 사용하는 함수)
메시지 처리와 관련된 로직을 SystemClass의 MessageHandler 함수로 전달하는 역할
*/
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/*
ContextClass의 포인터
전역적으로 애플리케이션의 인스턴스 참조 가능
*/
static Context* ApplicationHandle = 0;