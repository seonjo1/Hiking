#pragma once

#include "Pose.h"

class AnimationPlayer {
public:
	AnimationClip* clip = nullptr;
	float currentTime = 0.0f;
	float speed = 1.0f;

	void Play(AnimationClip* clip, float walkPhase);
	float UpdateTime(float deltaTime);
	void SamplePose(std::vector<LocalTx>& tx, const Skeleton& skeleton);
	XMFLOAT3 InterpolatePosition(const std::vector<PositionKeyframe>& keys, float time);
	XMFLOAT4 InterpolateRotation(const std::vector<RotationKeyframe>& keys, float time);
	XMFLOAT3 InterpolateScale(const std::vector<ScaleKeyframe>& keys, float time);
};