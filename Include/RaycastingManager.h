#pragma once

#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <DirectXMath.h>
#include "Common.h"
#include <algorithm>

using namespace DirectX;

enum class EIKPart {
	TOEBASE,
	TOEEND,
	FAIL,
	NONE
};

struct RaycastingInfo {
	XMFLOAT3 target;
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT3 dir;
	EIKPart part;
	float distance;
};

class RaycastingManager {
public:
	void raycastingForLeftFootIK(physx::PxScene* scene, physx::PxVec3 toeBasePose, physx::PxVec3 toeEndPose);
	void raycastingForRightFootIK(physx::PxScene* scene, physx::PxVec3 toeBasePose, physx::PxVec3 toeEndPose);
	void raycastingForY(physx::PxScene* scene, physx::PxVec3 hipsPose, physx::PxVec3 hipsFrontPose);
	void raycastingForNextStep(physx::PxScene* scene, physx::PxVec3 toeBasePose, physx::PxVec3 toeEndPose);
	void raycastingForMoveCheck(physx::PxScene* scene, physx::PxVec3 hips, physx::PxVec3 leftToeBasePose, physx::PxVec3 rightToeBasePose, physx::PxVec3 dir);
	float getHeightMapValue(physx::PxScene* scene, float z, float x);
	RaycastingInfo m_LeftFoot;
	RaycastingInfo m_RightFoot;
	RaycastingInfo m_NextStep;
	RaycastingInfo m_MoveCheck;
	RaycastingInfo m_Y;

private:
	void footRaycasting(physx::PxScene* scene, physx::PxVec3 toeBasePose, physx::PxVec3 toToeEnd, physx::PxVec3 toFoot, RaycastingInfo& info);
	float getDistance(physx::PxVec3& toTarget, physx::PxVec3& dir);
	void fillInfo(RaycastingInfo& dest, RaycastingInfo& src);

	const static float s_RayStartOffset;
	const static float s_RayDistance;
	const static physx::PxVec3 s_GravityDir;

};