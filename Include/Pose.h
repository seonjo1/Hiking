#pragma once

#include "AnimationData.h"

struct Pose {
    std::vector<XMMATRIX> local;
    std::vector<XMMATRIX> world;
    std::vector<XMMATRIX> finalMatrix;
    int count;
    
    void Initialize(size_t boneCount);
    void ApplyHierarchy(const Skeleton& skeleton);
};