#include "Pose.h"

void Pose::UpdateFinalPos(const Skeleton& skeleton) {
    std::stack<int> s;
    s.push(0);
    while (!s.empty()) {
        int idx = s.top();
        s.pop();
        int parent = skeleton.bones[idx].parentIndex;
        if (parent == -1) {
            // ��Ʈ bone�� ��� local�� ����
            world[idx] = local[idx];
        }
        else {
            // �θ��� transform�� �ڽ��� local transform �� ����
            world[idx] = XMMatrixMultiply(local[idx], world[parent]);
        }
        // vertex�� ������ǥ�� �����Ƿ� inverse bind pose ��������� 
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