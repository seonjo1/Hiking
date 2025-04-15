#include "Pose.h"
#include "IKManager.h"

physx::PxVec3 Pose::getBonePos(XMMATRIX& worldMatrix, int boneIdx)
{
    XMMATRIX boneWorld = XMMatrixMultiply(world[boneIdx], worldMatrix);
    XMVECTOR pos = XMVector3TransformCoord(XMVectorZero(), boneWorld);

    XMFLOAT3 out;
    XMStoreFloat3(&out, pos);

    return physx::PxVec3(out.x, out.y, out.z);
}

void Pose::UpdateWorldPos(const Skeleton& skeleton) {
    std::stack<int> s;
    s.push(0);
    while (!s.empty()) {
        int idx = s.top();
        s.pop();
        int parent = skeleton.bones[idx].parentIndex;

        // local 변환
        XMMATRIX T = XMMatrixTranslation(local[idx].position.x, local[idx].position.y, local[idx].position.z);
        XMMATRIX R = XMMatrixRotationQuaternion(XMLoadFloat4(&(local[idx].rotation)));
        XMMATRIX S = XMMatrixScaling(local[idx].scale.x, local[idx].scale.y, local[idx].scale.z);
        XMMATRIX local = S * R * T;

        if (parent == -1) {
            // 루트 bone인 경우 local만 적용
            world[idx] = local;
        }
        else {
            // 부모의 transform과 자신의 local transform 을 곱함
            world[idx] = XMMatrixMultiply(local, world[parent]);
        }

        int childrenNum = skeleton.bones[idx].children.size();
        for (int i = 0; i < childrenNum; i++)
        {
            s.push(skeleton.bones[idx].children[i]);
        }
    }
}

void Pose::UpdateFinalPos(const Skeleton& skeleton) {
    std::stack<int> s;
    s.push(0);
    while (!s.empty()) {
        int idx = s.top();
        s.pop();
        int parent = skeleton.bones[idx].parentIndex;

        // local 변환
        XMMATRIX T = XMMatrixTranslation(local[idx].position.x, local[idx].position.y, local[idx].position.z);
        XMMATRIX R = XMMatrixRotationQuaternion(XMLoadFloat4(&(local[idx].rotation)));
        XMMATRIX S = XMMatrixScaling(local[idx].scale.x, local[idx].scale.y, local[idx].scale.z);
        XMMATRIX local = S * R * T;
      
        if (parent == -1) {
            // 루트 bone인 경우 local만 적용
            world[idx] = local;
        }
        else {
            // 부모의 transform과 자신의 local transform 을 곱함
            world[idx] = XMMatrixMultiply(local, world[parent]);
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
    local.resize(boneCount);
    world.resize(boneCount, XMMatrixIdentity());
    finalMatrix.resize(boneCount, XMMatrixIdentity());
    rayMatrix.resize(boneCount, XMMatrixIdentity());
    IKRotation.resize(boneCount);
}

void Pose::UpdateIKRotation()
{
    for (int i = 0; i < count; i++)
    {
        IKRotation[i] = local[i].rotation;
    }
}

void Pose::UpdateIKWorldPos(const Skeleton& skeleton) {
	std::stack<int> s;
	s.push(0);
	while (!s.empty()) {
		int idx = s.top();
		s.pop();
		int parent = skeleton.bones[idx].parentIndex;

		// local 변환
		XMMATRIX T = XMMatrixTranslation(local[idx].position.x, local[idx].position.y, local[idx].position.z);
		XMMATRIX R = XMMatrixRotationQuaternion(XMLoadFloat4(&(IKRotation[idx])));
		XMMATRIX S = XMMatrixScaling(local[idx].scale.x, local[idx].scale.y, local[idx].scale.z);
		XMMATRIX local = S * R * T;

		if (parent == -1) {
			// 루트 bone인 경우 local만 적용
			world[idx] = local;
		}
		else {
			// 부모의 transform과 자신의 local transform 을 곱함
			world[idx] = XMMatrixMultiply(local, world[parent]);
		}

		int childrenNum = skeleton.bones[idx].children.size();
		for (int i = 0; i < childrenNum; i++)
		{
			s.push(skeleton.bones[idx].children[i]);
		}
	}
}

void Pose::IKChainBlending(IKChain& chain, float blendAlpha)
{
    int count = chain.Bones.size();
    for (int i = 0; i < count; i++)
    {
        IKBone& bone = chain.Bones[i];
        int idx = bone.idx;

        XMVECTOR aniQuat = XMLoadFloat4(&local[idx].rotation);
		XMVECTOR ikQuat = XMLoadFloat4(&IKRotation[idx]);
		XMVECTOR result = XMQuaternionSlerp(aniQuat, ikQuat, blendAlpha);
        XMStoreFloat4(&local[idx].rotation, result);
    }
}
