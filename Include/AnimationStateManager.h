#pragma once

#include "AnimationPlayer.h"

class AnimationStateManager {
public:
	std::string currentState;
	XMFLOAT3 leftTargetToHips;
	XMFLOAT3 rightTargetToHips;

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
	void getMinYoffset(Pose& pose, Skeleton& skeleton, XMMATRIX& worldMatrix, AnimationClip& clip, std::string leftPart, std::string rightPart);
	void setTargetToHipsKeyFrame(Pose& pose, Skeleton& skeleton, XMMATRIX& worldMatrix, AnimationClip& clip, std::string leftPart, std::string rightPart);
	XMFLOAT3 getLeftTargetToHips();
	XMFLOAT3 getRightTargetToHips();
};
