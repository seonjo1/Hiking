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
	m_Camera->SetPosition(0.0f, 5.0f, -30.0f);

	// 물리 엔진 초기화
	m_PhysicsManager = new PhysicsManager();
	m_PhysicsManager->initialize();

	// 애니메이션 모델 생성
	std::string filename("./Assets/Remy1.glb");

	m_AnimationModel = new Model(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), filename);
	if (!m_AnimationModel)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}
	m_AnimationModel->setPosition(XMFLOAT3(0.0f, -0.15f, 0.0f));
	//m_AnimationModel->setPosition(XMFLOAT3(0.0f, 10.0f, 0.0f));
	m_AnimationModel->setRotation(XMFLOAT3(-90.0f, 0.0f, 0.0f));
	m_AnimationModel->setScale(XMFLOAT3(0.02f, 0.02f, 0.02f));
	m_AnimationModel->setYoffset();
	m_AnimationModel->setTargetToHipsKeyFrame();

	// 구 모델 생성
	Model* sphere1 = Model::createSphere(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), XMFLOAT4(0.525f, 0.502f, 0.329f, 1.0f));
	sphere1->createStaticSphere(m_PhysicsManager->m_Physics, m_PhysicsManager->m_Scene);
	sphere1->setPosition(XMFLOAT3(-10.0f, -498.0f, 30.0f));
	sphere1->setScale(XMFLOAT3(1000.0f, 1000.0f, 1000.0f));
	sphere1->syncModelWithRigidbody(m_PhysicsManager->m_Physics);
	m_Models.push_back(sphere1);

	Model* sphere2 = Model::createSphere(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), XMFLOAT4(0.525f, 0.502f, 0.329f, 1.0f));
	sphere2->createStaticSphere(m_PhysicsManager->m_Physics, m_PhysicsManager->m_Scene);
	sphere2->setPosition(XMFLOAT3(10.0f, -1.5f, 10.0f));
	sphere2->setScale(XMFLOAT3(5.0f, 5.0f, 5.0f));
	sphere2->syncModelWithRigidbody(m_PhysicsManager->m_Physics);
	m_Models.push_back(sphere2);

	// 박스 모델 생성
	Model* box1 = Model::createBox(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), XMFLOAT4(0.725f, 0.502f, 0.329f, 1.0f));
	box1->createStaticBox(m_PhysicsManager->m_Physics, m_PhysicsManager->m_Scene);
	box1->setPosition(XMFLOAT3(10.0f, -0.5f, 10.0f));
	box1->setScale(XMFLOAT3(1000.0f, 1.0f, 1000.0f));
	box1->syncModelWithRigidbody(m_PhysicsManager->m_Physics);
	m_Models.push_back(box1);


	//// 박스 모델 생성
	Model* box2 = Model::createBox(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f));
	box2->createStaticBox(m_PhysicsManager->m_Physics, m_PhysicsManager->m_Scene);
	//box2->setPosition(XMFLOAT3(0.0f, -40.5f, 5.0f));
	box2->setPosition(XMFLOAT3(0.0f, -40.5f, 50.0f));
	box2->setScale(XMFLOAT3(100.0f, 100.0f, 100.0f));
	box2->setRotation(XMFLOAT3(-20.0f, 0.0f, 0.0f));
	box2->syncModelWithRigidbody(m_PhysicsManager->m_Physics);
	m_Models.push_back(box2);

	//// 계단 생성
	for (int i = 0; i < 20; i++)
	{
		Model* stair = Model::createBox(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), XMFLOAT4(0.929f, 0.639f, 0.161f, 1.0f));
		stair->createStaticBox(m_PhysicsManager->m_Physics, m_PhysicsManager->m_Scene);
		stair->setPosition(XMFLOAT3(6.0f + 2.0f * i, 0.0f, 0.0f));
		stair->setScale(XMFLOAT3(2.0f, 2.0f + 2.0f * i, 10.0f));
		stair->setRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
		stair->syncModelWithRigidbody(m_PhysicsManager->m_Physics);
		m_Models.push_back(stair);
	}

	// 모델 개수 저장
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

	m_ModelShader = new ModelShader;
	result = m_ModelShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model shader object.", L"Error", MB_OK);
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

	if (m_ModelShader)
	{
		m_ModelShader->Shutdown();
		delete m_ModelShader;
		m_ModelShader = 0;
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

	if (m_PhysicsManager)
	{
		m_PhysicsManager->shutdown();
		delete m_PhysicsManager;
	}

	return;
}

bool Application::Frame()
{
	float dt = m_Timer.GetDeltaTime();

	// animation
	if (dt > 0.0f)
	{
		m_PhysicsManager->stepSimulation(dt);
		m_AnimationModel->UpdateAnimation(m_PhysicsManager->m_Scene, dt);
	}

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
	m_Direct3D->BeginScene(0.812f, 0.906f, 0.984f, 1.0f);

	// view Matrix 업데이트
	m_Camera->Render();

	// MVP 변환 준비
	m_Camera->GetViewMatrix(matrix.view);
	m_Direct3D->GetProjectionMatrix(matrix.projection);

	if (m_boneDebugMode == true)
	{
		m_AnimationModel->DrawJointShader(m_Direct3D->GetDeviceContext(), m_JointShader, matrix);
		m_AnimationModel->DrawBoneShader(m_Direct3D->GetDeviceContext(), m_BoneShader, matrix, m_Camera->GetFront());
	}
	else
	{
		m_AnimationModel->DrawTextureShader(m_Direct3D->GetDeviceContext(), m_TextureShader, matrix);
	}

	if (m_rayDebugMode == true)
	{
		m_AnimationModel->DrawRayLineShader(m_Direct3D->GetDeviceContext(), m_BoneShader, matrix, m_Camera->GetFront());
		m_AnimationModel->DrawRayPointShader(m_Direct3D->GetDeviceContext(), m_JointShader, matrix);
	}

	if (m_rangeDebugMode == true)
	{
		m_AnimationModel->DrawRangeAxisShader(m_Direct3D->GetDeviceContext(), m_BoneShader, matrix, m_Camera->GetFront());
		m_AnimationModel->DrawRangeCornShader(m_Direct3D->GetDeviceContext(), m_JointShader, matrix);
	}

	for (int i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i]->DrawModelShader(m_Direct3D->GetDeviceContext(), m_ModelShader, matrix) == false)
		{
			return false;
		}
	}

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
	const float walkSpeed = 0.05f;
	int inputState = 0;

	bool pressUp = input->IsKeyDown(VK_UP);
	bool pressLeft = input->IsKeyDown(VK_LEFT);
	bool pressDown = input->IsKeyDown(VK_DOWN);
	bool pressRight = input->IsKeyDown(VK_RIGHT);
	XMFLOAT3 dir = getDirection(inputState, pressUp, pressLeft, pressDown, pressRight);

	if (inputState){
		m_AnimationModel->move(dir);
		m_AnimationModel->setState("walk");
	}
	else {
		if (m_AnimationModel->getSpeed() < walkSpeed)
			m_AnimationModel->setState("idle");
		m_AnimationModel->speedDown();
	}
	
}

void Application::setBoneDebugMode(bool flag)
{
	m_boneDebugMode = flag;
}

void Application::setRayDebugMode(bool flag)
{
	m_rayDebugMode = flag;
}

void Application::setRangeDebugMode(bool flag)
{
	m_rangeDebugMode = flag;
}
