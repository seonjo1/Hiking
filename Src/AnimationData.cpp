#include "AnimationData.h"

BoneTrack* AnimationClip::GetTrack(const std::string& boneName) {
    auto it = boneTracks.find(boneName);
    return (it != boneTracks.end()) ? &it->second : nullptr;
}

int Skeleton::GetBoneIndex(const std::string& name) const {
    auto it = nameToIndex.find(name);
    return (it != nameToIndex.end()) ? it->second : -1;
}

void Skeleton::SetBoneAxisAndRange(const std::string& name, XMFLOAT3 axis, float xMax, float xMin, float zMax, float zMin, float twist)
{
    Bone& bone =  bones[GetBoneIndex(name)];
    bone.axis = axis;
    bone.xMax = xMax;
	bone.xMin = xMin;
    bone.zMax = zMax;
    bone.zMin = zMin;
	bone.twist = twist;
    bone.hasAxis = true;
}
