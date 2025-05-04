#include "AnimationPlayer.h"

void AnimationPlayer::Play(AnimationClip* newClip, float walkPhase) {
	clip = newClip;
	currentTime = 0.0f;
}

float AnimationPlayer::UpdateTime(float deltaTime) {
	static const float speed = 4.0f;

	if (!clip) return 0.0f;

	currentTime += speed * deltaTime * clip->ticksPerSecond;
	if (currentTime >= clip->duration)
		currentTime = fmod(currentTime, clip->duration);

	if (clip->name == "walk")
	{
		return currentTime / clip->duration;
	}
	else
	{
		return 1.0f;
	}
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