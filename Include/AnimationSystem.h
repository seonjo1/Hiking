#pragma once

#include "Pose.h"

class AnimationPlayer {
public:
    AnimationClip* clip = nullptr;
    float currentTime = 0.0f;
    float speed = 1.0f;

    void Play(AnimationClip* clip);
    void UpdateTime(float deltaTime);
    void SamplePose(std::vector<LocalTx>& tx, const Skeleton& skeleton);
    XMFLOAT3 InterpolatePosition(const std::vector<PositionKeyframe>& keys, float time);
    XMFLOAT4 InterpolateRotation(const std::vector<RotationKeyframe>& keys, float time);
    XMFLOAT3 InterpolateScale(const std::vector<ScaleKeyframe>& keys, float time);
};

class AnimationStateManager {
public:
    std::string currentState;

    AnimationPlayer current;
    AnimationPlayer previous;
    float blendAlpha = 1.0f;
    float blendSpeed = 3.0f;

    void blendAnimTx(std::vector<LocalTx>& txVectorTarget, std::vector<LocalTx>& txVectorA, std::vector<LocalTx>& txVectorB, float blendAlpha);
    void UpdateAnimationClip(Pose& pose, Skeleton& skeleton);
    void UpdateTime(float dt);
    void SetState(std::string newState, std::unordered_map<std::string, AnimationClip>& clips);

};
