#pragma once

#include "AnimationPlayer.h"

class AnimationStateManager {
public:
	AnimationPlayer current;
	AnimationPlayer move;
	AnimationPlayer previous;
	std::string currentState;
	float prevY = 0.0f;
	float dampping = 1.0f;
	float walkPhase = 0.0f;
	float blendAlpha = 0.0f;
	float blendSpeed = 3.0f;
	float startPhase = 0.0f;
	XMFLOAT3 leftTargetToHips;
	XMFLOAT3 rightTargetToHips;

	void UpdateTime(float dt);
	void UpdateAnimationClip(Pose& pose, Skeleton& skeleton);
	void BlendAnimation(Pose& pose);
	void blendAnimTx(std::vector<LocalTx>& poseLocal, float blendAlpha);
	void initAnimationPlayer(int boneSize);

	void setDampping(float d);
	void resetDampping();
	void SetMoveState(std::string newState, std::unordered_map<std::string, AnimationClip>& clips);
	void setTargetToHipsKeyFrame(Pose& pose, Skeleton& skeleton, XMMATRIX& worldMatrix, AnimationClip& clip, std::string leftPart, std::string rightPart);
	bool SetState(std::string newState, std::unordered_map<std::string, AnimationClip>& clips);

	void getMinYoffset(Pose& pose, Skeleton& skeleton, XMMATRIX& worldMatrix, AnimationClip& clip, std::string leftPart, std::string rightPart);
	void getCurrentWorldBoneTransform(Pose& pose, int idx);
	float getDistance(Skeleton& skeleton);
	float getLeftDistance(Skeleton& skeleton, bool isPrev);
	float getRightDistance(Skeleton& skeleton, bool isPrev);
	XMFLOAT3 getLeftTargetToHips();
	XMFLOAT3 getRightTargetToHips();

};
