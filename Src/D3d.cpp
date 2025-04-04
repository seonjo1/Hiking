#include "d3d.h"

D3D::D3D()
{
	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
}

D3D::D3D(const D3D &other)
{
}

D3D::~D3D()
{
}

bool D3D::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth,
						  float screenNear)
{
	HRESULT result;
	IDXGIFactory *factory;
	IDXGIAdapter *adapter;
	IDXGIOutput *adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	size_t stringLength;
	DXGI_MODE_DESC *displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D *backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	float fieldOfView, screenAspect;

	// 수직 동기화 설정 (V-sync : 모니터의 프레임 출력과 GPU의 프레임 생성의 동기화)
	m_vsync_enabled = vsync;

	// DXGI 팩토리 생성
	// DXGI: GPU와 디스플레이 출력을 탐색하고 스왑 체인을 만들 수 있음
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&factory);
	if (FAILED(result))
	{
		return false;
	}

	// GPU 어댑터: GPU와 연결된 다른 하드웨어나 출력 장치(모니터 등)을 포함한 전체 장치를 지칭
	// 시스템에 설치된 GPU 어댑터를 나열한 후 0번째 저장
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	// GPU 어댑터에 연결된 출력 장치(모니터)를 나열한 후 0번째 저장
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// 특정 모니터에 대해 지원되는 디스플레이 모드(해상도, 주사율 등) 목록 가져오기 (현재는 개수만을 얻어옴)
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 디스플레이 모드 목록 개수만큼 배열 확보
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// 모니터가 지원하는 디스플레이 모드 목록 가져오기 (이번엔 배열에 담아 옴)
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes,
											   displayModeList);
	if (FAILED(result))
	{
		delete[] displayModeList;
		displayModeList = 0;
		return false;
	}

	// 디스플레이 모드 목록과 비교하여 해상도가 맞는 모드 찾아 저장
	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				// 해상도가 맞으면 주사율 저장 - [Numerator / denominator = 주사율 (ex: 60 / 1 = 60Hz)]
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// GPU 어댑터의 디스크립터(GPU 이름, 메모리 용량 등) 저장
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		delete[] displayModeList;
		displayModeList = 0;
		return false;
	}

	// GPU 메모리 크기 저장 (MB)
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// GPU 이름 문자열 복사
	// adapterDesc.Decription(wchar_t*) -> m_videoCardDescription(char*) 복사
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		delete[] displayModeList;
		displayModeList = 0;
		return false;
	}

	// displayModeList 해제
	delete[] displayModeList;
	displayModeList = 0;

	// COM 인터페이스 해제
	adapterOutput->Release();
	adapterOutput = 0;

	adapter->Release();
	adapter = 0;

	factory->Release();
	factory = 0;

	// 스왑 체인 객체 0으로 초기화
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// 백버퍼 1개 (프론트버퍼까지 총 2개)
	swapChainDesc.BufferCount = 1;

	// 백 버퍼 크기 설정
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// 백 버퍼 포맷 설정
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the refresh rate of the back buffer.
	if (m_vsync_enabled)
	{
		// 수직 동기화시 주사율 지정 (GPU FPS를 모니터의 FPS에 맞춤)
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		// GPU의 FPS 제한 해제
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// 스왑 체인의 백 버퍼의 용도를 렌더 타겟으로 설정
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// 출력 창 핸들 설정
	swapChainDesc.OutputWindow = hwnd;

	// 멀티 샘플링 비활성화
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	if (fullscreen)
	{
		// 창 모드 false
		swapChainDesc.Windowed = false;
	}
	else
	{
		// 창 모드 true
		swapChainDesc.Windowed = true;
	}

	// 디스플레이 모드 설정
	// 스캔라인 순서 설정 (UNSPECIFIED: 스캔라인 순서 자동 결정)
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	// 화면 해상도가 디스플레이 장치의 해상도와 맞지 않을때 화면의 크기를 어떻게 확장 혹은 축소할지 설정 (UNSPECIFIED:
	// 스케일링 방식 자동 결정)
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 스왑 방법 설정 (DXGI_SWAP_EFFECT_DISCARD : 백 버퍼 스왑 후 버리는 방식)
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// 스왑 체인에 대한 고급 설정 (0: 고급 설정 x)
	swapChainDesc.Flags = 0;

	// Direct3D 기능 수준 설정 (D3D_FEATURE_LEVEL_11_0: Direct3D 11의 가장 기본적인 기능 수준)
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// 디버그 레이어 추가
	UINT createDeviceFlags = 0;
#if defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// 스왑 체인, 디바이스, 디바이스 컨텍스트 생성
	// 디바이스: GPU를 제어하는 객체 (GPU와의 상호작용을 관리하고 리소스 생성)
	// 디바이스 컨텍스트: 디바이스와 실제 렌더링 작업을 수행하는 객체 (GPU에서 실제 작업을 실행하는 환경)
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION,
										   &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
	{
		return false;
	}

	// 백버퍼 얻어오기 (backBufferPtr에 백버퍼 포인터 얻어옴)
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	// 렌더 타겟 뷰 생성 (백버퍼를 렌더 타겟 뷰로 생성)
	// 렌더 타겟 뷰: GPU가 렌더링한 결과를 저장할 메모리 공간
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// 백버퍼 포인터 참조 해제
	backBufferPtr->Release();
	backBufferPtr = 0;

	// 깊이 버퍼 초기화
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// 깊이 버퍼 설정
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	// 24bit 깊이, 8bit 스텐실
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 멀티 샘플링 비활성화 (Count = 1 && Quality = 0)
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// 깊이 스텐실 2D텍스처 버퍼 생성
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 깊이 스텐실 설정 초기화
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// 깊이 테스트 활성화
	depthStencilDesc.DepthEnable = true;
	// 깊이 값 모든 픽셀에 대해 쓰기 가능 설정
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	// 깊이 비교 함수 설정 (현재 깊이 값이 더 작은 경우에만 픽셀이 그려지도록 함(근접 우선))
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	// 스텐실 테스트 활성화
	depthStencilDesc.StencilEnable = true;
	// 스텐실 읽기/쓰기 마스크 0xFF 설정 (모든 비트에 대해 읽기/쓰기가 가능)
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// 스텐실 연산 설정(픽셀이 앞면인 경우)
	// 스텐실 값이 테스트에 실패할 때 어떤 연산을 할지 정의 (KEEP: 스텐실 값 유지)
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	// 깊이 테스트에 실패할 때 어떤 연산을 할지 정의 (INCR: 스텐실 값 1 증가)
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	// 스텐실 테스트에 성공할 때 어떤 연산을 할지 정의 (KEEP: 스텐실 값 유지)
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	// 스텐실 값을 비교하는 방법 설정 (COMPARISON_ALWAYS: 항상 비교 연산 수행)
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 스텐실 연산 설정(픽셀이 앞면인 뒷면인 경우)
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 깊이 스텐실 상태 생성
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	// 렌더링 파이프라인에서 깊이 스텐실 상태 설정
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// 깊이 스텐실 뷰 초기화
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// 깊이 스텐실 뷰 설정
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// 깊이 스텐실 뷰 생성
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	// 파이프라인에 렌더 타겟 뷰랑 뎁스 스텐실 뷰 설정
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// 레스터화 설정
	// 안티 앨리어실 설정
	rasterDesc.AntialiasedLineEnable = false;
	// Cull 설정 (CULL_BACK: 뒤쪽 면 제거)
	rasterDesc.CullMode = D3D11_CULL_BACK;
	// 깊이 편향 설정 (값을 추가하면 깊이의 차이를 인위적으로 확대시켜 겹침 문제 해결)
	rasterDesc.DepthBias = 0;
	// 깊이 편향이 적용될 때 그 최대값 설정
	rasterDesc.DepthBiasClamp = 0.0f;
	// 깊이 클리핑 활성화 여부 (깊이가 화면의 범위 밖에 존재하면 그 버텍스들은 클리핑되어 렌더링 x)
	rasterDesc.DepthClipEnable = true;
	// 폴리곤을 채우는 방식 정의 (SOLID: 채워서 그림, WIREFRAME: 와이어 프레임 모드)
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	// 앞면 정의 (false: CCW가 뒷면)
	rasterDesc.FrontCounterClockwise = false;
	// 멀티 샘플링 여부
	rasterDesc.MultisampleEnable = false;
	// 화면 일부만 렌더링하는 시저 모드 여부
	rasterDesc.ScissorEnable = false;
	// 경사에 따른 깊이 편향 비율 (경사에 비례하여 깊이 편향을 동적으로 조정)
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// 레스터라이저 상태 생성
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		return false;
	}

	// 레스터라이저 상태 설정 (렌더링파이프라인에 설정)
	m_deviceContext->RSSetState(m_rasterState);

	// 뷰포트 설정
	m_viewport.Width = (float)screenWidth;
	m_viewport.Height = (float)screenHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	// 뷰포트 파이프라인에 설정
	m_deviceContext->RSSetViewports(1, &m_viewport);

	// FOV : pi / 4 = 45도
	fieldOfView = 3.141592654f / 4.0f;
	// aspect: width / height
	screenAspect = (float)screenWidth / (float)screenHeight;

	// 원근 투영 행렬 생성
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	// 월드 행렬 (단위 행렬 생성)
	m_worldMatrix = XMMatrixIdentity();

	// 직교 투영 행렬 생성
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
}

void D3D::Shutdown()
{
	// 스왑 체인 전체 화면 해제
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	// 레스터 상태 제거
	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	// 깊이 스텐실 뷰 제거
	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	// 깊이 스텐실 상태 제거
	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	// 깊이 스텐실 버퍼 제거
	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	// 렌더 타겟 뷰 제거
	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	// 디바이스 컨텍스트 제거
	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	// 디바이스 제거
	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	// 스왑 체인 제거
	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
}

void D3D::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// 백버퍼 해당 색으로 클리어
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// 깊이 버퍼 1.0(최대값)으로 클리어
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void D3D::EndScene()
{
	if (m_vsync_enabled)
	{
		// 수직 동기화 ON
		// Present(UINT SyncInterval, UINT Flags): 프론트 버퍼와 백 버퍼 교환
		// SyncInterval = 1: 수직 동기화 ON -> 1프레임 기다린 후 버퍼 교환 (스왑 체인에 설정한 것에 맞춤)
		m_swapChain->Present(1, 0);
	}
	else
	{
		// 수직 동기화 OFF
		// SyncInterval = 0: 수직 동기화 OFF
		m_swapChain->Present(0, 0);
	}

	return;
}

ID3D11Device *D3D::GetDevice()
{
	return m_device;
}

ID3D11DeviceContext *D3D::GetDeviceContext()
{
	return m_deviceContext;
}

void D3D::GetProjectionMatrix(XMMATRIX &projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}

void D3D::GetWorldMatrix(XMMATRIX &worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}

void D3D::GetOrthoMatrix(XMMATRIX &orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

void D3D::GetVideoCardInfo(char *cardName, int &memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}

void D3D::SetBackBufferRenderTarget()
{
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	return;
}

void D3D::ResetViewport()
{
	// Set the viewport.
	m_deviceContext->RSSetViewports(1, &m_viewport);

	return;
}