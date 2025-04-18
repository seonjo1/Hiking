#pragma once

#include "Common.h"
#include "DirectXMath.h"
#include <vector>
#include <algorithm>

using namespace DirectX;

class RaycastingManager;
class Skeleton;
class Pose;

struct IKBone
{
	int idx;
	// x, y, z ���� (pitch, yaw, roll)
	XMFLOAT3 axis;
	float maxDeg;
	float minDeg;
	bool angleEnable[3];
	float anglePlusLimits[3];
	float angleMinusLimits[3];
};

struct IKChain
{
	std::vector<IKBone> Bones;
	XMFLOAT3 EndEffector;
	XMFLOAT3 Target;
	int EndEffectorIdx;
	int DoFNum {0};
};

struct JacobianMatrix
{
public:
	int rowCount;
	int colCount;

	std::vector<std::vector<float>> m_data;

	void Resize(int rows, int cols) {
		rowCount = rows;
		colCount = cols;
		m_data.resize(rows);
		for (int i = 0; i < rows; i++)
		{
			m_data[i].resize(cols, 0.0f);
		}
	};

	void Set(int row, int col, float value)
	{
		m_data[row][col] = value;
	}

	float Get(int row, int col) const
	{
		return m_data[row][col];
	}

	void SetZero()
	{
		for (int i = 0; i < rowCount; ++i)
		{
			for (int j = 0; j < colCount; ++j)
			{
				m_data[i][j] = 0.0f;
			}
		}
	}

	void print()
	{
		for (int i = 0; i < rowCount; ++i)
		{
			for (int j = 0; j < colCount; ++j)
			{
				p(std::to_string(m_data[i][j]) + " ");
			}
			p("\n");
		}
	}
};

enum class EChainPart
{
	LEFT_FOOT = 0,
	RIGHT_FOOT = 1,
};

class IKManager
{
public:
	void initIKChains(Skeleton& skeleton);
	void calculateTarget(Pose& pose, XMMATRIX& worldMatrix, RaycastingManager& raycastingManager);
	void calculateJacobianMatrix(Pose& pose, XMMATRIX& worldMatrix);
	void solveDLS();
	void updateNowRotation(Pose& pose);
	void updateAngle();
	bool isFinish(Pose& pose, XMMATRIX& worldMatrix);
	IKChain& getChain(int idx);
	std::vector<XMFLOAT4>& getNowRotation();

private:
	void initLeftFootChains(Skeleton& skeleton);
	void initRightFootChains(Skeleton& skeleton);
	void quaternionToEuler(const XMFLOAT4& q, float* eulerDeg);
	void clampBoneAngle(IKBone& bone, XMFLOAT4& quat);
	XMVECTOR ClampTwist(FXMVECTOR twist, FXMVECTOR twistAxis, float minDeg, float maxDeg);
	XMVECTOR ClampSwingAsymmetric(XMVECTOR swing, XMVECTOR twistAxis, float maxDeg, float minDeg);

	void DecomposeSwingTwist(XMVECTOR q, XMVECTOR twistAxis, XMVECTOR& outSwing, XMVECTOR& outTwist);
	static inline float ClampF(float v, float lo, float hi);

	JacobianMatrix J;
	JacobianMatrix JTJ;
	JacobianMatrix JTJInv;
	std::vector<float> dP;
	std::vector<float> dTheta;
	std::vector<float> JTx;
	std::vector<IKChain> m_chains;
	std::vector<XMFLOAT4> m_nowRotation;

	int m_chainNum{ 0 };
	int m_rowNum{ 0 };
	int m_colNum{ 0 };
};