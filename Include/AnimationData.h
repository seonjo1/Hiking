#pragma once

#include <DirectXMath.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "Common.h"
#include "PhysicsManager.h"
#include "PhysicsObject.h"

using namespace DirectX;

struct Bone {
	std::string name;
	std::vector<int> children;

	int parentIndex = -1;
	bool hasAxis{ false };
	float xMax{ 0.0f };
    float xMin{ 0.0f };
    float zMax{ 0.0f };
	float zMin{ 0.0f };
	float twist{ 0.0f };
	XMFLOAT3 axis{ 0.0f, 0.0f, 0.0f };
	XMMATRIX offsetMatrix;  // 기본 transform의 역함수
};

class Skeleton {
public:
    std::vector<Bone> bones;
    std::unordered_map<std::string, int> nameToIndex;
    int rootBoneIdx;
	bool isRagdollActive = false;
	
	int GetBoneIndex(const std::string& name) const;
    void SetBoneAxisAndRange(const std::string& name, XMFLOAT3 axis, float xMax, float xMin, float zMax, float zMin, float twist);
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
	float minLeftFootOffset;
	float minRightFootOffset;
	std::vector<PositionKeyframe> leftTargetToHipsVector;
	std::vector<PositionKeyframe> rightTargetToHipsVector;
	std::unordered_map<std::string, BoneTrack> boneTracks;

	XMMATRIX leftToeTx;
    XMMATRIX rightToeTx;
	XMMATRIX leftToeEndTx;
	XMMATRIX rightToeEndTx;
	float leftPhase;
	float leftDist;
	float leftTime;
	float rightPhase;
	float rightDist;
	float rightTime;
    BoneTrack* GetTrack(const std::string& boneName);
};
