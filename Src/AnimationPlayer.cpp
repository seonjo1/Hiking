#include "AnimationPlayer.h"

void AnimationPlayer::Play(AnimationClip* newClip) {
	clip = newClip;
	currentTime = 0.0f;
}

float AnimationPlayer::UpdateTime(float deltaTime, float& walkPhase) {
	if (!clip) return 0.0f;

	currentTime += speed * deltaTime * clip->ticksPerSecond;

	if (currentTime >= clip->duration)
		currentTime = fmod(currentTime, clip->duration);

	if (clip->name == "walk")
	{
		walkPhase =  currentTime / clip->duration;
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

void AnimationPlayer::SamplePose(const Skeleton& skeleton) {
	for (size_t i = 0; i < skeleton.bones.size(); ++i) {
		const std::string& boneName = skeleton.bones[i].name;
		BoneTrack* track = clip->GetTrack(boneName);

		if (track) {
			pose.local[i].position = InterpolatePosition(track->positionKeys, currentTime);
			pose.local[i].rotation = InterpolateRotation(track->rotationKeys, currentTime);
			pose.local[i].scale = InterpolateScale(track->scaleKeys, currentTime);
		}
	}
}

void AnimationPlayer::UpdateTimeForYoffset(float time) {
	currentTime = time;
}


void AnimationPlayer::moveToPose(std::vector<LocalTx>& poseLocal)
{
	size_t count = poseLocal.size();

	for (size_t i = 0; i < count; ++i)
	{
		poseLocal[i].position = pose.local[i].position;
		poseLocal[i].rotation = pose.local[i].rotation;
		poseLocal[i].scale = pose.local[i].scale;
	}
}

float AnimationPlayer::getLeftGoRatio()
{
	float len;
	float phaseLen = (1.0f - clip->rightPhase + clip->leftPhase);
	float nowPhase = currentTime / clip->duration;
	if (nowPhase >= clip->rightPhase)
	{
		len = nowPhase - clip->rightPhase;
	}
	else
	{
		len = 1.0f - clip->rightPhase + nowPhase;
	}
	return len / phaseLen;
}

float AnimationPlayer::getRightGoRatio()
{
	float nowPhase = currentTime / clip->duration;
	return (nowPhase - clip->leftPhase) / (clip->rightPhase - clip->leftPhase);
}

float AnimationPlayer::getDt(bool leftGo)
{
	float ratio, dt, ticks;
	float nowPhase = currentTime / clip->duration;
	if (leftGo)
	{
		if (nowPhase >= clip->rightPhase)
		{
			ratio = 1.0f - nowPhase + clip->leftPhase;
		}
		else
		{
			ratio = (clip->leftPhase - nowPhase);
		}
	}
	else
	{
		ratio = clip->rightPhase - nowPhase;

	}

	ticks = ratio * clip->duration;
	dt = ticks / (speed * clip->ticksPerSecond);
	return dt;
}