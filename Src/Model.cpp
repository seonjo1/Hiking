#include "model.h"
#include "JointShader.h"
#include "BoneShader.h"
#include "ModelShader.h"

Model* Model::createSphere(ID3D11Device* device, ID3D11DeviceContext* deviceContext,  XMFLOAT4 color)
{
	Model* model = new Model();
	Mesh* mesh = Mesh::createSphere(device);
	model->addMesh(mesh);
	Texture* texture = new Texture(device, deviceContext, color);
	model->m_meshes[0]->setTexture(texture);
	model->addTexture(texture);
	return model;
}

Model* Model::createBox(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMFLOAT4 color)
{
	Model* model = new Model();
	Mesh* mesh = Mesh::createBox(device);
	model->addMesh(mesh);
	Texture* texture = new Texture(device, deviceContext, color);
	model->m_meshes[0]->setTexture(texture);
	model->addTexture(texture);
	return model;
}

Model::Model()
{
	m_position = { 0.0f, 0.0f, 0.0f };
	m_rotation = { 0.0f, 0.0f, 0.0f };
	m_scale = { 1.0f, 1.0f, 1.0f };
	m_size = 0;
	m_boneMesh = 0;
	m_stepMesh = 0;
	m_jointMesh = 0;
	m_rayToTargetMesh = 0;
	m_rayNormalMesh = 0;
	m_rangeAxisMesh = 0;
	m_cornMesh = 0;
	m_physicsObject = nullptr;
}

Model::Model(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::string filename)
{
	m_position = { 0.0f, 0.0f, 0.0f };
	m_rotation = { 0.0f, 0.0f, 0.0f };
	m_scale = { 1.0f, 1.0f, 1.0f };
	m_physicsObject = nullptr;
	LoadByAssimp(device, deviceContext, filename);
}

Model::Model(const Model& other)
{
}

Model::~Model()
{
}

void Model::LoadByAssimp(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::string filename)
{
	Assimp::Importer importer;
	// scene 구조체 받아오기
	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate |
		aiProcess_MakeLeftHanded |
		aiProcess_FlipUVs |
		aiProcess_FlipWindingOrder |
		aiProcess_JoinIdenticalVertices |
		aiProcess_LimitBoneWeights);
	std::string dirname = filename.substr(0, filename.find_last_of("/"));

	// scene load 오류 처리
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		throw std::runtime_error("failed to load obj file!");
	}
	// scene 안에있는 material 개수만큼 반복
	for (uint32_t i = 0; i < scene->mNumMaterials; i++)
	{
		// scene의 i번째 material 정보 get
		aiMaterial* materialInfo = scene->mMaterials[i];
		Texture* texPtr = new Texture(device, deviceContext, scene, materialInfo, dirname);
		m_textures.push_back(texPtr);
	}

	// animation 로딩
	if (HasAnimationInfo(scene) == true) {
		m_hasAnimation = true;
		LoadAnimationData(scene, m_skeleton);
		m_pose.Initialize(m_skeleton.bones.size());
		m_animStateManager.SetState("idle", m_animationClips);
		m_jointMesh = Mesh::createDebugSphere(device, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), 3.0f);
		m_stepMesh = Mesh::createDebugSphere(device, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), 3.0f);
		m_boneMesh = Mesh::createDebugLine(device, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
		m_rayToTargetMesh = Mesh::createDebugLine(device, XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
		m_rayNormalMesh = Mesh::createDebugLine(device, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
		m_rangeAxisMesh = Mesh::createDebugLine(device, XMFLOAT4(0.976f, 0.357f, 0.749f, 1.0f));
		m_cornMesh = Mesh::createCone(device);
		initRangeAxis();
		m_IKManager.initIKChains(m_skeleton);
		m_animStateManager.initAnimationPlayer(m_skeleton.bones.size());
	}

	// node 데이터 처리
	processNode(device, deviceContext, scene->mRootNode, scene);

	m_size = m_meshes.size();
}

void Model::processNode(ID3D11Device* device, ID3D11DeviceContext* deviceContext, aiNode* node, const aiScene* scene)
{
	// node에 포함된 mesh들 순회
	for (uint32_t i = 0; i < node->mNumMeshes; i++)
	{
		// 현재 처리할 mesh 찾기
		uint32_t meshIndex = node->mMeshes[i];
		aiMesh* mesh = scene->mMeshes[meshIndex];
		// 현재 mesh 데이터 처리
		processMesh(device, deviceContext, mesh, scene);
	}

	// 자식 노드 처리
	for (uint32_t i = 0; i < node->mNumChildren; i++)
		processNode(device, deviceContext, node->mChildren[i], scene);
}

void Model::processMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, aiMesh* mesh, const aiScene* scene)
{
	std::vector<VertexBoneData> vertexBones(mesh->mNumVertices);
	// 본 → 정점 매핑
	for (unsigned int i = 0; i < mesh->mNumBones; ++i) {
		aiBone* aiBone = mesh->mBones[i];
		std::string boneName = aiBone->mName.C_Str();

		int boneIndex = m_skeleton.GetBoneIndex(boneName);
		if (boneIndex == -1) continue;

		for (unsigned int j = 0; j < aiBone->mNumWeights; ++j) {
			uint32_t vertexId = aiBone->mWeights[j].mVertexId;
			float weight = aiBone->mWeights[j].mWeight;
			vertexBones[vertexId].Add(boneIndex, weight);
		}
	}

	std::vector<VertexType> vertices;
	vertices.resize(mesh->mNumVertices);
	for (uint32_t i = 0; i < mesh->mNumVertices; i++)
	{
		VertexType& v = vertices[i];
		v.position = XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		v.texture = XMFLOAT2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);

		vertexBones[i].NormalizeAndTrim();

		switch (vertexBones[i].influences.size())
		{
		case 4:
			v.boneIndices.w = vertexBones[i].influences[3].first;
			v.boneWeights.w = vertexBones[i].influences[3].second;
			[[fallthrough]];
		case 3:
			v.boneIndices.z = vertexBones[i].influences[2].first;
			v.boneWeights.z = vertexBones[i].influences[2].second;
			[[fallthrough]];
		case 2:
			v.boneIndices.y = vertexBones[i].influences[1].first;
			v.boneWeights.y = vertexBones[i].influences[1].second;
			[[fallthrough]];
		case 1:
			v.boneIndices.x = vertexBones[i].influences[0].first;
			v.boneWeights.x = vertexBones[i].influences[0].second;
		}
	}

	std::vector<uint32_t> indices;
	indices.resize(mesh->mNumFaces * 3);
	// face의 개수 = triangle 개수
	for (uint32_t i = 0; i < mesh->mNumFaces; i++)
	{
		indices[3 * i] = mesh->mFaces[i].mIndices[0];
		indices[3 * i + 1] = mesh->mFaces[i].mIndices[1];
		indices[3 * i + 2] = mesh->mFaces[i].mIndices[2];
	}

	Mesh* newMesh = new Mesh(device, vertices, indices);
	// mesh의 mMaterialINdex가 0이상이면 이 mesh는 material을 갖고 있으므로
	// 해당 material값을 setting 해준다.
	if (mesh->mMaterialIndex >= 0)
	{
		newMesh->setTexture(m_textures[mesh->mMaterialIndex]);
	}
	m_meshes.push_back(newMesh);
}

void Model::Shutdown()
{
	// Release the model texture.
	ReleaseMeshes();
	ReleaseTextures();
	
	if (m_physicsObject)
	{
		m_physicsObject->shutdown();
		delete m_physicsObject;
	}

	return;
}

bool Model::DrawBoneShader(ID3D11DeviceContext* deviceContext, BoneShader* boneShader, Matrix& matrix, XMFLOAT3 cameraFront)
{
	matrix.world = getWorldMatrix();

	m_boneMesh->Render(deviceContext);

	std::queue<int> q;
	q.push(m_skeleton.rootBoneIdx);
	
	while (!q.empty())
	{
		int idx = q.front();
		q.pop();

		Bone& bone = m_skeleton.bones[idx];

		int count = bone.children.size();
		for (int i = 0; i < count; i++)
		{
			int childIdx = bone.children[i];
			if (boneShader->Render(deviceContext, m_boneMesh->GetIndexCount(), matrix, m_pose.world[idx], m_pose.world[childIdx], cameraFront) == false)
				return false;
			q.push(childIdx);
		}
	}

	return true;
}

bool Model::DrawRayLineShader(ID3D11DeviceContext* deviceContext, BoneShader* boneShader, Matrix& matrix, XMFLOAT3 cameraFront)
{
	matrix.world = getWorldMatrix();

	// left foot
	int idx = -1;
	if (m_RaycastingManager.m_LeftFoot.part == EIKPart::TOEBASE)
	{
		idx = m_skeleton.GetBoneIndex("mixamorig:LeftToeBase");
	}
	else if (m_RaycastingManager.m_LeftFoot.part == EIKPart::TOEEND)
	{
		idx = m_skeleton.GetBoneIndex("mixamorig:LeftToe_End");
	}

	if (idx != -1)
	{
		// bone to target
		m_rayToTargetMesh->Render(deviceContext);
		if (boneShader->RenderRayLine(deviceContext, m_boneMesh->GetIndexCount(), matrix, m_pose.world[idx], m_RaycastingManager.m_LeftFoot, cameraFront) == false)
			return false;

		// target normal
		m_rayNormalMesh->Render(deviceContext);
		m_RaycastingManager.m_LeftFoot.dir = m_RaycastingManager.m_LeftFoot.normal;
		m_RaycastingManager.m_LeftFoot.distance = 0.5f;
		if (boneShader->RenderRayLine(deviceContext, m_boneMesh->GetIndexCount(), matrix, m_pose.world[idx], m_RaycastingManager.m_LeftFoot, cameraFront) == false)
			return false;
	}



	// right foot
	idx = -1;
	if (m_RaycastingManager.m_RightFoot.part == EIKPart::TOEBASE)
	{
		idx = m_skeleton.GetBoneIndex("mixamorig:RightToeBase");
	}
	else if (m_RaycastingManager.m_RightFoot.part == EIKPart::TOEEND)
	{
		idx = m_skeleton.GetBoneIndex("mixamorig:RightToe_End");
	}

	if (idx != -1)
	{
		// bone to target
		m_rayToTargetMesh->Render(deviceContext);
		if (boneShader->RenderRayLine(deviceContext, m_boneMesh->GetIndexCount(), matrix, m_pose.world[idx], m_RaycastingManager.m_RightFoot, cameraFront) == false)
			return false;

		// target normal
		m_rayNormalMesh->Render(deviceContext);
		m_RaycastingManager.m_RightFoot.dir = m_RaycastingManager.m_RightFoot.normal;
		m_RaycastingManager.m_RightFoot.distance = 0.5f;
		if (boneShader->RenderRayLine(deviceContext, m_boneMesh->GetIndexCount(), matrix, m_pose.world[idx], m_RaycastingManager.m_RightFoot, cameraFront) == false)
			return false;
	}

	return true;
}

XMFLOAT3 Model::getAxis(float xDeg, float yDeg, float zDeg)
{
	return XMFLOAT3(xDeg, yDeg, zDeg);
}

void Model::initRangeAxis()
{
	m_skeleton.SetBoneAxisAndRange("mixamorig:LeftToeBase", getAxis(0.0f, 1.0f, 0.0f), -40.0f, -40.5f, 0.5f, -0.5f, 0.0f);
	m_skeleton.SetBoneAxisAndRange("mixamorig:LeftFoot", getAxis(0.0f, 1.0f, 0.0f), -60.0f, -60.5f, 0.5f, -0.5f, 0.0f);
	m_skeleton.SetBoneAxisAndRange("mixamorig:LeftLeg", getAxis(0.0f, 1.0f, 0.0f), 150.0f, 0.0f, 20.0f, -20.0f, 0.0f);
	m_skeleton.SetBoneAxisAndRange("mixamorig:LeftUpLeg", getAxis(0.0f, 1.0f, 0.0f), 240.0f, 90.0f, 10.0f, -0.0f, 180.0f);

	m_skeleton.SetBoneAxisAndRange("mixamorig:RightToeBase", getAxis(0.0f, 1.0f, 0.0f), -40.0f, -40.5f, 0.5f, -0.5f, 0.0f);
	m_skeleton.SetBoneAxisAndRange("mixamorig:RightFoot", getAxis(0.0f, 1.0f, 0.0f), -60.0f, -60.5f, 0.5f, -0.5f, 0.0f);
	m_skeleton.SetBoneAxisAndRange("mixamorig:RightLeg", getAxis(0.0f, 1.0f, 0.0f), 150.0f, 0.0f, 20.0f, -20.0f, 0.0f);
	m_skeleton.SetBoneAxisAndRange("mixamorig:RightUpLeg", getAxis(0.0f, 1.0f, 0.0f), 240.0f, 90.0f, 0.0f, -10.0f, 180.0f);
}

bool Model::DrawRangeAxisShader(ID3D11DeviceContext* deviceContext, BoneShader* boneShader, Matrix& matrix, XMFLOAT3 cameraFront)
{
	matrix.world = getWorldMatrix();

	int count = m_skeleton.bones.size();
	
	m_rangeAxisMesh->Render(deviceContext);

	for (int i = 0; i < count; ++i)
	{
		Bone& bone = m_skeleton.bones[i];

		if (bone.hasAxis == true)
		{
			XMMATRIX local = m_pose.getLocalTranslationMatrix(i);
			XMMATRIX parent = m_pose.world[bone.parentIndex];
			XMMATRIX boneMatrix = XMMatrixMultiply(local, parent);

			//if (boneShader->RenderRangeAxis(deviceContext, m_rangeAxisMesh->GetIndexCount(), matrix, boneMatrix, XMFLOAT3(1.0f, 0.0f, 0.0f), cameraFront) == false)
			//	return false;
			if (boneShader->RenderRangeAxis(deviceContext, m_rangeAxisMesh->GetIndexCount(), matrix, boneMatrix, XMFLOAT3(0.0f, 1.0f, 0.0f), cameraFront) == false)
				return false;
			//if (boneShader->RenderRangeAxis(deviceContext, m_rangeAxisMesh->GetIndexCount(), matrix, boneMatrix, XMFLOAT3(0.0f, 0.0f, 1.0f), cameraFront) == false)
			//	return false;
		}
	}

	return true;
}

bool Model::DrawModelShader(ID3D11DeviceContext* deviceContext, ModelShader* modelShader, Matrix& matrix)
{
	matrix.world = getWorldMatrix();

	for (int i = 0; i < m_size; i++)
	{
		m_meshes[i]->Render(deviceContext);
		if (modelShader->Render(deviceContext, m_meshes[i]->GetIndexCount(), matrix, m_meshes[i]->getTexture()) == false)
			return false;
	}

	return true;
}

bool Model::DrawJointShader(ID3D11DeviceContext* deviceContext, JointShader* jointShader, Matrix& matrix)
{
	matrix.world = getWorldMatrix();

	int count = m_skeleton.bones.size();

	m_jointMesh->Render(deviceContext);

	int footIdx = m_skeleton.GetBoneIndex("mixamorig:LeftToeBase");
	int toeIdx = m_skeleton.GetBoneIndex("mixamorig:LeftToe_End");

	for (int i = 0; i < count; i++)
	{
		if (i == toeIdx)
		{
			XMMATRIX ma = XMMatrixMultiply(m_pose.world[i], matrix.world);
			XMVECTOR v = XMVector3TransformCoord(XMVectorZero(), ma);
			XMFLOAT3 pos;
			XMStoreFloat3(&pos, v);
		}

		if (i == footIdx)
		{
			XMMATRIX ma = XMMatrixMultiply(m_pose.world[i], matrix.world);
			XMVECTOR v = XMVector3TransformCoord(XMVectorZero(), ma);
			XMFLOAT3 pos;
			XMStoreFloat3(&pos, v);
		}

		if (jointShader->Render(deviceContext, m_jointMesh->GetIndexCount(), matrix, m_pose.world[i]) == false)
			return false;
	}

	return true;
}

bool Model::DrawRangeCornShader(ID3D11DeviceContext* deviceContext, JointShader* jointShader, Matrix& matrix)
{
	matrix.world = getWorldMatrix();

	int count = m_skeleton.bones.size();

	for (int i = 0; i < count; ++i)
	{
		Bone& bone = m_skeleton.bones[i];
		if (bone.hasAxis == true)
		{
			XMVECTOR twist = m_pose.getTwist(i);
			m_cornMesh->UpdateMeshVertices(deviceContext, twist, bone.xMax, bone.xMin, bone.zMax, bone.zMin);
			m_cornMesh->Render(deviceContext);

			XMMATRIX local = m_pose.getLocalTranslationMatrix(i);
			if (jointShader->Render(deviceContext, m_jointMesh->GetIndexCount(), matrix, XMMatrixMultiply(local, m_pose.world[bone.parentIndex])) == false)
				return false;
		}
	}

	return true;
}

bool Model::DrawRayPointShader(ID3D11DeviceContext* deviceContext, JointShader* jointShader, Matrix& matrix)
{	
	XMMATRIX scale = XMMatrixScaling(0.02f, 0.02f, 0.02f);
	XMMATRIX translation = XMMatrixTranslation(m_RaycastingManager.m_LeftFoot.target.x, m_RaycastingManager.m_LeftFoot.target.y, m_RaycastingManager.m_LeftFoot.target.z);
	matrix.world = scale * translation;
	
	m_jointMesh->Render(deviceContext);

	if (jointShader->Render(deviceContext, m_jointMesh->GetIndexCount(), matrix, XMMatrixIdentity()) == false)
		return false;


	translation = XMMatrixTranslation(m_RaycastingManager.m_RightFoot.target.x, m_RaycastingManager.m_RightFoot.target.y, m_RaycastingManager.m_RightFoot.target.z);
	matrix.world = scale * translation;

	if (jointShader->Render(deviceContext, m_jointMesh->GetIndexCount(), matrix, XMMatrixIdentity()) == false)
		return false;


	scale = XMMatrixScaling(0.03f, 0.03f, 0.03f);
	m_stepMesh->Render(deviceContext);
	if (m_RaycastingManager.m_MoveCheck.part != EIKPart::FAIL)
	{
		translation = XMMatrixTranslation(m_RaycastingManager.m_MoveCheck.target.x, m_RaycastingManager.m_MoveCheck.target.y, m_RaycastingManager.m_MoveCheck.target.z);
		matrix.world = scale * translation;

		if (jointShader->Render(deviceContext, m_stepMesh->GetIndexCount(), matrix, XMMatrixIdentity()) == false)
			return false;
	}

	if (m_RaycastingManager.m_NextStep.part != EIKPart::FAIL)
	{
		translation = XMMatrixTranslation(m_RaycastingManager.m_NextStep.target.x, m_RaycastingManager.m_NextStep.target.y, m_RaycastingManager.m_NextStep.target.z);
		matrix.world = scale * translation;
		if (jointShader->Render(deviceContext, m_stepMesh->GetIndexCount(), matrix, XMMatrixIdentity()) == false)
			return false;
	}

	return true;
}

bool Model::DrawTextureShader(ID3D11DeviceContext* deviceContext, TextureShader* textureShader, Matrix& matrix)
{
	matrix.world = getWorldMatrix();

	for (int i = 0; i < m_size; i++)
	{
		m_meshes[i]->Render(deviceContext);
		if (textureShader->Render(deviceContext, m_meshes[i]->GetIndexCount(), matrix, m_pose, m_meshes[i]->getTexture()) == false)
			return false;
	}

	return true;
}

void Model::setState(std::string state)
{
	if (m_hasAnimation == true)
	{
		bool result = m_animStateManager.SetState(state, m_animationClips);
		if (result == true)
		{
			m_prevStep = m_currentStep;
			m_currentStep.leftGo = false;
		}
	}
}

XMVECTOR Model::getTargetToHipsDest(XMFLOAT3 targetToHipsFloat, XMVECTOR& target)
{
	XMVECTOR targetToHips = XMLoadFloat3(&targetToHipsFloat);

	XMVECTOR q = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(m_rotation.y));
	targetToHips = XMVector3Rotate(targetToHips, q);

	XMVECTOR hipsDest = XMVectorAdd(target, targetToHips);
	XMVectorSetZ(hipsDest, 1.0f);
	return hipsDest;
}

void Model::modifyHipsPos(XMMATRIX& worldMatrix, physx::PxVec3& leftToeBase, physx::PxVec3& rightToeBase)
{
	static const float hipsSpeed = 0.5f;

	// hips 위치 변경
	physx::PxVec3 hips = m_pose.getBonePos(worldMatrix, m_skeleton.GetBoneIndex("mixamorig:Hips"));

	XMVECTOR hipsDest;
	XMVECTOR leftTarget = XMVectorSet(leftToeBase.x, leftToeBase.y, leftToeBase.z, 1.0f);
	XMVECTOR rightTarget = XMVectorSet(rightToeBase.x, rightToeBase.y, rightToeBase.z, 1.0f);

	// idle, walk를 기준으로 왼발, 오른발 선택
	if (m_animStateManager.currentState == "idle")
	{
		if (leftToeBase.y < rightToeBase.y)
		{
			hipsDest = getTargetToHipsDest(m_animStateManager.getLeftTargetToHips(), leftTarget);
		}
		else
		{
			hipsDest = getTargetToHipsDest(m_animStateManager.getRightTargetToHips(), rightTarget);
		}
	}
	else
	{
		if (m_animStateManager.walkPhase < 0.5f)
		{
			hipsDest = getTargetToHipsDest(m_animStateManager.getLeftTargetToHips(), leftTarget);
		}
		else
		{
			hipsDest = getTargetToHipsDest(m_animStateManager.getRightTargetToHips(), rightTarget);
		}
	}

	// 선택한 발 기준으로 hipsDest 계산

	XMVECTOR det;
	XMMATRIX inverseWorld = XMMatrixInverse(&det, worldMatrix);
	hipsDest = XMVector3TransformCoord(hipsDest, inverseWorld);
	XMStoreFloat3(&m_pose.local[m_skeleton.GetBoneIndex("mixamorig:Hips")].position, hipsDest);
}

void Model::modifyTarget(XMMATRIX& worldMatrix)
{
	const static float targetSpeed = 0.4f;

	// animation을 반영한 target 보정
	if (m_animStateManager.currentState == "walk")
	{
		physx::PxVec3 leftToeBase = m_pose.getBonePos(worldMatrix, m_skeleton.GetBoneIndex("mixamorig:LeftToeBase"));
		physx::PxVec3 rightToeBase = m_pose.getBonePos(worldMatrix, m_skeleton.GetBoneIndex("mixamorig:RightToeBase"));

		float leftToeBaseY = leftToeBase.y - m_position.y;
		float rightToeBaseY = rightToeBase.y - m_position.y;

		float leftToeBaseOffset = leftToeBaseY - m_animationClips["walk"].minLeftFootOffset;
		float rightToeBaseOffset = rightToeBaseY - m_animationClips["walk"].minRightFootOffset;

		XMFLOAT3 leftNormal = m_RaycastingManager.m_LeftFoot.normal;
		XMFLOAT3 rightNormal = m_RaycastingManager.m_RightFoot.normal;

		XMVECTOR leftOffset = XMVector3Normalize(XMLoadFloat3(&leftNormal));
		XMVECTOR rightOffset = XMVector3Normalize(XMLoadFloat3(&rightNormal));

		leftOffset = XMVectorScale(leftOffset, leftToeBaseOffset);
		rightOffset = XMVectorScale(rightOffset, rightToeBaseOffset);

		XMVECTOR leftTarget = XMLoadFloat3(&m_RaycastingManager.m_LeftFoot.target);
		XMVECTOR rightTarget = XMLoadFloat3(&m_RaycastingManager.m_RightFoot.target);
		leftTarget = XMVectorAdd(leftTarget, leftOffset);
		rightTarget = XMVectorAdd(rightTarget, rightOffset);

		XMVECTOR prevLeftTarget = XMLoadFloat3(&m_leftTarget);
		XMVECTOR prevRightTarget = XMLoadFloat3(&m_rightTarget);

		XMVECTOR toLeftTarget = XMVectorSubtract(leftTarget, prevLeftTarget);
		XMVECTOR toRightTarget = XMVectorSubtract(rightTarget, prevRightTarget);

		float toLeftTargetLength = XMVectorGetX(XMVector3Length(toLeftTarget));
		if (toLeftTargetLength > targetSpeed)
		{
			float speed = std::fmaxf(targetSpeed, toLeftTargetLength * 0.5f);
			toLeftTarget = XMVector3Normalize(toLeftTarget);
			toLeftTarget = XMVectorScale(toLeftTarget, speed);
			prevLeftTarget = XMVectorAdd(prevLeftTarget, toLeftTarget);
			XMStoreFloat3(&m_RaycastingManager.m_LeftFoot.target, prevLeftTarget);
		}
		else
		{
			XMStoreFloat3(&m_RaycastingManager.m_LeftFoot.target, leftTarget);
		}

		float toRightTargetLength = XMVectorGetX(XMVector3Length(toRightTarget));
		if (toRightTargetLength > targetSpeed)
		{
			float speed = std::fmaxf(targetSpeed, toRightTargetLength * 0.2f);
			toRightTarget = XMVector3Normalize(toRightTarget);
			toRightTarget = XMVectorScale(toRightTarget, speed);
			prevRightTarget = XMVectorAdd(prevRightTarget, toRightTarget);
			XMStoreFloat3(&m_RaycastingManager.m_RightFoot.target, prevRightTarget);
		}
		else
		{
			XMStoreFloat3(&m_RaycastingManager.m_RightFoot.target, rightTarget);
		}
	}
}

void Model::modifyWorldY(physx::PxScene* scene, XMMATRIX& worldMatrix)
{
	const static float ySpeed = 0.1f;
	const static float angleOffsetScale = 0.5f;

	XMFLOAT3 dir = getRotatedVector(m_rotation.y);

	// 가장 낮은 발에 몸 위치 맞추기
	float minY = fminf(m_RaycastingManager.m_LeftFoot.target.y, m_RaycastingManager.m_RightFoot.target.y) - 0.15f;

	// 오르막길이면 경사에 맞춰서 y값을 더 내림
	XMVECTOR yLeftNormal = XMLoadFloat3(&m_RaycastingManager.m_LeftFoot.normal);
	XMVECTOR yRightNormal = XMLoadFloat3(&m_RaycastingManager.m_RightFoot.normal);
	XMVECTOR yNormal = XMVectorScale(XMVectorAdd(yLeftNormal, yRightNormal), 0.5f);
	XMVECTOR dirVec = XMLoadFloat3(&dir);
	float dot = XMVectorGetX(XMVector3Dot(dirVec, yNormal));
	if (dot < 0.0f)
	{
		minY -= fabs(dot) * angleOffsetScale;
	}


	// 한 번에 이동하지 않고 조금씩 이동
	if (m_position.y > minY)
	{
		m_position.y = std::fmaxf(minY, m_position.y - ySpeed);
	}
	else
	{
		m_position.y = std::fminf(minY, m_position.y + ySpeed);
	}
	worldMatrix = getWorldMatrix();
}

void Model::setIdleStep(AnimationPlayer& player, XMMATRIX& worldMatrix, StepInfo& stepInfo, bool leftGo)
{
	if (leftGo == true)
	{
		// idle 왼발 set
		XMVECTOR step = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMMATRIX tx = player.pose.world[m_skeleton.GetBoneIndex("mixamorig:LeftToeBase")];
		tx = XMMatrixMultiply(tx, worldMatrix);
		step = XMVector3TransformCoord(step, tx);
		XMStoreFloat3(&stepInfo.nextStep, step);

		XMVECTOR stepEnd = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMMATRIX txEnd = player.pose.world[m_skeleton.GetBoneIndex("mixamorig:LeftToe_End")];
		txEnd = XMMatrixMultiply(txEnd, worldMatrix);
		stepEnd = XMVector3TransformCoord(stepEnd, txEnd);
		XMStoreFloat3(&stepInfo.nextStepEnd, stepEnd);
	}
	else
	{
		// idle 오른발 set
		XMVECTOR step = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMMATRIX tx = player.pose.world[m_skeleton.GetBoneIndex("mixamorig:RightToeBase")];
		tx = XMMatrixMultiply(tx, worldMatrix);
		step = XMVector3TransformCoord(step, tx);
		XMStoreFloat3(&stepInfo.nextStep, step);

		XMVECTOR stepEnd = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMMATRIX txEnd = player.pose.world[m_skeleton.GetBoneIndex("mixamorig:RightToe_End")];
		txEnd = XMMatrixMultiply(txEnd, worldMatrix);
		stepEnd = XMVector3TransformCoord(stepEnd, txEnd);
		XMStoreFloat3(&stepInfo.nextStepEnd, stepEnd);
	}
}

void Model::setNextStep(AnimationPlayer& player, XMMATRIX& worldMatrix, StepInfo& stepInfo, float walkPhase)
{
	if (player.clip->name == "idle")
	{
		return;
	}

	if (walkPhase < player.clip->leftPhase || player.clip->rightPhase <= walkPhase)
	{
		// left go
		if (stepInfo.leftGo == false)
		{
			// right -> left로 변경됐으므로 left 현재 위치를 last step으로 기록
			stepInfo.leftGo = true;
			physx::PxVec3 leftToeBase = m_pose.getBonePos(worldMatrix, m_skeleton.GetBoneIndex("mixamorig:LeftToeBase"));
			physx::PxVec3 leftToeEnd = m_pose.getBonePos(worldMatrix, m_skeleton.GetBoneIndex("mixamorig:LeftToe_End"));

			stepInfo.lastStep.x = leftToeBase.x;
			stepInfo.lastStep.y = leftToeBase.y;
			stepInfo.lastStep.z = leftToeBase.z;

			stepInfo.lastStepEnd.x = leftToeEnd.x;
			stepInfo.lastStepEnd.y = leftToeEnd.y;
			stepInfo.lastStepEnd.z = leftToeEnd.z;
		}

		// 진행 방향 구하기
		XMVECTOR dir = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
		XMVECTOR quat = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(m_rotation.y));
		dir = XMVector3Rotate(dir, quat);

		// 다음 Step까지의 움직임 구하기
		float dt = player.getDt(stepInfo.leftGo);
		float delta = dt * m_speed;
		dir = XMVectorScale(dir, delta);

		XMFLOAT3 posBuffer = m_position;
		XMVECTOR nextPos = XMLoadFloat3(&m_position);
		nextPos = XMVectorAdd(nextPos, dir);
		XMStoreFloat3(&m_position, nextPos);
		XMMATRIX nextWorldMatrix = getWorldMatrix();

		m_position = posBuffer;
		XMMATRIX nextLeftToeBaseTx = XMMatrixMultiply(player.clip->leftToeTx, nextWorldMatrix);
		XMMATRIX nextLeftToeEndTx = XMMatrixMultiply(player.clip->leftToeEndTx, nextWorldMatrix);

		XMVECTOR point = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR nextLeftToeBase = XMVector3TransformCoord(point, nextLeftToeBaseTx);
		XMVECTOR nextLeftToeEnd = XMVector3TransformCoord(point, nextLeftToeEndTx);

		XMStoreFloat3(&stepInfo.nextStep, nextLeftToeBase);
		XMStoreFloat3(&stepInfo.nextStepEnd, nextLeftToeEnd);

	}
	else
	{
		// right go
		if (stepInfo.leftGo == true)
		{
			// left -> right로 바뀌었으므로 left 위치를 이전 위치로 기록
			stepInfo.leftGo = false;

			physx::PxVec3 rightToeBase = m_pose.getBonePos(worldMatrix, m_skeleton.GetBoneIndex("mixamorig:RightToeBase"));
			physx::PxVec3 rightToeEnd = m_pose.getBonePos(worldMatrix, m_skeleton.GetBoneIndex("mixamorig:RightToe_End"));

			stepInfo.lastStep.x = rightToeBase.x;
			stepInfo.lastStep.y = rightToeBase.y;
			stepInfo.lastStep.z = rightToeBase.z;

			stepInfo.lastStepEnd.x = rightToeEnd.x;
			stepInfo.lastStepEnd.y = rightToeEnd.y;
			stepInfo.lastStepEnd.z = rightToeEnd.z;

		}
		
		// 진행 방향 구하기
		XMVECTOR dir = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
		XMVECTOR quat = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(m_rotation.y));
		dir = XMVector3Rotate(dir, quat);

		// 다음 Step까지의 움직임 구하기
		float dt = player.getDt(stepInfo.leftGo);
		float delta = dt * m_speed;
		dir = XMVectorScale(dir, delta);
		XMFLOAT3 posBuffer = m_position;
		XMVECTOR nextPos = XMLoadFloat3(&m_position);
		nextPos = XMVectorAdd(nextPos, dir);
		XMStoreFloat3(&m_position, nextPos);
		XMMATRIX nextWorldMatrix = getWorldMatrix();
		m_position = posBuffer;

		XMMATRIX nextRightToeBaseTx = XMMatrixMultiply(player.clip->rightToeTx, nextWorldMatrix);
		XMMATRIX nextRightToeEndTx = XMMatrixMultiply(player.clip->rightToeEndTx, nextWorldMatrix);

		XMVECTOR point = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR nextRightToeBase = XMVector3TransformCoord(point, nextRightToeBaseTx);
		XMVECTOR nextRightToeEnd = XMVector3TransformCoord(point, nextRightToeEndTx);

		XMStoreFloat3(&stepInfo.nextStep, nextRightToeBase);
		XMStoreFloat3(&stepInfo.nextStepEnd, nextRightToeEnd);
	}
}

void Model::UpdateNextStep(XMMATRIX& worldMatrix)
{
	if (m_animStateManager.previous.clip != nullptr)
	{
		// 현재 애니메이션 next step 계산
		setNextStep(m_animStateManager.current, worldMatrix, m_currentStep, m_animStateManager.walkPhase);

		if (m_animStateManager.blendAlpha < 1.0f)
		{
			if (m_animStateManager.previous.clip->name == "idle")
			{
				// 이전 애니메이션이 idle인 경우 현재 애니메이션의 발에 맞춰 next step 계산
				setIdleStep(m_animStateManager.previous, worldMatrix, m_prevStep, m_currentStep.leftGo);
			}
			else
			{
				// 이전 애니메이션 next step 계산
				setNextStep(m_animStateManager.previous, worldMatrix, m_prevStep, m_animStateManager.walkPhase);
			}

			if (m_animStateManager.current.clip->name == "idle")
			{
				// 현재 애니메이션이 idle인 경우 이전 애니메이션의 발에 맞춰 next step 계산
				setIdleStep(m_animStateManager.current, worldMatrix, m_currentStep, m_prevStep.leftGo);
			}
		}
	}
	else
	{
		setIdleStep(m_animStateManager.current, worldMatrix, m_currentStep, true);
	}
}

void Model::blendingNextStep()
{
	// blending Next Step
	if (m_animStateManager.previous.clip != nullptr)
	{
		XMVECTOR currNextStep = XMLoadFloat3(&m_currentStep.nextStep);
		XMVECTOR prevNextStep = XMLoadFloat3(&m_prevStep.nextStep);
		XMVECTOR currNextStepEnd = XMLoadFloat3(&m_currentStep.nextStepEnd);
		XMVECTOR prevNextStepEnd = XMLoadFloat3(&m_prevStep.nextStepEnd);

		currNextStep = XMVectorLerp(prevNextStep, currNextStep, m_animStateManager.blendAlpha);
		currNextStepEnd = XMVectorLerp(prevNextStepEnd, currNextStepEnd, m_animStateManager.blendAlpha);

		XMStoreFloat3(&m_currentStep.nextStep, currNextStep);
		XMStoreFloat3(&m_currentStep.nextStepEnd, currNextStepEnd);
	}
}

void Model::raycastingToForward(physx::PxScene* scene, XMMATRIX& worldMatrix)
{
	physx::PxVec3 hips = m_pose.getBonePos(worldMatrix, m_skeleton.GetBoneIndex("mixamorig:Hips"));

	XMVECTOR dirVec = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	XMVECTOR quat = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMConvertToRadians(m_rotation.y));
	dirVec = XMVector3Normalize(XMVector3Rotate(dirVec, quat));
	physx::PxVec3 dir = { XMVectorGetX(dirVec), XMVectorGetY(dirVec), XMVectorGetZ(dirVec) };
	m_RaycastingManager.raycastingForMoveCheck(scene, hips, dir);
}

void Model::raycastingNextStep(physx::PxScene* scene)
{
	physx::PxVec3 nextToeBase = { m_currentStep.nextStep.x, m_currentStep.nextStep.y, m_currentStep.nextStep.z };
	physx::PxVec3 nextToeEnd = { m_currentStep.nextStepEnd.x, m_currentStep.nextStepEnd.y, m_currentStep.nextStepEnd.z };
	m_RaycastingManager.raycastingForNextStep(scene, nextToeBase, nextToeEnd);
}

void Model::checkCanMove()
{
	if (m_RaycastingManager.m_MoveCheck.part == EIKPart::NONE || m_RaycastingManager.m_NextStep.part == EIKPart::FAIL)
	{
		if (m_RaycastingManager.m_MoveCheck.part == EIKPart::NONE)
		{
			p("front block!!\n");
		}
		if (m_RaycastingManager.m_NextStep.part == EIKPart::FAIL)
		{
			p("foot block!!\n");
		}
		m_stop = true;
		m_position = m_prevPosition;
	}
}

void Model::moveModel(XMMATRIX& worldMatrix, float dt)
{
	// 진행 방향 구하기
	XMVECTOR dir = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	XMVECTOR quat = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(m_rotation.y));
	dir = XMVector3Rotate(dir, quat);
	dir = XMVectorScale(dir, m_speed * dt);

	// 현재 위치에 이동방향 더하기
	XMVECTOR pos = XMLoadFloat3(&m_position);
	pos = XMVectorAdd(pos, dir);
	XMStoreFloat3(&m_position, pos);
	m_prevPosition = m_position;

	worldMatrix = getWorldMatrix();
}

void Model::footRaycasting(physx::PxScene* scene, XMMATRIX& worldMatrix)
{
	physx::PxVec3 leftToeBase = m_pose.getBonePos(worldMatrix, m_skeleton.GetBoneIndex("mixamorig:LeftToeBase"));
	physx::PxVec3 leftToeEnd = m_pose.getBonePos(worldMatrix, m_skeleton.GetBoneIndex("mixamorig:LeftToe_End"));
	physx::PxVec3 rightToeBase = m_pose.getBonePos(worldMatrix, m_skeleton.GetBoneIndex("mixamorig:RightToeBase"));
	physx::PxVec3 rightToeEnd = m_pose.getBonePos(worldMatrix, m_skeleton.GetBoneIndex("mixamorig:RightToe_End"));
	m_RaycastingManager.raycastingForLeftFootIK(scene, leftToeBase, leftToeEnd);
	m_RaycastingManager.raycastingForRightFootIK(scene, rightToeBase, rightToeEnd);

	// Raycasting 실패시 IK 적용 x
	if (m_RaycastingManager.m_LeftFoot.part == EIKPart::FAIL || m_RaycastingManager.m_RightFoot.part == EIKPart::FAIL)
	{
		m_pose.UpdateFinalPos(m_skeleton);
		return;
	}
}

void Model::UpdateAnimation(physx::PxScene* scene, float dt)
{
	if (m_hasAnimation == true) {
		// 1. anmation update
		m_animStateManager.UpdateTime(dt);
		m_animStateManager.UpdateAnimationClip(m_pose, m_skeleton);
		
		// 3. 현재, 이전 애니메이션 기반 next step 계산 (step은 두 발의 Target Y값 + 이동 불가 판정에만 영향을 끼침)
		XMMATRIX worldMatrix = getWorldMatrix();
		UpdateNextStep(worldMatrix);
		
		// 4. next step 블랜딩
		blendingNextStep();
		m_animStateManager.BlendAnimation(m_pose);
		m_pose.UpdateWorldPos(m_skeleton);
		
		// 5. next step Raycasting
		raycastingNextStep(scene);
		
		// 6. 몸 앞쪽 Raycasting
		raycastingToForward(scene, worldMatrix);
		
		// 7. 이동 불가 check
		checkCanMove();
		if (m_stop == true)
		{
			m_pose.UpdateFinalPos(m_skeleton);
			m_stop = false;
			return;
		}

		// 8. 모델 move
		moveModel(worldMatrix, dt);
		
		// 9. 블랜딩 결과로 Raycasting
		footRaycasting(scene, worldMatrix);
		
		// 10. world Y값 결정
		modifyWorldY(scene, worldMatrix);
		
		// 11. bone 위치 보정
		modifyTarget(worldMatrix);
		
		// 12. target 위치 저장
		m_leftTarget = m_RaycastingManager.m_LeftFoot.target;
		m_rightTarget = m_RaycastingManager.m_RightFoot.target;

		/*
		 [Foot IK 적용]
		 0. IKangle Update
		 반복
		 1. pose 업데이트
		 2. Target, EndEffector, dx 계산
		 3. Jacobian Matrix 생성
		 4. DLS 적용
		 5. 각도 갱신
		 6. 반복 or 종료
		*/
		// 0. IKangle 업데이트
		p("\n\n\nstart IK!!!\n");

		m_IKManager.updateNowRotation(m_pose);
		m_IKManager.resetValuesForIK(m_RaycastingManager, m_skeleton, m_animStateManager.walkPhase, m_pose, worldMatrix);
		m_pose.UpdateIKWorldPos(m_skeleton, m_IKManager.getNowRotation());
		static const int MAX_ITERATION = 30;
		int iteration = 0;
		while (iteration < MAX_ITERATION)
		{
			// 1. Target, EndEffector, dx 계산
			m_IKManager.calculateTarget(m_pose, worldMatrix, m_RaycastingManager);
			// 2. Jacobian Matrix 생성
			m_IKManager.calculateJacobianMatrix(m_pose, worldMatrix);
			// 3. DLS 적용
			m_IKManager.solveDLS();
			// 4. 각도 갱신
			m_IKManager.updateAngle(m_pose, worldMatrix, m_skeleton);
			// 5. worldPos 업데이트
			m_pose.UpdateIKWorldPos(m_skeleton, m_IKManager.getNowRotation());
			// 6. 반복 or 종료
			if (m_IKManager.isFinish(m_pose, worldMatrix) == true)
			{
				break;
			}
			iteration++;
		}
		
		m_IKManager.blendingIKRotation();

		m_pose.IKChainBlending(m_IKManager.getChain(0), m_IKManager.getNowRotation(), 1.0f);
		m_pose.IKChainBlending(m_IKManager.getChain(1), m_IKManager.getNowRotation(), 1.0f);

		m_pose.UpdateFinalPos(m_skeleton);
		m_stop = false;
	}
}

float Model::getLeftFootBlendingAlpha()
{
	float walkPhase = m_animStateManager.walkPhase;


	// 14 ~ 26
	if (0.4f < walkPhase && walkPhase <= 0.743f)
	{
		return 1.0f;
	}

	// 6 ~ 14
	if (0.1714f < walkPhase && walkPhase <= 0.4f)
	{
		return std::fminf(0.5f, (walkPhase - 0.1714f) / 0.3286f);
	}

	// 26 ~ 30
	if (0.743f < walkPhase && walkPhase <= 0.857f)
	{
		return std::fminf(0.5f, 1.0f - ((walkPhase - 0.743f) / 0.114f));
	}

	return 0.5f;
}

void Model::ReleaseTextures()
{
	for (int i = 0; i < m_textures.size(); i++)
	{
		if (m_textures[i])
		{
			m_textures[i]->Shutdown();
			delete m_textures[i];
		}
	}

	return;
}

void Model::ReleaseMeshes()
{
	if (m_boneMesh)
	{
		m_boneMesh->Shutdown();
		delete m_boneMesh;
	}

	if (m_stepMesh)
	{
		m_stepMesh->Shutdown();
		delete m_stepMesh;
	}

	if (m_rangeAxisMesh)
	{
		m_rangeAxisMesh->Shutdown();
		delete m_rangeAxisMesh;
	}

	if (m_jointMesh)
	{
		m_jointMesh->Shutdown();
		delete m_jointMesh;
	}

	if (m_rayToTargetMesh)
	{
		m_rayToTargetMesh->Shutdown();
		delete m_rayToTargetMesh;
	}

	if (m_rayNormalMesh)
	{
		m_rayNormalMesh->Shutdown();
		delete m_rayNormalMesh;
	}

	if (m_cornMesh)
	{
		m_cornMesh->Shutdown();
		delete m_cornMesh;
	}

	for (int i = 0; i < m_size; i++)
	{
		if (m_meshes[i])
		{
			m_meshes[i]->Shutdown();
			delete m_meshes[i];
		}
	}

	return;
}

XMMATRIX Model::getWorldMatrix()
{
	float toRadian = XM_PI / 180.0f;
	XMMATRIX translation = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(m_rotation.x * toRadian, m_rotation.y * toRadian, m_rotation.z * toRadian);
	XMMATRIX rotation = XMMatrixRotationQuaternion(quaternion);
	XMMATRIX scale = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

	return scale * rotation * translation;  // 순서대로 적용: 스케일 -> 회전 -> 위치
}

XMMATRIX Model::getWorldMatrixNotIncludeScale()
{
	float toRadian = XM_PI / 180.0f;
	XMMATRIX translation = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(m_rotation.x * toRadian, m_rotation.y * toRadian, m_rotation.z * toRadian);
	XMMATRIX rotation = XMMatrixRotationQuaternion(quaternion);

	return rotation * translation;  // 순서대로 적용: 스케일 -> 회전 -> 위치
}

void Model::setPosition(XMFLOAT3 position)
{
	m_position = position;
	m_prevPosition = position;
}

void Model::setRotation(XMFLOAT3 rotation)
{
	m_rotation = rotation;
}

void Model::setScale(XMFLOAT3 scale)
{
	m_scale = scale;
}

void Model::ParseSkeleton(aiNode* node, int parentIndex, Skeleton& skeleton, const std::unordered_set<std::string>& usedBones) {
	std::string name = node->mName.C_Str();

	if (usedBones.find(name) == usedBones.end()) {
		for (unsigned int i = 0; i < node->mNumChildren; ++i)
			ParseSkeleton(node->mChildren[i], parentIndex, skeleton, usedBones);
		return;
	}

	int thisIndex = (int)skeleton.bones.size();

	Bone bone;
	bone.name = name;
	bone.parentIndex = parentIndex;
	bone.offsetMatrix = XMMatrixIdentity(); // offsetMatrix는 나중에 추가
	skeleton.nameToIndex[name] = thisIndex;
	skeleton.bones.push_back(bone);
	
	if (parentIndex != -1)
		skeleton.bones[parentIndex].children.push_back(thisIndex);
	else
		skeleton.rootBoneIdx = thisIndex;

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
		ParseSkeleton(node->mChildren[i], thisIndex, skeleton, usedBones);
}

DirectX::XMMATRIX Model::ConvertToXM(const aiMatrix4x4& m) {
	return XMMATRIX(
		m.a1, m.b1, m.c1, m.d1,
		m.a2, m.b2, m.c2, m.d2,
		m.a3, m.b3, m.c3, m.d3,
		m.a4, m.b4, m.c4, m.d4
	);
}

void Model::LoadBoneOffsets(const aiScene* scene, Skeleton& skeleton) {
	for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		for (unsigned int i = 0; i < mesh->mNumBones; ++i) {
			aiBone* aiBone = mesh->mBones[i];
			std::string name = aiBone->mName.C_Str();

			auto it = skeleton.nameToIndex.find(name);
			if (it != skeleton.nameToIndex.end()) {
				int boneIndex = it->second;
				skeleton.bones[boneIndex].offsetMatrix = ConvertToXM(aiBone->mOffsetMatrix);
			}
		}
	}
}

std::unordered_set<std::string> Model::CollectUsedBoneNames(const aiScene* scene) {
	std::unordered_set<std::string> boneNames;
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[i];
		for (unsigned int j = 0; j < mesh->mNumBones; ++j) {
			boneNames.insert(mesh->mBones[j]->mName.C_Str());
		}
	}
	return boneNames;
}

void Model::LoadAnimationData(const aiScene* scene, Skeleton& skeleton) {
	/*
		Bone 정보 저장 (root Bone은 parent idx == -1)
	*/ 

	std::unordered_set<std::string> boneSet = CollectUsedBoneNames(scene);
	ParseSkeleton(scene->mRootNode, -1, skeleton, boneSet);
	LoadBoneOffsets(scene, skeleton);

	// 애니메이션 정보 저장
	for (unsigned int i = 0; i < scene->mNumAnimations; ++i) {
		aiAnimation* aiAnim = scene->mAnimations[i];

		// 애니메이션 클립 생성
		AnimationClip clip;
		clip.name = aiAnim->mName.C_Str();	// 클립 이름
		clip.duration = aiAnim->mDuration;	// 총 Tick 수
		clip.ticksPerSecond = aiAnim->mTicksPerSecond != 0 ? aiAnim->mTicksPerSecond : 25.0; // 1초당 tick 수

		// 애니메이션 Bone Track 생성 (Bone 별로 애니메이션 keyframe 정보 저장)
		for (unsigned int j = 0; j < aiAnim->mNumChannels; ++j) {
			// Bone 선택 (channel == bone 1개의 애니메이션 트랙)
			aiNodeAnim* channel = aiAnim->mChannels[j];
			std::string boneName = channel->mNodeName.C_Str();

			BoneTrack track;
			track.boneName = boneName;
			{
				for (unsigned int k = 0; k < channel->mNumPositionKeys; ++k) {
					auto& kf = channel->mPositionKeys[k];
					track.positionKeys.push_back({ kf.mTime, { kf.mValue.x, kf.mValue.y, kf.mValue.z } });
				}
				track.positionKeys[0].time = 0.0f;
				
			}

			{
				for (unsigned int k = 0; k < channel->mNumRotationKeys; ++k) {
					auto& kf = channel->mRotationKeys[k];
					track.rotationKeys.push_back({ kf.mTime, { kf.mValue.x, kf.mValue.y, kf.mValue.z, kf.mValue.w } });
				}
				track.rotationKeys[0].time = 0.0f;
			}

			{
				for (unsigned int k = 0; k < channel->mNumScalingKeys; ++k) {
					auto& kf = channel->mScalingKeys[k];
					track.scaleKeys.push_back({ kf.mTime, { kf.mValue.x, kf.mValue.y, kf.mValue.z } });
				}
				track.rotationKeys[0].time = 0.0f;
			}

			clip.boneTracks[boneName] = track;
		}
		m_animationClips[clip.name] = clip;
	}

}

bool Model::HasAnimationInfo(const aiScene* scene){
	if (!scene || scene->mNumAnimations == 0) return false;

	for (unsigned int i = 0; i < scene->mNumAnimations; ++i) {
		if (scene->mAnimations[i]->mNumChannels > 0)
			return true;
	}

	return false;
}

bool Model::getRotateDir(XMFLOAT3& targetDir, XMFLOAT3& nowDir)
{
	XMVECTOR nowVec = XMLoadFloat3(&nowDir);
	XMVECTOR targetVec = XMLoadFloat3(&targetDir);

	XMVECTOR cross = XMVector3Cross(nowVec, targetVec);

	XMFLOAT3 result;
	XMStoreFloat3(&result, cross);
	if (result.y > 0.0f)
	{
		return true;
	}
	else
	{
		return false;
	}
}

XMFLOAT3 Model::getRotatedVector(float degree)
{
	XMFLOAT3 dir = { 0.0f, 0.0f, -1.0f };
	XMVECTOR v = XMLoadFloat3(&dir);
	XMMATRIX rotY = XMMatrixRotationY(m_rotation.y * XM_PI / 180.0f);
	XMVECTOR rotated = XMVector3TransformNormal(v, rotY);
	XMVECTOR normalized = XMVector3Normalize(rotated);
	XMStoreFloat3(&dir, rotated);

	return dir;
}

void Model::setToTarget(XMFLOAT3& targetDir)
{
	if (targetDir.z > 0.1f)
	{
		if (targetDir.x > 0.1f)
		{
			m_rotation.y = 225.0f;
		}
		else if (targetDir.x < -0.1f)
		{
			m_rotation.y = 135.0f;
		}
		else
		{
			m_rotation.y = 180.0f;
		}
	} else if (targetDir.z < -0.1f)
	{
		if (targetDir.x > 0.1f)
		{
			m_rotation.y = 315.0f;
		}
		else if (targetDir.x < -0.1f)
		{
			m_rotation.y = 45.0f;
		}
		else
		{
			m_rotation.y = 0.0f;
		}
	}
	else if (targetDir.x > 0.0f)
	{
		m_rotation.y = 270.0f;
	}
	else
	{
		m_rotation.y = 90.0f;
	}
}

void Model::speedDown()
{
	const static float accel = 1.0f;
	const static float minSpeed = 0.0f;
	
	m_speed = max(minSpeed, m_speed - accel);
}

void Model::move(XMFLOAT3& targetDir)
{
	const static float rotSpeed = 10.0f;
	const static float accel = 1.0f;
	const static float maxSpeed = 7.3f;

	// 현재 방향 벡터
	XMFLOAT3 nowDir = getRotatedVector(m_rotation.y);

	// target 벡터와 현재 벡터 사잇각 구하기
	XMVECTOR targetVec = XMLoadFloat3(&targetDir);
	XMVECTOR nowVec = XMLoadFloat3(&nowDir);
	XMVECTOR dot = XMVector3Dot(targetVec, nowVec);

	float cosVal;
	XMStoreFloat(&cosVal, dot);
	cosVal = std::clamp(cosVal, -1.0f, 1.0f);

	float theta = acosf(cosVal);

	if (theta < rotSpeed * XM_PI / 180.0f)
	{
		setToTarget(targetDir);
	}
	else
	{
		bool isCW = getRotateDir(targetDir, nowDir);

		// 회전 방향 구하기

		if (isCW)
		{
			m_rotation.y += rotSpeed;
		}
		else
		{
			m_rotation.y -= rotSpeed;
		}
		if (m_rotation.y >= 360.0f)
			m_rotation.y -= 360.0f;
	}

	m_speed = min(m_speed + accel, maxSpeed);
}

void Model::addMesh(Mesh* mesh)
{
	m_meshes.push_back(mesh);
	m_size = m_meshes.size();
}

void Model::addTexture(Texture* texture)
{
	m_textures.push_back(texture);
}

float Model::getSpeed()
{
	return m_speed;
}

void Model::createStaticBox(physx::PxPhysics* physics, physx::PxScene* scene)
{
	m_physicsObject = new PhysicsObject();
	m_physicsObject->createStaticObject(physics);
	m_physicsObject->setMaterial(physics, 0.6f, 0.6f, 0.3f);
	m_physicsObject->createBoxShape(physics, physx::PxMeshScale(1.0f));
	m_physicsObject->addToScene(scene);
}

void Model::createStaticSphere(physx::PxPhysics* physics, physx::PxScene* scene)
{
	m_physicsObject = new PhysicsObject();
	m_physicsObject->createStaticObject(physics);
	m_physicsObject->setMaterial(physics, 0.6f, 0.6f, 0.3f);
	m_physicsObject->createSphereShape(physics, physx::PxMeshScale(1.0f));
	m_physicsObject->addToScene(scene);
}

void Model::syncModelWithRigidbody(physx::PxPhysics* physics)
{
	if (m_physicsObject)
	{
		// degree -> radian 변환
		float pitch = m_rotation.x * (3.14159f / 180.0f);  // pitch (X축 회전)
		float yaw = m_rotation.y * (3.14159f / 180.0f);    // yaw (Y축 회전)
		float roll = m_rotation.z * (3.14159f / 180.0f);   // roll (Z축 회전)

		// PhysX 퀘터니언으로 변환
		physx::PxQuat quatRotation = physx::PxQuat(pitch, physx::PxVec3(1.0f, 0.0f, 0.0f)) *
			physx::PxQuat(yaw, physx::PxVec3(0.0f, 1.0f, 0.0f)) *
			physx::PxQuat(roll, physx::PxVec3(0.0f, 0.0f, 1.0f));

		m_physicsObject->updatePosition(physx::PxVec3(m_position.x, m_position.y, m_position.z));
		m_physicsObject->updateRotation(quatRotation);
		m_physicsObject->updateScale(physics, m_scale);
	}
}

void Model::setYoffset()
{
	XMFLOAT3 nowPosition = m_position;
	XMFLOAT3 nowRotation = m_rotation;
	m_position = { 0.0f, 0.0f, 0.0f };
	m_rotation = { -90.0f, 0.0f, 0.0f };
	XMMATRIX worldMatrix = getWorldMatrix();
	m_animStateManager.getMinYoffset(m_pose, m_skeleton, worldMatrix, m_animationClips["walk"], "mixamorig:LeftToeBase", "mixamorig:RightToeBase");
	m_position = nowPosition;
	m_rotation = nowRotation;
}

void Model::setTargetToHipsKeyFrame()
{
	XMFLOAT3 nowPosition = m_position;
	XMFLOAT3 nowRotation = m_rotation;
	m_position = { 0.0f, 0.0f, 0.0f };
	XMMATRIX worldMatrix = getWorldMatrix();

	for (auto& pair : m_animationClips)
	{
		m_animStateManager.setTargetToHipsKeyFrame(m_pose, m_skeleton, worldMatrix, pair.second, "mixamorig:LeftToeBase", "mixamorig:RightToeBase");
	}
	
	m_position = nowPosition;
	m_rotation = nowRotation;
}