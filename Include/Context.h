#pragma once

/*
Windows API header
����ȭ (windows.h ���Խ� ���ʿ��� ��ũ�ο� ���Ǹ� �����Ͽ� ������ �ð��� ���̰� ���� ũ�⸦ �ٿ� ���� ����)
*/
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include "application.h"

/*
���ø����̼��� ���� �ý����� ����ϴ� Ŭ����
������ �ʱ�ȭ, �޽��� ó��, ������ ����, ���ø����̼� ���� ���� ��� ����
*/
class Context
{
public:
	Context();
	Context(const Context& other);
	~Context();

	// �ý��� �ʱ�ȭ �Լ� (������ �ʱ�ȭ, �Է� ó��, ���ø����̼� Ŭ���� �ʱ�ȭ)
	bool Initialize();

	// �ý��� ���� �Լ� (������ â �ݰ� �ڿ� ����)
	void Shutdown();

	// ���ø����̼��� �� ���� ������ ó���ϴ� �Լ� (���ø����̼��� ��� ����Ǵ� ���� �������� ó���ϰ� �޽��� ó��)
	void Run();

	// ������ �޽��� ó�� �Լ� (������ â���� �߻��ϴ� ���� �Է��̳� �ý��� �޽��� ó��)
	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

private:
	// ���ø����̼��� �������� ó�� (���� �����̳� �׷��� ó��)
	bool Frame();

	// ������ â�� �ʱ�ȭ�ϴ� �Լ�
	void InitializeWindows(int& screenWidth, int& screenHeight);
	
	// ������ â�� �����ϰ� �����ϴ� �Լ�
	void ShutdownWindows();

private:
	// ���ø����̼��� �̸�
	LPCWSTR m_applicationName;

	// ���ø����̼��� �ν��Ͻ� �ڵ�
	HINSTANCE m_hinstance;

	// ������ â�� �ڵ�
	HWND m_hwnd;

	// �Է� ó�� Ŭ����
	Input* m_Input;

	// ���ø����̼� Ŭ����
	Application* m_Application;

	bool m_cameraControl{ false };
};

/*
������ ���ν��� �Լ�(������ �޽����� ó���ϸ�, �⺻���� �޽��� �������� ����ϴ� �Լ�)
�޽��� ó���� ���õ� ������ SystemClass�� MessageHandler �Լ��� �����ϴ� ����
*/
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/*
ContextClass�� ������
���������� ���ø����̼��� �ν��Ͻ� ���� ����
*/
static Context* ApplicationHandle = 0;