#include "Pose.h"

void Pose::ApplyHierarchy(const Skeleton& skeleton) {
    for (size_t i = 0; i < count; ++i) {
        int parent = skeleton.bones[i].parentIndex;
        if (parent == -1) {
            // 루트 bone인 경우 local만 적용
            world[i] = local[i];
        }
        else {
            // 부모의 transform과 자신의 local transform 을 곱함
            world[i] = XMMatrixMultiply(local[i], world[parent]);
        }
        // vertex가 월드좌표에 있으므로 inverse bind pose 곱해줘야함 
        finalMatrix[i] = XMMatrixMultiply(skeleton.bones[i].offsetMatrix, world[i]);
    }
}

void Pose::Initialize(size_t boneCount) {
    count = boneCount;
    local.resize(boneCount, XMMatrixIdentity());
    world.resize(boneCount, XMMatrixIdentity());
    finalMatrix.resize(boneCount, XMMatrixIdentity());
}