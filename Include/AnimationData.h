#pragma once

#include <DirectXMath.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "Common.h"

using namespace DirectX;

struct Bone {
    std::string name;
    int parentIndex = -1;
    std::vector<int> children;
    XMMATRIX offsetMatrix;
};
    
class Skeleton {
public:
    std::vector<Bone> bones;
    std::unordered_map<std::string, int> nameToIndex;
    int GetBoneIndex(const std::string& name) const;
};

struct PositionKeyframe {
    double time;
    XMFLOAT3 position;
};

struct RotationKeyframe {
    double time;
    XMFLOAT4 rotation;
};

struct ScaleKeyframe {
    double time;
    XMFLOAT3 scale;
};

struct BoneTrack {
    std::string boneName;
    std::vector<PositionKeyframe> positionKeys;
    std::vector<RotationKeyframe> rotationKeys;
    std::vector<ScaleKeyframe> scaleKeys;
};

struct AnimationClip {
    std::string name;
    double duration;        // 총 Tick 수
    double ticksPerSecond;  // 1초당 몇 Tick인지 
    std::unordered_map<std::string, BoneTrack> boneTracks;

    BoneTrack* GetTrack(const std::string& boneName);
};
