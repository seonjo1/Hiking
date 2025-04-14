#pragma once

#include "AnimationPlayer.h"

class AnimationStateManager {
public:
	std::string currentState;

	AnimationPlayer current;
	AnimationPlayer previous;
	float blendAlpha = 0.0f;
	float blendSpeed = 3.0f;
	float walkPhase = 0.0f;
	float startPhase = 0.0f;

	void blendAnimTx(std::vector<LocalTx>& txVectorTarget, std::vector<LocalTx>& txVectorA, std::vector<LocalTx>& txVectorB, float blendAlpha);
	void UpdateAnimationClip(Pose& pose, Skeleton& skeleton);
	void UpdateTime(float dt);
	void SetState(std::string newState, std::unordered_map<std::string, AnimationClip>& clips);

};
