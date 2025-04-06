#include "AnimationSystem.h"

void AnimationPlayer::Play(AnimationClip* newClip) {
    clip = newClip;
    currentTime = 0.0f;
}

void AnimationPlayer::Update(float deltaTime) {
    if (!clip) return;
    currentTime += deltaTime * clip->ticksPerSecond;
    if (currentTime > clip->duration)
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

void AnimationPlayer::SamplePose(AnimTx& tx, const Skeleton& skeleton) {
    tx.position.resize(skeleton.bones.size());
    tx.rotation.resize(skeleton.bones.size());
    tx.scale.resize(skeleton.bones.size());

    for (size_t i = 0; i < skeleton.bones.size(); ++i) {
        const std::string& boneName = skeleton.bones[i].name;
        BoneTrack* track = clip->GetTrack(boneName);

        if (track) {
            tx.position[i] = InterpolatePosition(track->positionKeys, currentTime);
            tx.rotation[i] = InterpolateRotation(track->rotationKeys, currentTime);
            tx.scale[i] = InterpolateScale(track->scaleKeys, currentTime);
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

void AnimationStateManager::Update(float dt) {
    previous.Update(dt);
    current.Update(dt);
    blendAlpha += dt * blendSpeed;
    if (blendAlpha > 1.0f) blendAlpha = 1.0f;
}

void AnimationStateManager::blendAnimTx(AnimTx& txA, AnimTx& txB, float blendAlpha)
{
    size_t count = txA.position.size();

    for (size_t i = 0; i < count; ++i) {
        // Position
        XMVECTOR posA = XMLoadFloat3(&txA.position[i]);
        XMVECTOR posB = XMLoadFloat3(&txB.position[i]);
        XMVECTOR posLerp = XMVectorLerp(posA, posB, blendAlpha);
        XMStoreFloat3(&txA.position[i], posLerp);

        // Scale
        XMVECTOR scaleA = XMLoadFloat3(&txA.scale[i]);
        XMVECTOR scaleB = XMLoadFloat3(&txB.scale[i]);
        XMVECTOR scaleLerp = XMVectorLerp(scaleA, scaleB, blendAlpha);
        XMStoreFloat3(&txA.scale[i], scaleLerp);

        // Rotation (quaternion SLERP)
        XMVECTOR rotA = XMLoadFloat4(&txA.rotation[i]);
        XMVECTOR rotB = XMLoadFloat4(&txB.rotation[i]);
        XMVECTOR rotSlerp = XMQuaternionSlerp(rotA, rotB, blendAlpha);
        XMStoreFloat4(&txA.rotation[i], rotSlerp);
    }
}

void AnimationStateManager::GetFinalPose(Pose& outPose, Skeleton& skeleton) {
    AnimTx txA, txB;

    if (previous.clip == nullptr)
    {
        current.SamplePose(txB, skeleton);
        for (size_t i = 0; i < skeleton.bones.size(); ++i) {
            XMMATRIX T = XMMatrixTranslation(txA.position[i].x, txA.position[i].y, txA.position[i].z);
            XMMATRIX R = XMMatrixRotationQuaternion(XMLoadFloat4(&(txA.rotation[i])));
            XMMATRIX S = XMMatrixScaling(txA.scale[i].x, txA.scale[i].y, txA.scale[i].z);
            outPose.local[i] = S * R * T;
        }
        outPose.ApplyHierarchy(skeleton);
    }
    else {
        previous.SamplePose(txA, skeleton);
        current.SamplePose(txB, skeleton);

        for (size_t i = 0; i < skeleton.bones.size(); ++i) {
            blendAnimTx(txA, txB, blendAlpha);
            XMMATRIX T = XMMatrixTranslation(txA.position[i].x, txA.position[i].y, txA.position[i].z);
            XMMATRIX R = XMMatrixRotationQuaternion(XMLoadFloat4(&(txA.rotation[i])));
            XMMATRIX S = XMMatrixScaling(txA.scale[i].x, txA.scale[i].y, txA.scale[i].z);
            outPose.local[i] = S * R * T;
        }
        outPose.ApplyHierarchy(skeleton);
    }
}
