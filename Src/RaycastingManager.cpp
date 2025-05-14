#include "RaycastingManager.h"
#include "IKManager.h"

const float RaycastingManager::s_RayStartOffset = -3.5f;
const float RaycastingManager::s_RayDistance = 7.0f;
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
	static const float toToeEndScale = 0.3f;
	static const float toFootScale = 0.5f;

    physx::PxVec3 toToeEnd = physx::PxVec3(toeEndPose.x - toeBasePose.x, 0.0f, toeEndPose.z - toeBasePose.z);
    toToeEnd = toToeEnd.getNormalized();
    physx::PxVec3 toFoot = -toToeEnd;

    toToeEnd = toToeEnd * toToeEndScale;
    toFoot = toFoot * toFootScale;

    footRaycasting(scene, toeBasePose, toToeEnd, toFoot, m_LeftFoot);
}


void RaycastingManager::raycastingForRightFootIK(physx::PxScene* scene, physx::PxVec3 toeBasePose, physx::PxVec3 toeEndPose)
{
	static const float toToeEndScale = 0.3f;
	static const float toFootScale = 0.5f;

	physx::PxVec3 toToeEnd = physx::PxVec3(toeEndPose.x - toeBasePose.x, 0.0f, toeEndPose.z - toeBasePose.z);
	toToeEnd = toToeEnd.getNormalized();
	physx::PxVec3 toFoot = -toToeEnd;

	toToeEnd = toToeEnd * toToeEndScale;
	toFoot = toFoot * toFootScale;

    footRaycasting(scene, toeBasePose, toToeEnd, toFoot, m_RightFoot);
}

void RaycastingManager::raycastingForNextStep(physx::PxScene* scene, physx::PxVec3 toeBasePose, physx::PxVec3 toeEndPose)
{
	static const float toToeEndScale = 0.3f;
	static const float toFootScale = 0.5f;

	physx::PxVec3 toToeEnd = physx::PxVec3(toeEndPose.x - toeBasePose.x, 0.0f, toeEndPose.z - toeBasePose.z);
	toToeEnd = toToeEnd.getNormalized();
	physx::PxVec3 toFoot = -toToeEnd;

	toToeEnd = toToeEnd * toToeEndScale;
	toFoot = toFoot * toFootScale;

	footRaycasting(scene, toeBasePose, toToeEnd, toFoot, m_NextStep);
}

void RaycastingManager::raycastingForMoveCheck(physx::PxScene* scene, physx::PxVec3 hips, physx::PxVec3 dir)
{
	const float rayDistance = 2.8f;
	const float rowOffset = 0.7f;
	const float colOffset = -0.8f;
    const float offset = -0.3f;
	static physx::PxRaycastBuffer leftRayHit, rightRayHit, centerRayHit;
    
    physx::PxVec3 up = { 0.0f, 1.0f, 0.0f };
    physx::PxVec3 left = up.cross(dir);
    physx::PxVec3 right = -left;

	// left raycasting
	physx::PxVec3 leftRayStart = hips + left * rowOffset + colOffset * up + dir * offset;
	bool leftRaySuccess = scene->raycast(
        leftRayStart,
		dir,
		rayDistance,
		leftRayHit,
		physx::PxHitFlag::ePOSITION
	);

	m_MoveCheck.part = EIKPart::FAIL;

	if (leftRaySuccess == true)
	{
		m_MoveCheck.part = EIKPart::NONE;
        m_MoveCheck.target = { leftRayHit.block.position.x, leftRayHit.block.position.y, leftRayHit.block.position.z };
	}

	physx::PxVec3 rightRayStart = hips + right * rowOffset + colOffset * up + dir * offset;
	bool rightRaySuccess = scene->raycast(
        rightRayStart,
		dir,
		rayDistance,
        rightRayHit,
		physx::PxHitFlag::ePOSITION
	);

	if (rightRaySuccess == true)
	{
        m_MoveCheck.part = EIKPart::NONE;
		m_MoveCheck.target = { rightRayHit.block.position.x, rightRayHit.block.position.y, rightRayHit.block.position.z };
	}

	physx::PxVec3 centerRayStart = hips + colOffset * up + dir * offset;
	bool centerRaySuccess = scene->raycast(
		centerRayStart,
		dir,
		rayDistance,
		centerRayHit,
		physx::PxHitFlag::ePOSITION
	);

	if (centerRaySuccess == true)
	{
		m_MoveCheck.part = EIKPart::NONE;
		m_MoveCheck.target = { centerRayHit.block.position.x, centerRayHit.block.position.y, centerRayHit.block.position.z };
	}
}

void RaycastingManager::raycastingForBlockInfo(physx::PxScene* scene, physx::PxVec3 xzDir)
{
	static const float yOffset = 10.0f;
	static const float distance = 15.0f;

	static physx::PxRaycastBuffer rayHit;

	physx::PxVec3 dir = { 0.0f, -1.0f, 0.0f };
	physx::PxVec3 offset = { 0.0f, 1.0f, 0.0f };
	physx::PxVec3 start = { m_FindObstacle.pos.x, m_FindObstacle.pos.y, m_FindObstacle.pos.z };
	physx::PxVec3 rayStart = start + offset * yOffset + xzDir * 0.01f;
	bool raySuccess = scene->raycast(
		rayStart,
		dir,
		distance,
		rayHit,
		physx::PxHitFlag::ePOSITION
	);

	m_FindObstacle.target.x = rayHit.block.position.x;
	m_FindObstacle.target.y = rayHit.block.position.y;
	m_FindObstacle.target.z = rayHit.block.position.z;
}


bool RaycastingManager::raycastingForFindBlock(physx::PxScene* scene, physx::PxVec3 start, physx::PxVec3 end, physx::PxVec3 xzDir)
{
	static physx::PxRaycastBuffer rayHit;

	// start 가 물체 안에 있는지 확인
	{
		physx::PxRaycastBuffer hit;
		physx::PxVec3 up = { 0.0f, 1.0f, 0.0f };
		physx::PxVec3 rayStart = start + up * 10.0f;
		bool raySuccess = scene->raycast(
			rayStart,
			-up,
			30.0f,
			hit,
			physx::PxHitFlag::ePOSITION
		);

		// start가 물체 아래 있으면 그 위치 살짝 앞으로 target 설정 
		if (raySuccess && start.y < hit.block.position.y - 0.1f)
		{
			m_FindObstacle.target.x = hit.block.position.x + xzDir.x * 0.1f;
			m_FindObstacle.target.y = hit.block.position.y;
			m_FindObstacle.target.z = hit.block.position.z + xzDir.z * 0.1f;
			return true;
		}
	}

	physx::PxVec3 dir = (end - start);
	float rayDistance = dir.magnitude();
	dir.normalize();
	physx::PxVec3 up = { 0.0f, 1.0f, 0.0f };
	float lOffset = 0.0f;
	float rOffset = 3.5f;

	for (int i = 0; i < 15; i++)
	{
		float offset = (lOffset + rOffset) * 0.5f;
		physx::PxVec3 rayStart = start + up * offset;
	
		bool raySuccess = scene->raycast(
			rayStart,
			dir,
			rayDistance,
			rayHit,
			physx::PxHitFlag::ePOSITION
		);

		if (raySuccess == true)
		{
			lOffset = offset;
			m_FindObstacle.pos.x = rayHit.block.position.x;
			m_FindObstacle.pos.y = rayHit.block.position.y;
			m_FindObstacle.pos.z = rayHit.block.position.z;
		}
		else
		{
			rOffset = offset;
		}
	}

	if (lOffset == 0.0f)
	{
		return false;
	}
	else
	{
		raycastingForBlockInfo(scene, xzDir);
		return true;
	}
}

void RaycastingManager::raycastingForY(physx::PxScene* scene, physx::PxVec3 hipsPose, physx::PxVec3 hipsFrontPose)
{
	const float rayStartOffset = -5.0f;
	const float rayDistance = 15.0f;
    static physx::PxRaycastBuffer rayHit, frontRayHit;

    // toe raycasting
    physx::PxVec3 rayStart = hipsPose + s_GravityDir * rayStartOffset;
    bool raySuccess = scene->raycast(
        rayStart,
        s_GravityDir,
        rayDistance,
        rayHit,
        physx::PxHitFlag::ePOSITION | physx::PxHitFlag::eNORMAL
    );

    // raycasting 성공시 경사면 각도 확인 (너무 크면 hit 안한처리
	m_Y.part = EIKPart::NONE;

    // 정보 2개중 1개 탈락
    if (raySuccess)
    {
        m_Y.pos = XMFLOAT3(rayHit.block.position.x, rayHit.block.position.y, rayHit.block.position.z);
        m_Y.normal = XMFLOAT3(rayHit.block.normal.x, rayHit.block.normal.y, rayHit.block.normal.z);
    }
    else
    {
        m_Y.part = EIKPart::FAIL;
    }

    physx::PxVec3 frontRayStart = hipsFrontPose + s_GravityDir * rayStartOffset;
	bool frontRaySuccess = scene->raycast(
		frontRayStart,
		s_GravityDir,
        rayDistance,
		frontRayHit,
		physx::PxHitFlag::ePOSITION | physx::PxHitFlag::eNORMAL
	);

    if (frontRaySuccess)
    {
        if (raySuccess && m_Y.pos.y < frontRayHit.block.position.y)
        {
			m_Y.pos.y = frontRayHit.block.position.y;
			m_Y.normal = XMFLOAT3(frontRayHit.block.normal.x, frontRayHit.block.normal.y, frontRayHit.block.normal.z);
        }
    }
}

