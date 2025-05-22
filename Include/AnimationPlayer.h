#pragma once

#include "Pose.h"

class AnimationPlayer {
public:
	AnimationClip* clip = nullptr;
	Pose pose;
	float speed = 40.0f;
	float currentTime = 0.0f;
	XMFLOAT3 leftTargetToHips;
	XMFLOAT3 rightTargetToHips;

	void Play(AnimationClip* clip);
	void UpdateTimeForYoffset(float time);
	void SamplePose(const Skeleton& skeleton);
	void moveToPose(std::vector<LocalTx>& poseLocal);
	float UpdateTime(float deltaTime, float& walkPhase);
	XMFLOAT3 InterpolatePosition(const std::vector<PositionKeyframe>& keys, float time);
	XMFLOAT4 InterpolateRotation(const std::vector<RotationKeyframe>& keys, float time);
	XMFLOAT3 InterpolateScale(const std::vector<ScaleKeyframe>& keys, float time);

	float getDt(bool leftGo);
	float getLeftGoRatio();
	float getRightGoRatio();
};