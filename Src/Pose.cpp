#include "Pose.h"

void Pose::ApplyHierarchy(const Skeleton& skeleton) {
    std::stack<int> s;
    s.push(0);
    //p("\n\n\nstart!!!!!!!!!!!!\n");
    while (!s.empty()) {
        int idx = s.top();
        s.pop();
        //p("now: " + std::to_string(idx) + "\n");
        int parent = skeleton.bones[idx].parentIndex;
        if (parent == -1) {
            // ��Ʈ bone�� ��� local�� ����
            world[idx] = local[idx];
        }
        else {
            // �θ��� transform�� �ڽ��� local transform �� ����
            world[idx] = XMMatrixMultiply(local[idx], world[parent]);
        }
        //p("local Matrix!!\n");
		//printMatrix(local[idx]);
		//p("offsetMatrix!!\n");
		//printMatrix(skeleton.bones[idx].offsetMatrix);
        // vertex�� ������ǥ�� �����Ƿ� inverse bind pose ��������� 
        finalMatrix[idx] = XMMatrixMultiply(skeleton.bones[idx].offsetMatrix, world[idx]);
		//p("finalMatrix!!\n");
		//printMatrix(finalMatrix[idx]);
        int childrenNum = skeleton.bones[idx].children.size();
        for (int i = 0; i < childrenNum; i++)
        {
            //p("add child : " + std::to_string(skeleton.bones[idx].children[i]) + "\n");
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