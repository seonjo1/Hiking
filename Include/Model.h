#pragma once

#include <queue>
#include <cmath>
#include "mesh.h"
#include "AnimationStateManager.h"
#include "AnimationData.h"
#include "PhysicsObject.h"
#include "PhysicsManager.h"
#include "RaycastingManager.h"
#include "IKManager.h"
#include <unordered_set>

class TextureShader;
class JointShader;
class BoneShader;
class ModelShader;

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
		// 무게순 정렬 후 상위 4개만 사용
		std::sort(influences.begin(), influences.end(), [](std::pair<int, float>& a, std::pair<int, float>& b) {
			return a.second > b.second;
			});
		if (influences.size() > 4) influences.resize(4);

		// 정규화
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
	static Model* createSphere(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMFLOAT4 color);
	static Model* createBox(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMFLOAT4 color);

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

	void UpdateAnimation(physx::PxScene* scene, float dt);
	bool DrawTextureShader(ID3D11DeviceContext*, TextureShader*, Matrix&);
	bool DrawJointShader(ID3D11DeviceContext*, JointShader*, Matrix&);
	bool DrawBoneShader(ID3D11DeviceContext*, BoneShader*, Matrix&, XMFLOAT3);
	bool DrawModelShader(ID3D11DeviceContext*, ModelShader*, Matrix&);
	bool DrawRayLineShader(ID3D11DeviceContext*, BoneShader*, Matrix&, XMFLOAT3);

	void createStaticBox(physx::PxPhysics* physics, physx::PxScene* scene);
	void createStaticSphere(physx::PxPhysics* physics, physx::PxScene* scene);

	void Shutdown();
	XMMATRIX getWorldMatrix();
	XMMATRIX getWorldMatrixNotIncludeScale();
	void setPosition(XMFLOAT3);
	void setRotation(XMFLOAT3);
	void setScale(XMFLOAT3);
	void speedDown();
	void move(XMFLOAT3&);
	bool getRotateDir(XMFLOAT3& targetDir, XMFLOAT3& nowDir);
	XMFLOAT3 getRotatedVector(float degree);
	void setToTarget(XMFLOAT3& targetDir);
	void addMesh(Mesh* mesh);
	void addTexture(Texture* texture);
	void syncModelWithRigidbody(physx::PxPhysics* physics);
	float getSpeed();

private:
	void ReleaseTextures();
	void ReleaseMeshes();
	
	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;
	XMFLOAT3 m_scale;

	Mesh* m_jointMesh;
	Mesh* m_boneMesh;
	Mesh* m_rayToTargetMesh;
	Mesh* m_rayNormalMesh;
	std::vector<Mesh*> m_meshes;
	std::vector<Texture*> m_textures;
	PhysicsObject* m_physicsObject;

	Skeleton m_skeleton;							// 본 계층 정보
	std::unordered_map<std::string, AnimationClip> m_animationClips;
	AnimationStateManager m_animStateManager;		// 상태별 애니메이션 관리
	RaycastingManager m_RaycastingManager;
	IKManager m_IKManager;

	Pose m_pose;									// Pose (현재 프레임의 뼈 트랜스폼들)

	// 애니메이션 여부
	bool m_hasAnimation = false;
	float m_speed { 0.0f };
	UINT m_size; // mesh 개수
};