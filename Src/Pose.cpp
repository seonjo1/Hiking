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

        // local ��ȯ
        XMMATRIX T = XMMatrixTranslation(local[idx].position.x, local[idx].position.y, local[idx].position.z);
        XMMATRIX R = XMMatrixRotationQuaternion(XMLoadFloat4(&(local[idx].rotation)));
        XMMATRIX S = XMMatrixScaling(local[idx].scale.x, local[idx].scale.y, local[idx].scale.z);
        XMMATRIX local = S * R * T;

        if (parent == -1) {
            // ��Ʈ bone�� ��� local�� ����
            world[idx] = local;
        }
        else {
            // �θ��� transform�� �ڽ��� local transform �� ����
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

        // local ��ȯ
        XMMATRIX T = XMMatrixTranslation(local[idx].position.x, local[idx].position.y, local[idx].position.z);
        XMMATRIX R = XMMatrixRotationQuaternion(XMLoadFloat4(&(local[idx].rotation)));
        XMMATRIX S = XMMatrixScaling(local[idx].scale.x, local[idx].scale.y, local[idx].scale.z);
        XMMATRIX local = S * R * T;
      
        if (parent == -1) {
            // ��Ʈ bone�� ��� local�� ����
            world[idx] = local;
        }
        else {
            // �θ��� transform�� �ڽ��� local transform �� ����
            world[idx] = XMMatrixMultiply(local, world[parent]);
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
    local.resize(boneCount);
    world.resize(boneCount, XMMatrixIdentity());
    finalMatrix.resize(boneCount, XMMatrixIdentity());
    rayMatrix.resize(boneCount, XMMatrixIdentity());
}

XMMATRIX Pose::getLocalTranslationMatrix(int idx)
{
    XMMATRIX translation = XMMatrixTranslation(local[idx].position.x, local[idx].position.y, local[idx].position.z);

	// 1. twist �� ����ȭ
	XMVECTOR twistAxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR q = XMLoadFloat4(&local[idx].rotation);
	// 2. q �� ���� �κ��� twist �࿡ �翵 (���ʹϾ��� x, y, z�� �ٸ� �࿡ �����ϸ� �࿡���� ȸ���� �и��� �� ����)
	XMVECTOR qVec = XMVectorSet(XMVectorGetX(q), XMVectorGetY(q), XMVectorGetZ(q), 0.0f);
	float dotVT = XMVectorGetX(XMVector3Dot(qVec, twistAxis));
	XMVECTOR proj = XMVectorScale(twistAxis, dotVT);

	// 3. twist ���ʹϾ� (proj xyz + q.w) ����ȭ
	q = XMQuaternionNormalize(
		XMVectorSet(
			XMVectorGetX(proj),
			XMVectorGetY(proj),
			XMVectorGetZ(proj),
			XMVectorGetW(q)
		)
	);

    XMMATRIX rotation = XMMatrixRotationQuaternion(q);

    return rotation * translation;
}

void Pose::UpdateIKWorldPos(const Skeleton& skeleton, std::vector<XMFLOAT4>& IKRotation) {
	std::stack<int> s;
	s.push(0);
	while (!s.empty()) {
		int idx = s.top();
		s.pop();
		int parent = skeleton.bones[idx].parentIndex;

		// local ��ȯ
		XMMATRIX T = XMMatrixTranslation(local[idx].position.x, local[idx].position.y, local[idx].position.z);
		XMMATRIX R = XMMatrixRotationQuaternion(XMLoadFloat4(&(IKRotation[idx])));
		XMMATRIX S = XMMatrixScaling(local[idx].scale.x, local[idx].scale.y, local[idx].scale.z);
		XMMATRIX local = S * R * T;

		if (parent == -1) {
			// ��Ʈ bone�� ��� local�� ����
			world[idx] = local;
		}
		else {
			// �θ��� transform�� �ڽ��� local transform �� ����
			world[idx] = XMMatrixMultiply(local, world[parent]);
		}

		int childrenNum = skeleton.bones[idx].children.size();
		for (int i = 0; i < childrenNum; i++)
		{
			s.push(skeleton.bones[idx].children[i]);
		}
	}
}

void Pose::IKChainBlending(IKChain& chain, std::vector<XMFLOAT4>& IKRotation, float blendAlpha)
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
