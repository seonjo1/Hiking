#include "AnimationData.h"

BoneTrack* AnimationClip::GetTrack(const std::string& boneName) {
    auto it = boneTracks.find(boneName);
    return (it != boneTracks.end()) ? &it->second : nullptr;
}

int Skeleton::GetBoneIndex(const std::string& name) const {
    auto it = nameToIndex.find(name);
    return (it != nameToIndex.end()) ? it->second : -1;
}

void Skeleton::SetBoneAxisAndRange(const std::string& name, XMFLOAT3 axis, float xMax, float xMin, float yMax, float yMin, float zMax, float zMin)
{
    Bone& bone =  bones[GetBoneIndex(name)];
    bone.axis = axis;
    bone.xMax = xMax;
	bone.xMin = xMin;
	bone.yMax = yMax;
	bone.yMin = yMin;
    bone.zMax = zMax;
    bone.zMin = zMin;
    bone.hasAxis = true;
}
