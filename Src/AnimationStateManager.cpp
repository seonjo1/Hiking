#include "AnimationStateManager.h"

void AnimationStateManager::SetState(std::string newState, std::unordered_map<std::string, AnimationClip>& clips) {
    if (newState == currentState) {
        return;
    }

    if (clips.find(newState) == clips.end()) {
        return;
    }

    previous = current;
    current.Play(&(clips[newState]), walkPhase);
    currentState = newState;
    blendAlpha = 0.0f;
    walkPhase = 0.0f;
}

void AnimationStateManager::UpdateTime(float dt) {
    walkPhase = current.UpdateTime(dt);
    previous.UpdateTime(dt);
	blendAlpha += dt * blendSpeed;
	if (blendAlpha > 1.0f) blendAlpha = 1.0f;
}

void AnimationStateManager::blendAnimTx(std::vector<LocalTx>& txVectorTarget, std::vector<LocalTx>& txVectorA, std::vector<LocalTx>& txVectorB, float blendAlpha)
{
    size_t count = txVectorTarget.size();

    for (size_t i = 0; i < count; ++i) {
        // Position
        XMVECTOR posA = XMLoadFloat3(&txVectorA[i].position);
        XMVECTOR posB = XMLoadFloat3(&txVectorB[i].position);
        XMVECTOR posLerp = XMVectorLerp(posA, posB, blendAlpha);
        XMStoreFloat3(&(txVectorTarget[i].position), posLerp);

        // Scale
        XMVECTOR scaleA = XMLoadFloat3(&txVectorA[i].scale);
        XMVECTOR scaleB = XMLoadFloat3(&txVectorB[i].scale);
        XMVECTOR scaleLerp = XMVectorLerp(scaleA, scaleB, blendAlpha);
        XMStoreFloat3(&(txVectorTarget[i].scale), scaleLerp);

        // Rotation (quaternion SLERP)
        XMVECTOR rotA = XMLoadFloat4(&txVectorA[i].rotation);
        XMVECTOR rotB = XMLoadFloat4(&txVectorB[i].rotation);
        XMVECTOR rotSlerp = XMQuaternionSlerp(rotA, rotB, blendAlpha);
        XMStoreFloat4(&(txVectorTarget[i].rotation), rotSlerp);
    }
}
void AnimationStateManager::UpdateAnimationClip(Pose& pose, Skeleton& skeleton) {
    if (previous.clip == nullptr)
    {
        current.SamplePose(pose.local, skeleton);
    }
    else {
        std::vector<LocalTx> txVectorA(skeleton.bones.size());
        std::vector<LocalTx> txVectorB(skeleton.bones.size());

        previous.SamplePose(txVectorA, skeleton);
        current.SamplePose(txVectorB, skeleton);
        blendAnimTx(pose.local, txVectorA, txVectorB, blendAlpha);
    }
}

