#include "AnimationStateManager.h"

bool AnimationStateManager::SetState(std::string newState, std::unordered_map<std::string, AnimationClip>& clips) {
    if (newState == currentState) {
        return false;
    }

    if (clips.find(newState) == clips.end()) {
        return false;
    }

    previous = current;
    current.Play(&(clips[newState]));
    currentState = newState;
    blendAlpha = 0.0f;
    if (newState == "walk")
    {
        prevY = 0.0f;
		walkPhase = 0.0f;
    }

    return true;
}

void AnimationStateManager::SetMoveState(std::string newState, std::unordered_map<std::string, AnimationClip>& clips) {
	move.Play(&(clips[newState]));
}

void AnimationStateManager::UpdateTime(float dt) {
    current.UpdateTime(dt, walkPhase);
    previous.UpdateTime(dt, walkPhase);

    if (current.clip->name == "walk")
    {
        move.currentTime = current.currentTime;
    }
    else if (previous.clip != nullptr && previous.clip->name == "walk")
    {
		move.currentTime = previous.currentTime;
    }

	blendAlpha += dt * blendSpeed;
	if (blendAlpha > 1.0f) blendAlpha = 1.0f;
}

void AnimationStateManager::blendAnimTx(std::vector<LocalTx>& poseLocal, float blendAlpha)
{
    size_t count = poseLocal.size();

	std::vector<LocalTx>& pl = previous.pose.local;
	std::vector<LocalTx>& cl = current.pose.local;

    for (size_t i = 0; i < count; ++i) {
        // Position
        XMVECTOR posA = XMLoadFloat3(&pl[i].position);
        XMVECTOR posB = XMLoadFloat3(&cl[i].position);
        XMVECTOR posLerp = XMVectorLerp(posA, posB, blendAlpha);
        XMStoreFloat3(&(poseLocal[i].position), posLerp);

        // Scale
        XMVECTOR scaleA = XMLoadFloat3(&pl[i].scale);
        XMVECTOR scaleB = XMLoadFloat3(&cl[i].scale);
        XMVECTOR scaleLerp = XMVectorLerp(scaleA, scaleB, blendAlpha);
        XMStoreFloat3(&(poseLocal[i].scale), scaleLerp);

        // Rotation (quaternion SLERP)
        XMVECTOR rotA = XMLoadFloat4(&pl[i].rotation);
        XMVECTOR rotB = XMLoadFloat4(&cl[i].rotation);
        XMVECTOR rotSlerp = XMQuaternionSlerp(rotA, rotB, blendAlpha);
        XMStoreFloat4(&(poseLocal[i].rotation), rotSlerp);
    }
}

void AnimationStateManager::UpdateAnimationClip(Pose& pose, Skeleton& skeleton) {
    if (previous.clip == nullptr)
    {
        current.SamplePose(skeleton);
        current.pose.UpdateWorldPos(skeleton);
    }
    else {
        previous.SamplePose(skeleton);
        current.SamplePose(skeleton);
        move.SamplePose(skeleton);
		previous.pose.UpdateWorldPos(skeleton);
		current.pose.UpdateWorldPos(skeleton);
    }
}

void AnimationStateManager::BlendAnimation(Pose& pose)
{
    if (previous.clip == nullptr)
	{
        current.moveToPose(pose.local);
		leftTargetToHips = current.leftTargetToHips;
		rightTargetToHips = current.rightTargetToHips;
    }
    else
	{
		blendAnimTx(pose.local, blendAlpha);

		XMVECTOR prevLToH = XMLoadFloat3(&(previous.leftTargetToHips));
		XMVECTOR currLToH = XMLoadFloat3(&(current.leftTargetToHips));
		XMVECTOR LToH = XMVectorLerp(prevLToH, currLToH, blendAlpha);
		XMStoreFloat3(&leftTargetToHips, LToH);

		XMVECTOR prevRToH = XMLoadFloat3(&(previous.rightTargetToHips));
		XMVECTOR currRToH = XMLoadFloat3(&(current.rightTargetToHips));
		XMVECTOR RToH = XMVectorLerp(prevRToH, currRToH, blendAlpha);
		XMStoreFloat3(&rightTargetToHips, RToH);

    }
}

void AnimationStateManager::getMinYoffset(Pose& pose, Skeleton& skeleton, XMMATRIX& worldMatrix, AnimationClip& clip, std::string leftPart, std::string rightPart)
{
    AnimationPlayer tmpPlayer;
	tmpPlayer.pose.Initialize(skeleton.bones.size());
    tmpPlayer.Play(&clip);
    BoneTrack& leftFootBoneTrack = clip.boneTracks[leftPart];
    std::vector<RotationKeyframe>& v = leftFootBoneTrack.rotationKeys;

	clip.minLeftFootOffset = FLT_MAX;
	clip.minRightFootOffset = FLT_MAX;

    for (int i = 0; i < v.size(); ++i)
    {
        float time = v[i].time;
        tmpPlayer.UpdateTimeForYoffset(time);
        tmpPlayer.SamplePose(skeleton);
        tmpPlayer.moveToPose(pose.local);
		pose.UpdateWorldPos(skeleton);
        
        XMVECTOR point = XMVectorSet(0, 0, 0, 1);
		XMMATRIX toLeft = XMMatrixMultiply(pose.world[skeleton.GetBoneIndex(leftPart)], worldMatrix);
		XMMATRIX toRight = XMMatrixMultiply(pose.world[skeleton.GetBoneIndex(rightPart)], worldMatrix);

        XMVECTOR left = XMVector3TransformCoord(point, toLeft);
        XMVECTOR right = XMVector3TransformCoord(point, toRight);

		clip.minLeftFootOffset = std::fminf(XMVectorGetY(left), clip.minLeftFootOffset);
		clip.minRightFootOffset = std::fminf(XMVectorGetY(right), clip.minRightFootOffset);
    }
}

void AnimationStateManager::setTargetToHipsKeyFrame(Pose& pose, Skeleton& skeleton, XMMATRIX& worldMatrix, AnimationClip& clip, std::string leftPart, std::string rightPart)
{
    static const float groundY = 0.07f;

    AnimationPlayer tmpPlayer;
    tmpPlayer.pose.Initialize(skeleton.bones.size());
    tmpPlayer.Play(&clip);
    BoneTrack& leftFootBoneTrack = clip.boneTracks[leftPart];
    std::vector<RotationKeyframe>& v = leftFootBoneTrack.rotationKeys;

    int count = v.size();

    clip.leftTargetToHipsVector.resize(count);
    clip.rightTargetToHipsVector.resize(count);

    float prevLeftY = 0.0f;
    float prevRightY = 0.0f;

    for (int i = 0; i < count; ++i)
    {
        float time = v[i].time;
        tmpPlayer.UpdateTimeForYoffset(time);
        tmpPlayer.SamplePose(skeleton);
        tmpPlayer.moveToPose(pose.local);
        pose.UpdateWorldPos(skeleton);

        XMVECTOR point = XMVectorSet(0, 0, 0, 1);
        XMMATRIX toHips = XMMatrixMultiply(pose.world[skeleton.GetBoneIndex("mixamorig:Hips")], worldMatrix);
		XMMATRIX toLeft = XMMatrixMultiply(pose.world[skeleton.GetBoneIndex(leftPart)], worldMatrix);
		XMMATRIX toLeftEnd = XMMatrixMultiply(pose.world[skeleton.GetBoneIndex("mixamorig:LeftToe_End")], worldMatrix);
		XMMATRIX toRight = XMMatrixMultiply(pose.world[skeleton.GetBoneIndex(rightPart)], worldMatrix);
		XMMATRIX toRightEnd = XMMatrixMultiply(pose.world[skeleton.GetBoneIndex("mixamorig:RightToe_End")], worldMatrix);

        XMVECTOR hips = XMVector3TransformCoord(point, toHips);
		XMVECTOR left = XMVector3TransformCoord(point, toLeft);
		XMVECTOR right = XMVector3TransformCoord(point, toRight);

        if (clip.name != "idle")
        {
            if (prevLeftY > groundY && XMVectorGetY(left) < groundY)
            {
                clip.leftDist = tmpPlayer.pose.local[skeleton.GetBoneIndex("mixamorig:Hips")].position.y;
                clip.leftPhase = time / clip.duration;
				clip.leftToeTx = pose.world[skeleton.GetBoneIndex(leftPart)];
				clip.leftToeEndTx = pose.world[skeleton.GetBoneIndex("mixamorig:LeftToe_End")];
            }
            if (prevRightY > groundY && XMVectorGetY(right) < groundY)
            {
				clip.rightDist = tmpPlayer.pose.local[skeleton.GetBoneIndex("mixamorig:Hips")].position.y;
				clip.rightTime = time;
				clip.rightPhase = time / clip.duration;
				clip.rightToeTx = pose.world[skeleton.GetBoneIndex(rightPart)];
				clip.rightToeEndTx = pose.world[skeleton.GetBoneIndex("mixamorig:RightToe_End")];
            }
			prevLeftY = XMVectorGetY(left);
			prevRightY = XMVectorGetY(right);
        }

        XMVECTOR leftToHips = XMVectorSubtract(hips, left);
        XMVECTOR rightToHips = XMVectorSubtract(hips, right);

        XMStoreFloat3(&clip.leftTargetToHipsVector[i].position, leftToHips);
        XMStoreFloat3(&clip.rightTargetToHipsVector[i].position, rightToHips);
        clip.leftTargetToHipsVector[i].time = time;
        clip.rightTargetToHipsVector[i].time = time;
    }
}

XMFLOAT3 AnimationStateManager::getLeftTargetToHips()
{
    return leftTargetToHips;
}

XMFLOAT3 AnimationStateManager::getRightTargetToHips()
{
    return rightTargetToHips;
}

void AnimationStateManager::initAnimationPlayer(int boneSize)
{
	current.pose.Initialize(boneSize);
	previous.pose.Initialize(boneSize);
	move.pose.Initialize(boneSize);
}

void AnimationStateManager::getCurrentWorldBoneTransform(Pose& pose, int idx)
{
    pose.world[idx] = current.pose.world[idx];
}

float AnimationStateManager::getLeftDistance(Skeleton& skeleton, bool isPrev)
{
	float distance = 0.0f;
	int idx = skeleton.GetBoneIndex("mixamorig:Hips");
	if (isPrev)
	{
		distance = move.clip->leftDist - previous.clip->leftDist;
		distance = distance - prevY;
		if (distance < 0.0f)
		{
			int i = move.clip->boneTracks["mixamorig:Hips"].positionKeys.size() - 1;
			float offset = move.clip->boneTracks["mixamorig:Hips"].positionKeys[i].position.y - previous.clip->boneTracks["mixamorig:Hips"].positionKeys[i].position.y;
            distance += offset;
		}
	}
	else
	{
		distance = move.clip->leftDist - current.clip->leftDist;
		distance = distance - prevY;
		if (distance < 0.0f)
		{
			int i = move.clip->boneTracks["mixamorig:Hips"].positionKeys.size() - 1;
			float offset = move.clip->boneTracks["mixamorig:Hips"].positionKeys[i].position.y - current.clip->boneTracks["mixamorig:Hips"].positionKeys[i].position.y;
			distance += offset;
		}
	}
	distance = distance * 0.02f * dampping;

	return distance;
}

float AnimationStateManager::getRightDistance(Skeleton& skeleton, bool isPrev)
{
	float distance = 0.0f;
	int idx = skeleton.GetBoneIndex("mixamorig:Hips");
	if (isPrev)
	{
		distance = move.clip->rightDist - previous.clip->rightDist;
		distance = distance - prevY;
		if (distance < 0.0f)
		{
			int i = move.clip->boneTracks["mixamorig:Hips"].positionKeys.size() - 1;
			float offset = move.clip->boneTracks["mixamorig:Hips"].positionKeys[i].position.y - previous.clip->boneTracks["mixamorig:Hips"].positionKeys[i].position.y;
			distance += offset;
		}
	}
	else
	{
		distance = move.clip->rightDist - current.clip->rightDist;
		distance = distance - prevY;
		if (distance < 0.0f)
		{
			int i = move.clip->boneTracks["mixamorig:Hips"].positionKeys.size() - 1;
			float offset = move.clip->boneTracks["mixamorig:Hips"].positionKeys[i].position.y - current.clip->boneTracks["mixamorig:Hips"].positionKeys[i].position.y;
			distance += offset;
		}
	}
	distance = distance * 0.02f * dampping;
	return distance;
}

float AnimationStateManager::getDistance(Skeleton& skeleton)
{
    float distance = 0.0f;
    int idx = skeleton.GetBoneIndex("mixamorig:Hips");
	if (previous.clip != nullptr && previous.clip->name == "walk")
	{
		distance = move.pose.local[idx].position.y - previous.pose.local[idx].position.y;
		float tmp = distance;
		prevY = distance - prevY;
		if (prevY < 0.0f)
		{
			int i = move.clip->boneTracks["mixamorig:Hips"].positionKeys.size() - 1;
			float offset = move.clip->boneTracks["mixamorig:Hips"].positionKeys[i].position.y - previous.clip->boneTracks["mixamorig:Hips"].positionKeys[i].position.y;
			prevY += offset;
		}
		distance = prevY * (1.0f - blendAlpha);
		prevY = tmp;
	}
	else if (current.clip->name == "walk")
	{
		distance = move.pose.local[idx].position.y - current.pose.local[idx].position.y;

		float tmp = distance;
		prevY = distance - prevY;
		if (prevY < 0.0f)
		{
			int i = move.clip->boneTracks["mixamorig:Hips"].positionKeys.size() - 1;
			float offset = move.clip->boneTracks["mixamorig:Hips"].positionKeys[i].position.y - current.clip->boneTracks["mixamorig:Hips"].positionKeys[i].position.y;
			prevY += offset;
		}
		distance = prevY * blendAlpha;
		prevY = tmp;
	}
	distance = distance * 0.02f * dampping;

    return distance;
}

void AnimationStateManager::setDampping(float d)
{
	dampping = d;
}

void AnimationStateManager::resetDampping()
{
	dampping = 1.0f;
}