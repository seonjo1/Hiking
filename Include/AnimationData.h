#pragma once

#include <DirectXMath.h>
#include <string>
#include <vector>
#include <unordered_map>

using namespace DirectX;

struct Bone {
    std::string name;
    int parentIndex = -1;
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
    double duration;        // �� Tick ��
    double ticksPerSecond;  // 1�ʴ� �� Tick���� 
    std::unordered_map<std::string, BoneTrack> boneTracks;

    BoneTrack* GetTrack(const std::string& boneName);
};
