#pragma once

#include "mesh.h"
#include "AnimationSystem.h"
#include "AnimationData.h"
#include "Pose.h"
#include <unordered_set>

class TextureShader;

struct Matrix
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
};

struct VertexBoneData {
	std::vector<std::pair<int, float>> influences;

	void Add(int boneIndex, float weight) {
		if (weight == 0) return;
		influences.emplace_back(boneIndex, weight);
	}

	void NormalizeAndTrim() {
		if (influences.size() == 0)
			return;
		// ���Լ� ���� �� ���� 4���� ���
		std::sort(influences.begin(), influences.end(), [](std::pair<int, float>& a, std::pair<int, float>& b) {
			return a.second > b.second;
			});
		if (influences.size() > 4) influences.resize(4);

		// ����ȭ
		float total = 0.0f;
		for (std::pair<int, float>& pair : influences) {
			total += pair.second;
		}
		if (total > 0.0f) {
			for (std::pair<int, float>& pair : influences) {
				pair.second /= total;
			}
		}
	}
};

class Model
{
public:
	Model();
	Model(ID3D11Device*, ID3D11DeviceContext*, std::string);
	Model(const Model&);
	~Model();

	void LoadByAssimp(ID3D11Device*, ID3D11DeviceContext*, std::string filename);	
	void processNode(ID3D11Device*, ID3D11DeviceContext*, aiNode*, const aiScene*);
	void processMesh(ID3D11Device*, ID3D11DeviceContext*, aiMesh*, const aiScene*);
	void LoadAnimationData(const aiScene* scene, Skeleton& skeleton);
	void ParseSkeleton(aiNode* node, int parentIndex, Skeleton& skeleton, const std::unordered_set<std::string>& usedBones);
	std::unordered_set<std::string> CollectUsedBoneNames(const aiScene* scene);
	XMMATRIX ConvertToXM(const aiMatrix4x4& m);
	void LoadBoneOffsets(const aiScene* scene, Skeleton& skeleton);
	bool HasAnimationInfo(const aiScene* scene);
	void setState(std::string state);

	void UpdateAnimation(float dt);
	bool Draw(ID3D11DeviceContext*, TextureShader*, Matrix&);
	void Shutdown();
	XMMATRIX getWorldMatrix();
	void setPosition(XMFLOAT3);
	void setRotation(XMFLOAT3);

private:
	void ReleaseTextures();
	void ReleaseMeshes();
	
	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;
	XMFLOAT3 m_scale;

	std::vector<Mesh*> m_meshes;
	std::vector<Texture*> m_textures;

	Skeleton m_skeleton;							// �� ���� ����
	std::unordered_map<std::string, AnimationClip> m_animationClips;
	AnimationStateManager m_animStateManager;		// ���º� �ִϸ��̼� ����

	Pose m_pose;									// Pose (���� �������� �� Ʈ��������)
	std::vector<XMMATRIX> m_skinningMatrices;		// GPU�� �ѱ� �� ��ĵ� (���� ��Ű�׿�)

	// �ִϸ��̼� ����
	bool m_hasAnimation = false;
	UINT m_size; // mesh ����
};