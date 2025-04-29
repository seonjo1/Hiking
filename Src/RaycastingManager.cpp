#include "RaycastingManager.h"

const float RaycastingManager::s_RayStartOffset = -5.0f;
const float RaycastingManager::s_RayDistance = 50.0f;
const physx::PxVec3 RaycastingManager::s_GravityDir = { 0.0f, -1.0f, 0.0f };

float RaycastingManager::getDistance(physx::PxVec3& toTarget, physx::PxVec3& dir)
{
    if (toTarget.dot(dir) < 0.0f)
    {
        return -toTarget.magnitude();
    }
    else
    {
        return toTarget.magnitude();
    }
}

void RaycastingManager::footRaycasting(physx::PxScene* scene, physx::PxVec3 footPose, physx::PxVec3 toePose, RaycastingInfo& info)
{
    const float slopeDotThreshold = cosf(physx::PxDegToRad(50.0f));
    static physx::PxRaycastBuffer toeHit;
    static physx::PxRaycastBuffer footHit;

    // ����Ʈ�� Raycasting ���� ���
    info.part = EIKPart::NONE;

    // toe raycasting
    physx::PxVec3 toeRayStart = toePose + s_GravityDir * s_RayStartOffset;
    bool toeRaySuccess = scene->raycast(
        toeRayStart,
        s_GravityDir,
        s_RayDistance,
        toeHit,
        physx::PxHitFlag::ePOSITION | physx::PxHitFlag::eNORMAL
    );

    // foot raycasting
    physx::PxVec3 footRayStart = footPose + s_GravityDir * s_RayStartOffset;
    bool footRaySuccess = scene->raycast(
        footRayStart,
        s_GravityDir,
        s_RayDistance,
        footHit,
        physx::PxHitFlag::ePOSITION | physx::PxHitFlag::eNORMAL
    );

    // raycasting ������ ���� ���� Ȯ�� (�ʹ� ũ�� hit ����ó��
    if (toeRaySuccess)
    {
        toeRaySuccess = toeHit.block.normal.dot(physx::PxVec3(0.0f, 1.0f, 0.0f)) > slopeDotThreshold;
    }
    else
    {
        info.part = EIKPart::FAIL;
    }

    if (footRaySuccess) {
        footRaySuccess = footHit.block.normal.dot(physx::PxVec3(0.0f, 1.0f, 0.0f)) > slopeDotThreshold;
    }
    else
    {
        info.part = EIKPart::FAIL;
    }

    // ���� 2���� 1�� Ż��
/*	if (toeRaySuccess && footRaySuccess)
	{
		float toeDistance = getDistance(toeHit.block.position - toePose, physx::PxVec3(0.0f, -1.0f, 0.0f));
		float footDistance = getDistance(footHit.block.position - footPose, physx::PxVec3(0.0f, -1.0f, 0.0f));
		if (toeDistance < footDistance)
		{
			info.pos = XMFLOAT3(toeHit.block.position.x, toeHit.block.position.y, toeHit.block.position.z);
			info.normal = XMFLOAT3(toeHit.block.normal.x, toeHit.block.normal.y, toeHit.block.normal.z);
			info.part = EIKPart::TOE;
			info.distance = toeDistance;
			info.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);
		}
		else
		{
			info.pos = XMFLOAT3(footHit.block.position.x, footHit.block.position.y, footHit.block.position.z);
			info.normal = XMFLOAT3(footHit.block.normal.x, footHit.block.normal.y, footHit.block.normal.z);
			info.part = EIKPart::FOOT;
			info.distance = footDistance;
			info.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);
		}
	}
	else if (toeRaySuccess)
	{
		float toeDistance = getDistance(toeHit.block.position - toePose, physx::PxVec3(0.0f, -1.0f, 0.0f));
		info.pos = XMFLOAT3(toeHit.block.position.x, toeHit.block.position.y, toeHit.block.position.z);
		info.normal = XMFLOAT3(toeHit.block.normal.x, toeHit.block.normal.y, toeHit.block.normal.z);
		info.part = EIKPart::TOE;
		info.distance = toeDistance;
		info.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);
	}
	else */if (footRaySuccess)
    {
        float footDistance = getDistance(footHit.block.position - footPose, physx::PxVec3(0.0f, -1.0f, 0.0f));
        info.pos = XMFLOAT3(footHit.block.position.x, footHit.block.position.y, footHit.block.position.z);
        info.normal = XMFLOAT3(footHit.block.normal.x, footHit.block.normal.y, footHit.block.normal.z);
        info.part = EIKPart::FOOT;
        info.distance = footDistance;
        info.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);
    }
}

void RaycastingManager::raycastingForLeftFootIK(physx::PxScene* scene, physx::PxVec3 footPose, physx::PxVec3 toePose)
{
    footRaycasting(scene, footPose, toePose, m_LeftFoot);
}


void RaycastingManager::raycastingForRightFootIK(physx::PxScene* scene, physx::PxVec3 footPose, physx::PxVec3 toePose)
{
    footRaycasting(scene, footPose, toePose, m_RightFoot);
}
