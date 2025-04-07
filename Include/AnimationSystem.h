#pragma once

#include "Pose.h"

struct AnimTx {
    std::vector<XMFLOAT3> position;
    std::vector<XMFLOAT4> rotation;
    std::vector<XMFLOAT3> scale;
};

class AnimationPlayer {
public:
    AnimationClip* clip = nullptr;
    float currentTime = 0.0f;
    float speed = 1.0f;

    void Play(AnimationClip* clip);
    void Update(float deltaTime);
    void SamplePose(AnimTx& tx, const Skeleton& skeleton);
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
    float blendSpeed = 2.0f;

    void blendAnimTx(AnimTx& txA, AnimTx& txB, float blendAlpha);
    void GetFinalPose(Pose& outPose, Skeleton& skeleton);
    void Update(float dt);
    void SetState(std::string newState, std::unordered_map<std::string, AnimationClip>& clips);

};
