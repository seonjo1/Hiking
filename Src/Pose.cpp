#include "Pose.h"

void Pose::UpdateFinalPos(const Skeleton& skeleton) {
    std::stack<int> s;
    s.push(0);
    while (!s.empty()) {
        int idx = s.top();
        s.pop();
        int parent = skeleton.bones[idx].parentIndex;
        if (parent == -1) {
            // 루트 bone인 경우 local만 적용
            world[idx] = local[idx];
        }
        else {
            // 부모의 transform과 자신의 local transform 을 곱함
            world[idx] = XMMatrixMultiply(local[idx], world[parent]);
        }
        // vertex가 월드좌표에 있으므로 inverse bind pose 곱해줘야함 
        finalMatrix[idx] = XMMatrixMultiply(skeleton.bones[idx].offsetMatrix, world[idx]);
        int childrenNum = skeleton.bones[idx].children.size();
        for (int i = 0; i < childrenNum; i++)
        {
            s.push(skeleton.bones[idx].children[i]);
        }
    }
}

void Pose::Initialize(size_t boneCount) {
    count = boneCount;
    local.resize(boneCount, XMMatrixIdentity());
    world.resize(boneCount, XMMatrixIdentity());
    finalMatrix.resize(boneCount, XMMatrixIdentity());
}