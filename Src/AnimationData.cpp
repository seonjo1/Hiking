#include "AnimationData.h"

BoneTrack* AnimationClip::GetTrack(const std::string& boneName) {
    auto it = boneTracks.find(boneName);
    return (it != boneTracks.end()) ? &it->second : nullptr;
}

int Skeleton::GetBoneIndex(const std::string& name) const {
    auto it = nameToIndex.find(name);
    return (it != nameToIndex.end()) ? it->second : -1;
}
