#include "model.h"
#include "TextureShader.h"

Model::Model()
{
	m_position = { 0.0f, 0.0f, 0.0f };
	m_rotation = { 0.0f, 0.0f, 0.0f };
	m_scale = { 1.0f, 1.0f, 1.0f };
	m_size = 0;
}

Model::Model(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::string filename)
{
	m_position = { 0.0f, 0.0f, 0.0f };
	m_rotation = { 0.0f, 0.0f, 0.0f };
	m_scale = { 1.0f, 1.0f, 1.0f };
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
		m_textures.push_back(new Texture(device, deviceContext, materialInfo, dirname));
	}

	// animation 로딩
	if (HasAnimationInfo(scene) == true) {
		m_hasAnimation = true;
		LoadAnimationData(scene, m_skeleton);
		m_pose.Initialize(m_skeleton.bones.size());
		m_animStateManager.SetState("Run", m_animationClips);
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
		//v.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
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
		newMesh->setTexture(m_textures[mesh->mMaterialIndex]);
	m_meshes.push_back(newMesh);
}

void Model::Shutdown()
{
	// Release the model texture.
	ReleaseTextures();

	return;
}

bool Model::Draw(ID3D11DeviceContext* deviceContext, TextureShader* textureShader, Matrix& matrix)
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
		m_animStateManager.SetState(state, m_animationClips);
	}
}

void Model::UpdateAnimation(float dt)
{
	if (m_hasAnimation == true){
		m_animStateManager.Update(dt);
		m_animStateManager.GetFinalPose(m_pose, m_skeleton);
	}
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
	XMMATRIX translation = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
	XMMATRIX rotation = XMMatrixRotationQuaternion(quaternion);
	XMMATRIX scale = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

	return scale * rotation * translation;  // 순서대로 적용: 스케일 -> 회전 -> 위치
}

void Model::setPosition(XMFLOAT3 position)
{
	m_position = position;
}

void Model::setRotation(XMFLOAT3 rotation)
{
	m_rotation = rotation;
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

	//std::string s = "Add bone: " + bone.name + "\n";
	//p(s);
	//s = "parent Idx: " + to_string(parentIndex) + "\n";
	//p(s);
	//s = "now Idx: " + to_string(thisIndex) + "\n";
	//p(s);

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

		//std::string s = "animation clip name: " + clip.name + "\n";
		//p(s);

		// 애니메이션 Bone Track 생성 (Bone 별로 애니메이션 keyframe 정보 저장)
		for (unsigned int j = 0; j < aiAnim->mNumChannels; ++j) {
			// Bone 선택 (channel == bone 1개의 애니메이션 트랙)
			aiNodeAnim* channel = aiAnim->mChannels[j];
			std::string boneName = channel->mNodeName.C_Str();

			BoneTrack track;
			track.boneName = boneName;
			
			for (unsigned int k = 0; k < channel->mNumPositionKeys; ++k) {
				auto& kf = channel->mPositionKeys[k];
				track.positionKeys.push_back({ kf.mTime, { kf.mValue.x, kf.mValue.y, kf.mValue.z } });
			}

			for (unsigned int k = 0; k < channel->mNumRotationKeys; ++k) {
				auto& kf = channel->mRotationKeys[k];
				track.rotationKeys.push_back({ kf.mTime, { kf.mValue.x, kf.mValue.y, kf.mValue.z, kf.mValue.w } });
			}

			for (unsigned int k = 0; k < channel->mNumScalingKeys; ++k) {
				auto& kf = channel->mScalingKeys[k];
				track.scaleKeys.push_back({ kf.mTime, { kf.mValue.x, kf.mValue.y, kf.mValue.z } });
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
