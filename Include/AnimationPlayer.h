#pragma once

#include "Pose.h"

class AnimationPlayer {
public:
	AnimationClip* clip = nullptr;
	Pose pose;
	float currentTime = 0.0f;
	float speed = 1.0f;
	XMFLOAT3 leftTargetToHips;
	XMFLOAT3 rightTargetToHips;

	void Play(AnimationClip* clip);
	float UpdateTime(float deltaTime);
	void UpdateTimeForYoffset(float time);
	void SamplePose(const Skeleton& skeleton);
	XMFLOAT3 InterpolatePosition(const std::vector<PositionKeyframe>& keys, float time);
	XMFLOAT4 InterpolateRotation(const std::vector<RotationKeyframe>& keys, float time);
	XMFLOAT3 InterpolateScale(const std::vector<ScaleKeyframe>& keys, float time);
	void moveToPose(std::vector<LocalTx>& poseLocal);
};