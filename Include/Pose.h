#pragma once

#include "AnimationData.h"
#include <stack>

struct LocalTx {
    XMFLOAT3 position {0.0f, 0.0f, 0.0f};
    XMFLOAT4 rotation {0.0f, 0.0f, 0.0f, 1.0f};
    XMFLOAT3 scale {1.0f, 1.0f, 1.0f};
};

struct Pose {
    std::vector<LocalTx> local;
    std::vector<XMMATRIX> world;
    std::vector<XMMATRIX> finalMatrix;
    int count;
    
    void Initialize(size_t boneCount);
    void UpdateFinalPos(const Skeleton& skeleton);
};