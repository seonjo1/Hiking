#pragma once

#include "AnimationData.h"
#include <PxPhysicsAPI.h>
#include <stack>

struct LocalTx {
    XMFLOAT3 position {0.0f, 0.0f, 0.0f};
    XMFLOAT4 rotation {0.0f, 0.0f, 0.0f, 1.0f};
    XMFLOAT3 scale {1.0f, 1.0f, 1.0f};
};

struct IKChain;

struct Pose {
	std::vector<LocalTx> local;
    std::vector<XMMATRIX> world;
    std::vector<XMMATRIX> finalMatrix;
    std::vector<XMMATRIX> rayMatrix;
    int count;
    
    void Initialize(size_t boneCount);
    physx::PxVec3 getBonePos(XMMATRIX& worldMatrix, int boneIdx);
	void UpdateWorldPos(const Skeleton& skeleton);
    void UpdateFinalPos(const Skeleton& skeleton);
    void UpdateIKWorldPos(const Skeleton& skeleton, std::vector<XMFLOAT4>& IKRotation);
    void IKChainBlending(IKChain& chain, std::vector<XMFLOAT4>& IKRotation, float blendAlpha);
};