#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <iostream>

// 링크할 라이브러리
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "user32.lib")

// 전역 변수들
IDXGISwapChain* swapChain = nullptr;
ID3D11Device* device = nullptr;
ID3D11DeviceContext* deviceContext = nullptr;
ID3D11RenderTargetView* renderTargetView = nullptr;

// 윈도우 프로시저 함수
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

// DirectX 11 초기화 함수
bool InitDirectX(HWND hWnd)
{
	// Swap Chain 및 Device, Device Context 초기화
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 1;                                // 버퍼 하나 사용
	swapChainDesc.BufferDesc.Width = 800;                         // 창 너비
	swapChainDesc.BufferDesc.Height = 600;                        // 창 높이
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 색상 포맷
	swapChainDesc.SampleDesc.Count = 1;                           // 샘플링 1회
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;                             // 렌더링할 창
	swapChainDesc.Windowed = TRUE;                                 // 윈도우 모드
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
		&swapChainDesc, &swapChain, &device, nullptr, &deviceContext);

	if (FAILED(hr))
	{
		MessageBox(hWnd, L"DirectX 11 Device and SwapChain Creation Failed", L"Error", MB_OK);
		return false;
	}

	// 렌더 타겟 뷰 생성
	ID3D11Texture2D* backBuffer = nullptr;
	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	if (FAILED(hr))
	{
		MessageBox(hWnd, L"GetBuffer failed", L"Error", MB_OK);
		return false;
	}

	hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
	backBuffer->Release(); // 더 이상 필요하지 않으면 해제
	if (FAILED(hr))
	{
		MessageBox(hWnd, L"CreateRenderTargetView failed", L"Error", MB_OK);
		return false;
	}

	deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);

	// 뷰포트 설정
	D3D11_VIEWPORT viewport = {};
	viewport.Width = (float)800;
	viewport.Height = (float)600;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	deviceContext->RSSetViewports(1, &viewport);

	return true;
}

// WinMain 함수
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	// 윈도우 클래스 설정
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"DX11WindowClass";
	RegisterClass(&wc);

	// 윈도우 창 생성
	HWND hWnd = CreateWindowEx(
		0, wc.lpszClassName, L"DirectX 11 Example", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
		return 0;

	ShowWindow(hWnd, nCmdShow);

	// DirectX 초기화
	if (!InitDirectX(hWnd))
	{
		return 0; // 초기화 실패
	}

	// 메시지 루프
	MSG msg = {};
	while (true)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				return (int)msg.wParam;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// 배경을 검정색으로 채우기
		float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		deviceContext->ClearRenderTargetView(renderTargetView, clearColor);

		// 스왑체인 표시
		swapChain->Present(1, 0);
	}

	return 0;
}
