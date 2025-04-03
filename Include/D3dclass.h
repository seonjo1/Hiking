#pragma once

// 라이브러리 링킹
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

// Direct3D 11 API를 쓸 수 있게 해주는 헤더
#include <d3d11.h>
// DirectX 수학 라이브러리
#include <directxmath.h>

using namespace DirectX;

class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass& other);
	~D3DClass();

	bool Initialize(int , int, bool, HWND, bool, float, float);
	void Shutdown();

	// 렌더링 시작
	void BeginScene(float, float, float, float);
	
	// 렌더링 종료 후 디스플레이
	void EndScene();

	// Direct3D 디바이스 반환
	ID3D11Device* GetDevice();

	// 디바이스 컨텍스트 반환
	ID3D11DeviceContext* GetDeviceContext();

	// 투영 행렬 반환
	void GetProjectionMatrix(XMMATRIX&);

	// 월드 행렬 반환
	void GetWorldMatrix(XMMATRIX&);
	
	// 직교 행렬 반환
	void GetOrthoMatrix(XMMATRIX&);

	// GPU 정보 반환
	void GetVideoCardInfo(char*, int&);

	// 백버퍼 렌더링 타겟 설정
	void SetBackBufferRenderTarget();

	// 뷰 포트 초기화
	void ResetViewport();

private:

	/*
		버퍼 : 데이터 저장하는 메모리 공간
		상태 : 특정 설정이나 동작 방식
		뷰: 버퍼를 사용하는 방법
	*/

	// 수직 동기화 활성화 여부
	bool m_vsync_enabled;

	// GPU 메모리 크기(MB)
	int m_videoCardMemory;

	// GPU 이름
	char m_videoCardDescription[128];

	// 스왑 체인 객체
	IDXGISwapChain* m_swapChain;

	// Direct3D 디바이스 객체
	ID3D11Device* m_device;

	// 디바이스 컨텍스트 객체
	ID3D11DeviceContext* m_deviceContext;

	// 렌더 타겟 뷰 (렌더링 할 뷰 정의)
	ID3D11RenderTargetView* m_renderTargetView;

	// 깊이 스텐실 버퍼, 상태, 뷰 객체
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;

	// 레스터화 상태 객체
	ID3D11RasterizerState* m_rasterState;

	// 투영 행렬
	XMMATRIX m_projectionMatrix;

	// 월드 행렬
	XMMATRIX m_worldMatrix;

	// 직교 행렬
	XMMATRIX m_orthoMatrix;

	// 뷰포트 객체
	D3D11_VIEWPORT m_viewport;
};
