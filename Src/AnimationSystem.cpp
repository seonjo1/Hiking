#include "AnimationSystem.h"

void AnimationPlayer::Play(AnimationClip* newClip) {
    clip = newClip;
    currentTime = 0.0f;
}

void AnimationPlayer::UpdateTime(float deltaTime) {
    static const float speed = 20.0f;

    if (!clip) return;
    currentTime += speed * deltaTime * clip->ticksPerSecond;
    if (currentTime >= clip->duration)
        currentTime = fmod(currentTime, clip->duration);
}

XMFLOAT3 AnimationPlayer::InterpolatePosition(const std::vector<PositionKeyframe>& keys, float time) {
    if (keys.empty()) return XMFLOAT3(0, 0, 0);
    if (keys.size() == 1) return keys[0].position;


    for (size_t i = 0; i < keys.size() - 1; ++i) {
        if (time < keys[i + 1].time) {
            float t = (float)((time - keys[i].time) / (keys[i + 1].time - keys[i].time));
            XMVECTOR a = XMLoadFloat3(&keys[i].position);
            XMVECTOR b = XMLoadFloat3(&keys[i + 1].position);
            XMVECTOR result = XMVectorLerp(a, b, t);
            XMFLOAT3 out;
            XMStoreFloat3(&out, result);
            return out;
        }
    }

    return keys.back().position;
}

XMFLOAT4 AnimationPlayer::InterpolateRotation(const std::vector<RotationKeyframe>& keys, float time) {
    if (keys.empty()) return XMFLOAT4(0, 0, 0, 1);
    if (keys.size() == 1) return keys[0].rotation;

    for (size_t i = 0; i < keys.size() - 1; ++i) {
        if (time < keys[i + 1].time) {
            float t = (float)((time - keys[i].time) / (keys[i + 1].time - keys[i].time));
            XMVECTOR a = XMLoadFloat4(&keys[i].rotation);
            XMVECTOR b = XMLoadFloat4(&keys[i + 1].rotation);
            XMVECTOR result = XMQuaternionSlerp(a, b, t);
            XMFLOAT4 out;
            XMStoreFloat4(&out, result);
            return out;
        }
    }
    return keys.back().rotation;
}

XMFLOAT3 AnimationPlayer::InterpolateScale(const std::vector<ScaleKeyframe>& keys, float time) {
    if (keys.empty()) return XMFLOAT3(1, 1, 1);
    if (keys.size() == 1) return keys[0].scale;

    for (size_t i = 0; i < keys.size() - 1; ++i) {
        if (time < keys[i + 1].time) {
            float t = (float)((time - keys[i].time) / (keys[i + 1].time - keys[i].time));
            XMVECTOR a = XMLoadFloat3(&keys[i].scale);
            XMVECTOR b = XMLoadFloat3(&keys[i + 1].scale);
            XMVECTOR result = XMVectorLerp(a, b, t);
            XMFLOAT3 out;
            XMStoreFloat3(&out, result);
            return out;
        }
    }
    return keys.back().scale;
}

void AnimationPlayer::SamplePose(std::vector<LocalTx>& txVector, const Skeleton& skeleton) {
    for (size_t i = 0; i < skeleton.bones.size(); ++i) {
        const std::string& boneName = skeleton.bones[i].name;
        BoneTrack* track = clip->GetTrack(boneName);

        if (track) {
            txVector[i].position = InterpolatePosition(track->positionKeys, currentTime);
            txVector[i].rotation = InterpolateRotation(track->rotationKeys, currentTime);
            txVector[i].scale = InterpolateScale(track->scaleKeys, currentTime);
        }
    }
}

void AnimationStateManager::SetState(std::string newState, std::unordered_map<std::string, AnimationClip>& clips) {
    if (newState == currentState) {
        return;
    }

    if (clips.find(newState) == clips.end()) {
        return;
    }

    previous = current;
    current.Play(&(clips[newState]));
    currentState = newState;
    blendAlpha = 0.0f;
}

void AnimationStateManager::UpdateTime(float dt) {
	previous.UpdateTime(dt);
	current.UpdateTime(dt);
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

