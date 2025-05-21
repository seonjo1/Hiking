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

void Application::createStairs(int num)
{
	// ��� ����
	float stairHeight = 0.7f;
	float stairXWidth = 2.0f;
	float stairZWidth = 10.0f;
	XMFLOAT4 colors[3] = { XMFLOAT4(0.229f, 0.239f, 0.461f, 1.0f), XMFLOAT4(0.429f, 0.639f, 0.261f, 1.0f) , XMFLOAT4(0.929f, 0.639f, 0.261f, 1.0f) };

	for (int i = 0; i < 20; i++)
	{
		XMFLOAT4 color = colors[(i % 3)];
		Model* stair = Model::createBox(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), color);
		stair->createStaticBox(m_PhysicsManager->m_Physics, m_PhysicsManager->m_Scene);
		stair->setPosition(XMFLOAT3(8.0f + stairXWidth * i, 0.5f * (stairHeight + stairHeight * i), 0.0f));
		stair->setScale(XMFLOAT3(stairXWidth, stairHeight + stairHeight * i, stairZWidth));
		stair->setRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
		stair->syncModelWithRigidbody(m_PhysicsManager->m_Physics);
		m_Models.push_back(stair);
	}
}

void Application::createRandomTerrain(int num)
{
	// Ÿ�� �⺻ ũ�� (XZ ũ�� ����, Y�� ���� ����)
	const float tileWidth = 2.0f;
	const float tileDepth = 10.0f;
	const float minHeight = 0.1f;
	const float maxHeight = 2.2f;
	// ȸ�� �ִ� ���� (�� ����)
	const float maxAngleDeg = 25.0f;

	// ���� �迭 (��� �������� ������)
	XMFLOAT4 colors[3] = {
		{ 0.229f, 0.239f, 0.461f, 1.0f },
		{ 0.429f, 0.639f, 0.261f, 1.0f },
		{ 0.929f, 0.639f, 0.261f, 1.0f }
	};

	// ���� ������ ����
	std::mt19937                    rng(std::random_device{}());
	std::uniform_real_distribution<float> angleDist(-maxAngleDeg, maxAngleDeg);
	std::uniform_real_distribution<float> heightDist(minHeight, maxHeight);

	float startX = -3.0f;  // ���� X ��ǥ
	float posZ = 0.0f;  // Z ��ǥ (1���� ���� ���)

	for (int i = 0; i < num; ++i)
	{
		// 1) ���� ���̿� �÷�
		float h = heightDist(rng);
		XMFLOAT4 color = colors[i % 3];

		// 2) �� ���� �� PhysX �ݶ��̴� ����
		Model* tile = Model::createBox(m_Direct3D->GetDevice(),
			m_Direct3D->GetDeviceContext(),
			color);
		tile->createStaticBox(m_PhysicsManager->m_Physics,
			m_PhysicsManager->m_Scene);

		// 3) ������ ���� (Y�� ���� ����)
		tile->setScale(XMFLOAT3(tileWidth, h, tileDepth));

		// 4) ���� ȸ�� (X�� ��ġ, Z�� ��)
		float pitch = XMConvertToRadians(angleDist(rng));
		float roll = XMConvertToRadians(angleDist(rng));
		tile->setRotation(XMFLOAT3(pitch, 0.0f, roll));

		// 5) ������ ����
		//    Y�� Ÿ�� ������ ����(h/2)�� ���Ƽ� �ٴڿ� �򸮵���
		float posX = startX - tileWidth * i;
		tile->setPosition(XMFLOAT3(posX, h * 0.5f, posZ));

		// 6) PhysX�� ����ȭ �� ����Ʈ�� �߰�
		tile->syncModelWithRigidbody(m_PhysicsManager->m_Physics);
		m_Models.push_back(tile);
	}
}

void Application::createSlopeTerrain(int num)
{
	// Ÿ�� �⺻ ũ�� (XZ ũ�� ����, Y�� ���� ����)
	const float tileWidth = 2.0f;
	const float tileDepth = 10.0f;
	const float minHeight = 0.1f;
	const float maxHeight = 2.2f;
	// ȸ�� �ִ� ���� (�� ����)
	const float maxAngleDeg = 25.0f;

	// ���� �迭 (��� �������� ������)
	XMFLOAT4 colors[3] = {
		{ 0.229f, 0.239f, 0.461f, 1.0f },
		{ 0.429f, 0.639f, 0.261f, 1.0f },
		{ 0.929f, 0.639f, 0.261f, 1.0f }
	};

	// ���� ������ ����
	std::mt19937                    rng(std::random_device{}());
	std::uniform_real_distribution<float> angleDist(-maxAngleDeg, maxAngleDeg);
	std::uniform_real_distribution<float> heightDist(minHeight, maxHeight);

	float startX = -3.0f;  // ���� X ��ǥ
	float posZ = -10.0f;  // Z ��ǥ (1���� ���� ���)

	for (int i = 0; i < num; ++i)
	{
		// 1) ���� ���̿� �÷�
		float h = heightDist(rng);
		XMFLOAT4 color = colors[i % 3];

		// 2) �� ���� �� PhysX �ݶ��̴� ����
		Model* tile = Model::createBox(m_Direct3D->GetDevice(),
			m_Direct3D->GetDeviceContext(),
			color);
		tile->createStaticBox(m_PhysicsManager->m_Physics,
			m_PhysicsManager->m_Scene);

		// 3) ������ ���� (Y�� ���� ����)
		tile->setScale(XMFLOAT3(tileWidth, h, tileDepth));

		// 4) ���� ȸ�� (X�� ��ġ, Z�� ��)
		float pitch = 0.0f;
		float roll = angleDist(rng);
		tile->setRotation(XMFLOAT3(pitch, 0.0f, roll));

		// 5) ������ ����
		//    Y�� Ÿ�� ������ ����(h/2)�� ���Ƽ� �ٴڿ� �򸮵���
		float posX = startX - tileWidth * i;
		tile->setPosition(XMFLOAT3(posX + 0.5f, h * 0.5f, posZ));

		// 6) PhysX�� ����ȭ �� ����Ʈ�� �߰�
		tile->syncModelWithRigidbody(m_PhysicsManager->m_Physics);
		m_Models.push_back(tile);
	}
}

void Application::createGround()
{
	// ���� ����
	Model* ground = Model::createBox(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), XMFLOAT4(0.725f, 0.502f, 0.329f, 1.0f));
	ground->createStaticBox(m_PhysicsManager->m_Physics, m_PhysicsManager->m_Scene);
	ground->setPosition(XMFLOAT3(10.0f, -0.5f, 10.0f));
	ground->setScale(XMFLOAT3(1000.0f, 1.0f, 1000.0f));
	ground->syncModelWithRigidbody(m_PhysicsManager->m_Physics);
	m_Models.push_back(ground);
}


void Application::createRock()
{
	// �� �� ����
	Model* rock = Model::createSphere(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), XMFLOAT4(0.725f, 0.202f, 0.529f, 1.0f));
	rock->createDynamicSphere(m_PhysicsManager->m_Physics, m_PhysicsManager->m_Scene, 100.0f);
	rock->setPosition(XMFLOAT3(0.0f, 60.0f, 60.0f));
	rock->setScale(XMFLOAT3(7.0f, 7.0f, 7.0f));
	rock->syncModelWithRigidbody(m_PhysicsManager->m_Physics);
	m_Models.push_back(rock);
}

void Application::createSphere(XMFLOAT3 pos, XMFLOAT4 color)
{
	Model* sphere = Model::createSphere(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), color);
	sphere->createStaticSphere(m_PhysicsManager->m_Physics, m_PhysicsManager->m_Scene);
	sphere->setPosition(pos);
	sphere->setScale(XMFLOAT3(5.0f, 5.0f, 5.0f));
	sphere->syncModelWithRigidbody(m_PhysicsManager->m_Physics);
	m_Models.push_back(sphere);
}

void Application::createSpheres()
{
	XMFLOAT3 pos1 = { 10.0f, -1.5f, -10.0f };
	XMFLOAT3 pos2 = { 11.75f, -1.5f, -13.5f };

	XMFLOAT4 colors[3] = {
	{ 0.229f, 0.239f, 0.461f, 1.0f },
	{ 0.429f, 0.639f, 0.261f, 1.0f },
	{ 0.929f, 0.639f, 0.261f, 1.0f }
	};

	for (int i = 0; i < 10; i++)
	{
		XMFLOAT4 color1 = colors[(i % 3)];
		XMFLOAT4 color2 = colors[((i + 1) % 3)];
		createSphere(pos1, color1);
		createSphere(pos2, color2);
		pos1.x += 3.5f;
		pos2.x += 3.5f;
	}
}

void Application::createSlope()
{
	Model* slope = Model::createBox(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f));
	slope->createStaticBox(m_PhysicsManager->m_Physics, m_PhysicsManager->m_Scene);
	slope->setPosition(XMFLOAT3(0.0f, -40.5f, 50.0f));
	slope->setScale(XMFLOAT3(100.0f, 100.0f, 100.0f));
	slope->setRotation(XMFLOAT3(-20.0f, 0.0f, 0.0f));
	slope->syncModelWithRigidbody(m_PhysicsManager->m_Physics);
	m_Models.push_back(slope);
}

bool Application::initShaders(HWND hwnd)
{
	// ���̴� ��ü ���� �� �ʱ�ȭ
	m_TextureShader = new TextureShader;
	bool result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
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
	m_Camera->SetPosition(0.0f, 5.0f, -30.0f);

	// ���� ���� �ʱ�ȭ
	m_PhysicsManager = new PhysicsManager();
	m_PhysicsManager->initialize();

	// �ִϸ��̼� �� ����
	std::string filename("./Assets/Remy3.glb");

	m_AnimationModel = new Model(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), filename);
	if (!m_AnimationModel)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}
	m_AnimationModel->setPosition(XMFLOAT3(0.0f, -0.15f, 0.0f));
	m_AnimationModel->setRotation(XMFLOAT3(-90.0f, 0.0f, 0.0f));
	m_AnimationModel->setScale(XMFLOAT3(0.02f, 0.02f, 0.02f));
	m_AnimationModel->setYoffset();
	m_AnimationModel->setTargetToHipsKeyFrame();

	// �� ����
	createGround();
	//createRock();
	createSlope();
	createSpheres();
	createStairs(20);
	//createRandomTerrain(30);
	createSlopeTerrain(30);

	// �� ���� ����
	m_modelCount = m_Models.size();

	// ���̴� �ʱ�ȭ
	bool success = initShaders(hwnd);

	return success;
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

void Application::updateDynamicRigidbody()
{
	for (auto* model : m_Models)
	{
		model->updatePhysxResult();
	}
}

bool Application::Frame()
{
	float dt = m_Timer.GetDeltaTime();

	// animation
	if (dt > 0.0f)
	{
		m_PhysicsManager->stepSimulation(dt);
		updateDynamicRigidbody();
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

	// �� ���� Ŭ����
	m_Direct3D->BeginScene(0.812f, 0.906f, 0.984f, 1.0f);

	// view Matrix ������Ʈ
	m_Camera->Render();

	// MVP ��ȯ �غ�
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
