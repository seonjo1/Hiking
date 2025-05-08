#include "RaycastingManager.h"
#include "IKManager.h"

const float RaycastingManager::s_RayStartOffset = -5.0f;
const float RaycastingManager::s_RayDistance = 9.0f;
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

void RaycastingManager::footRaycasting(physx::PxScene* scene, physx::PxVec3 toeBasePose, physx::PxVec3 toToeEnd, physx::PxVec3 toFoot, RaycastingInfo& info)
{
    const float slopeDotThreshold = cosf(physx::PxDegToRad(50.0f));
    static physx::PxRaycastBuffer toeEndHit, toeBaseHit, footHit;

	physx::PxVec3 toeEndPose = toeBasePose + toToeEnd;
	physx::PxVec3 footPose = toeBasePose + toFoot;

    // toeBase Raycasting
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

	// toeEnd Raycasting
	physx::PxVec3 toeEndRayStart = toeEndPose + s_GravityDir * s_RayStartOffset;
	bool toeEndRaySuccess = scene->raycast(
		toeEndRayStart,
		s_GravityDir,
		s_RayDistance,
		toeEndHit,
		physx::PxHitFlag::ePOSITION | physx::PxHitFlag::eNORMAL
	);

	if (toeEndRaySuccess) {
		toeEndRaySuccess = toeEndHit.block.normal.dot(physx::PxVec3(0.0f, 1.0f, 0.0f)) > slopeDotThreshold;
	}

	// foot Raycasting
	physx::PxVec3 footRayStart = footPose + s_GravityDir * s_RayStartOffset;
	bool footRaySuccess = scene->raycast(
		footRayStart,
		s_GravityDir,
		s_RayDistance,
		footHit,
		physx::PxHitFlag::ePOSITION | physx::PxHitFlag::eNORMAL
	);

	if (footRaySuccess) {
        footRaySuccess = toeEndHit.block.normal.dot(physx::PxVec3(0.0f, 1.0f, 0.0f)) > slopeDotThreshold;
	}

    // info 채우기
	if (!toeEndRaySuccess && !toeBaseRaySuccess && !footRaySuccess)
	{
		info.part = EIKPart::FAIL;
        return;
	}
    else
    {
        info.part = EIKPart::TOEBASE;
    }

	XMVECTOR toeBaseRayStartPose = XMVectorSet(toeBaseRayStart.x, toeBaseRayStart.y, toeBaseRayStart.z, 1.0f);

    // foot info 채우기
    RaycastingInfo footInfo;
    if (footRaySuccess)
    {
        XMFLOAT3 footNormal = XMFLOAT3(footHit.block.normal.x, footHit.block.normal.y, footHit.block.normal.z);
        XMFLOAT3 N = XMFLOAT3(0.0f, 1.0f, 0.0f);
        XMVECTOR quat = IKManager::getQuatFromTo(N, footNormal);
        XMVECTOR toToeBase = XMVectorSet(-toFoot.x, -toFoot.y, -toFoot.z, 0.0f);
        XMVECTOR toTarget = XMVector3Rotate(toToeBase, quat);
        XMVECTOR foot = XMVectorSet(footHit.block.position.x, footHit.block.position.y, footHit.block.position.z, 1.0f);
        XMVECTOR target = XMVectorAdd(foot, toTarget);
        float footDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(toeBaseRayStartPose, target)));

        footInfo.pos = XMFLOAT3(footHit.block.position.x, footHit.block.position.y, footHit.block.position.z);
        XMStoreFloat3(&footInfo.target, target);
        footInfo.normal = footNormal;
        footInfo.distance = footDistance;
        footInfo.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);
    }

    // toeEnd info 채우기
	RaycastingInfo toeEndInfo;
    if (toeEndRaySuccess)
    {
		XMFLOAT3 toeEndNormal = XMFLOAT3(toeEndHit.block.normal.x, toeEndHit.block.normal.y, toeEndHit.block.normal.z);
		XMFLOAT3 N = XMFLOAT3(0.0f, 1.0f, 0.0f);
		XMVECTOR quat = IKManager::getQuatFromTo(N, toeEndNormal);
		XMVECTOR toToeBase = XMVectorSet(-toToeEnd.x, -toToeEnd.y, -toToeEnd.z, 0.0f);
		XMVECTOR toTarget = XMVector3Rotate(toToeBase, quat);
		XMVECTOR toeEnd = XMVectorSet(toeEndHit.block.position.x, toeEndHit.block.position.y, toeEndHit.block.position.z, 1.0f);
		XMVECTOR target = XMVectorAdd(toeEnd, toTarget);
		float toeEndDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(toeBaseRayStartPose, target)));

        toeEndInfo.pos = XMFLOAT3(toeEndHit.block.position.x, toeEndHit.block.position.y, toeEndHit.block.position.z);
		XMStoreFloat3(&toeEndInfo.target, target);
        toeEndInfo.normal = toeEndNormal;
        toeEndInfo.distance = toeEndDistance;
        toeEndInfo.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);
    }

    // toeBase info 채우기
	RaycastingInfo toeBaseInfo;
    if (toeBaseRaySuccess)
    {
        toeBaseInfo.pos = XMFLOAT3(toeBaseHit.block.position.x, toeBaseHit.block.position.y, toeBaseHit.block.position.z);
        toeBaseInfo.target = toeBaseInfo.pos;
        toeBaseInfo.normal = XMFLOAT3(toeBaseHit.block.normal.x, toeBaseHit.block.normal.y, toeBaseHit.block.normal.z);
        toeBaseInfo.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);
		XMVECTOR target = XMLoadFloat3(&toeBaseInfo.target);
		float toeBaseDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(toeBaseRayStartPose, target)));
        toeBaseInfo.distance = toeBaseDistance;
    }

    if (footRaySuccess)
    {
        fillInfo(info, footInfo);
    }
    else
    {
        info.distance = FLT_MAX;
    }

    if (toeEndRaySuccess && (info.distance > toeEndInfo.distance))
    {
        fillInfo(info, toeEndInfo);
    }

    if (toeBaseRaySuccess && (info.distance > toeBaseInfo.distance))
	{
		fillInfo(info, toeBaseInfo);
    }
}

void RaycastingManager::fillInfo(RaycastingInfo& dest, RaycastingInfo& src)
{
    dest.distance = src.distance;
    dest.normal = src.normal;
    dest.pos = src.pos;
    dest.target = src.target;
    dest.dir = src.dir;
}

void RaycastingManager::raycastingForLeftFootIK(physx::PxScene* scene, physx::PxVec3 toeBasePose, physx::PxVec3 toeEndPose)
{
	static const float toToeEndScale = 0.4f;
	static const float toFootScale = 0.55f;

    physx::PxVec3 toToeEnd = physx::PxVec3(toeEndPose.x - toeBasePose.x, 0.0f, toeEndPose.z - toeBasePose.z);
    toToeEnd = toToeEnd.getNormalized();
    physx::PxVec3 toFoot = -toToeEnd;

    toToeEnd = toToeEnd * toToeEndScale;
    toFoot = toFoot * toFootScale;

    footRaycasting(scene, toeBasePose, toToeEnd, toFoot, m_LeftFoot);
}


void RaycastingManager::raycastingForRightFootIK(physx::PxScene* scene, physx::PxVec3 toeBasePose, physx::PxVec3 toeEndPose)
{
	static const float toToeEndScale = 0.4f;
	static const float toFootScale = 0.55f;

	physx::PxVec3 toToeEnd = physx::PxVec3(toeEndPose.x - toeBasePose.x, 0.0f, toeEndPose.z - toeBasePose.z);
	toToeEnd = toToeEnd.getNormalized();
	physx::PxVec3 toFoot = -toToeEnd;

	toToeEnd = toToeEnd * toToeEndScale;
	toFoot = toFoot * toFootScale;

    footRaycasting(scene, toeBasePose, toToeEnd, toFoot, m_RightFoot);
}

void RaycastingManager::raycastingForY(physx::PxScene* scene, physx::PxVec3 hipsPose, physx::PxVec3 hipsFrontPose)
{
    static physx::PxRaycastBuffer rayHit, frontRayHit;

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
	m_Y.part = EIKPart::NONE;

    // 정보 2개중 1개 탈락
    if (raySuccess)
    {
        m_Y.pos = XMFLOAT3(rayHit.block.position.x, rayHit.block.position.y, rayHit.block.position.z);
    }
    else
    {
        m_Y.part = EIKPart::FAIL;
    }

    physx::PxVec3 frontRayStart = hipsFrontPose + s_GravityDir * s_RayStartOffset;
	bool frontRaySuccess = scene->raycast(
		frontRayStart,
		s_GravityDir,
		s_RayDistance,
		frontRayHit,
		physx::PxHitFlag::ePOSITION | physx::PxHitFlag::eNORMAL
	);

    if (frontRaySuccess)
    {
        if (raySuccess)
        {
            m_Y.pos.y = fmaxf(m_Y.pos.y, frontRayHit.block.position.y);
        }
        else
        {
            m_Y.pos.y = frontRayHit.block.position.y;
        }
    }
}