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
    bool hasAxis { false };
    XMFLOAT3 axis { 0.0f, 0.0f, 0.0f };
    float limit { 0.0f };
};
    
class Skeleton {
public:
    std::vector<Bone> bones;
    std::unordered_map<std::string, int> nameToIndex;
    int rootBoneIdx;
    int GetBoneIndex(const std::string& name) const;
    void SetBoneAxisAndRange(const std::string& name, XMFLOAT3 axis, float limit);
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
    double duration;        // �� Tick ��
    double ticksPerSecond;  // 1�ʴ� �� Tick���� 
    std::unordered_map<std::string, BoneTrack> boneTracks;

    BoneTrack* GetTrack(const std::string& boneName);
};
