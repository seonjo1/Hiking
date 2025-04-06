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

	Skeleton m_skeleton;							// 본 계층 정보
	std::unordered_map<std::string, AnimationClip> m_animationClips;
	AnimationStateManager m_animStateManager;		// 상태별 애니메이션 관리

	Pose m_pose;									// Pose (현재 프레임의 뼈 트랜스폼들)
	std::vector<XMMATRIX> m_skinningMatrices;		// GPU에 넘길 본 행렬들 (최종 스키닝용)

	// 애니메이션 여부
	bool m_hasAnimation = false;
	UINT m_size; // mesh 개수
};