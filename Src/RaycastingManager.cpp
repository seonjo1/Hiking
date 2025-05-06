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

void RaycastingManager::footRaycasting(physx::PxScene* scene, physx::PxVec3 toeBasePose, RaycastingInfo& info)
{
    const float slopeDotThreshold = cosf(physx::PxDegToRad(50.0f));
    static physx::PxRaycastBuffer toeEndHit;
    static physx::PxRaycastBuffer toeBaseHit;

    // 디폴트로 Raycasting 실패 등록
    info.part = EIKPart::NONE;

    // foot raycasting
    physx::PxVec3 toeBaseRayStart = toeBasePose + s_GravityDir * s_RayStartOffset;
    bool toeBaseRaySuccess = scene->raycast(
        toeBaseRayStart,
        s_GravityDir,
        s_RayDistance,
        toeBaseHit,
        physx::PxHitFlag::ePOSITION | physx::PxHitFlag::eNORMAL
    );

    if (toeBaseRaySuccess) {
        toeBaseRaySuccess = toeBaseHit.block.normal.dot(physx::PxVec3(0.0f, 1.0f, 0.0f)) > slopeDotThreshold;
    }
    else
    {
        info.part = EIKPart::FAIL;
    }

    if (toeBaseRaySuccess)
    {
		float toeBaseDistance = getDistance(toeBaseHit.block.position - toeBasePose, physx::PxVec3(0.0f, -1.0f, 0.0f));
		info.pos = XMFLOAT3(toeBaseHit.block.position.x, toeBaseHit.block.position.y, toeBaseHit.block.position.z);
        info.target = info.pos;
		info.normal = XMFLOAT3(toeBaseHit.block.normal.x, toeBaseHit.block.normal.y, toeBaseHit.block.normal.z);
		info.part = EIKPart::TOEBASE;
		info.distance = toeBaseDistance;
		info.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);
    }
}

void RaycastingManager::raycastingForLeftFootIK(physx::PxScene* scene, physx::PxVec3 toeBasePose)
{
    footRaycasting(scene, toeBasePose, m_LeftFoot);
}


void RaycastingManager::raycastingForRightFootIK(physx::PxScene* scene, physx::PxVec3 toeBasePose)
{
    footRaycasting(scene, toeBasePose, m_RightFoot);
}

void RaycastingManager::raycastingForY(physx::PxScene* scene, physx::PxVec3 hipsPose)
{
    const float slopeDotThreshold = cosf(physx::PxDegToRad(10.0f));
    static physx::PxRaycastBuffer rayHit;

    // 디폴트로 Raycasting 실패 등록
    m_Y.part = EIKPart::NONE;

    // toe raycasting
    physx::PxVec3 rayStart = hipsPose + s_GravityDir * s_RayStartOffset;
    bool raySuccess = scene->raycast(
        rayStart,
        s_GravityDir,
        s_RayDistance,
        rayHit,
        physx::PxHitFlag::ePOSITION | physx::PxHitFlag::eNORMAL
    );

    // raycasting 성공시 경사면 각도 확인 (너무 크면 hit 안한처리

    // 정보 2개중 1개 탈락
    if (raySuccess)
    {
        m_Y.pos = XMFLOAT3(rayHit.block.position.x, rayHit.block.position.y, rayHit.block.position.z);
    }
    else
    {
        m_Y.part = EIKPart::FAIL;
    }
}