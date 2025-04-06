#include "Pose.h"

void Pose::ApplyHierarchy(const Skeleton& skeleton) {
    for (size_t i = 0; i < count; ++i) {
        int parent = skeleton.bones[i].parentIndex;
        if (parent == -1) {
            // ��Ʈ bone�� ��� local�� ����
            world[i] = local[i];
        }
        else {
            // �θ��� transform�� �ڽ��� local transform �� ����
            world[i] = XMMatrixMultiply(local[i], world[parent]);
        }
        // vertex�� ������ǥ�� �����Ƿ� inverse bind pose ��������� 
        finalMatrix[i] = XMMatrixMultiply(skeleton.bones[i].offsetMatrix, world[i]);
    }
}

void Pose::Initialize(size_t boneCount) {
    count = boneCount;
    local.resize(boneCount, XMMatrixIdentity());
    world.resize(boneCount, XMMatrixIdentity());
    finalMatrix.resize(boneCount, XMMatrixIdentity());
}