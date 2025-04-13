#pragma once

#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <DirectXMath.h>
#include "Common.h"

using namespace DirectX;

enum class EIKPart {
	FOOT,
	TOE,
	FAIL,
	NONE
};

struct RaycastingInfo {
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT3 dir;
	EIKPart part;
	float distance;
};

class RaycastingManager {
public:
	void raycastingForLeftFootIK(physx::PxScene* scene, physx::PxVec3 footPos, physx::PxVec3 toePose);
	void raycastingForRightFootIK(physx::PxScene* scene, physx::PxVec3 footPos, physx::PxVec3 toePose);

	RaycastingInfo m_LeftFoot;
	RaycastingInfo m_RightFoot;

private:
	void footRaycasting(physx::PxScene* scene, physx::PxVec3 footPose, physx::PxVec3 toePose, RaycastingInfo& info);
	float getDistance(physx::PxVec3& toTarget, physx::PxVec3& dir);

	const static float s_RayStartOffset;
	const static float s_RayDistance;
	const static physx::PxVec3 s_GravityDir;

};