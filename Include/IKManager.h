#pragma once

#include "Common.h"
#include "DirectXMath.h"
#include "RaycastingManager.h"
#include <vector>
#include <algorithm>

using namespace DirectX;

class RaycastingManager;
class Skeleton;
class Pose;

struct IKAngleBuffer
{
	float xMax;
	float xMin;
	float zMax;
	float zMin;
};

struct IKBone
{
	int idx;
	int parentsIdx;
	int childIdx;
	// x, y, z ¼ø¼­ (pitch, yaw, roll)
	XMFLOAT3 axis;
	float xMax;
	float xMin;
	float zMax;
	float zMin;
	XMVECTOR twist;
	IKAngleBuffer angleBuffer;
	XMFLOAT3 worldDest;
	XMFLOAT3 worldChildDest;
};

struct IKChain
{
	std::vector<IKBone> Bones;
	XMFLOAT3 EndEffector;
	XMFLOAT3 Target;
	XMFLOAT3 Normal;
	int EndEffectorIdx;
	int DoFNum {0};
	int clampingIdx;
	bool start;
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
	static XMVECTOR getQuatFromTo(XMFLOAT3 from, XMFLOAT3 to);
	static float safeAcosf(float dotResult);

	void initIKChains(Skeleton& skeleton);
	void resetValuesForIK(RaycastingManager& raycastingManager, Skeleton& skeleton, float walkPhase, Pose& pose, XMMATRIX& worldMatrix);
	void calculateTarget(Pose& pose, XMMATRIX& worldMatrix, RaycastingManager& raycastingManager);
	void calculateJacobianMatrix(Pose& pose, XMMATRIX& worldMatrix);
	void solveDLS();
	void updateNowRotation(Pose& pose);
	void updateAngle(Pose& pose, XMMATRIX& worldMatrix, Skeleton& skeleton);
	bool isFinish(Pose& pose, XMMATRIX& worldMatrix);
	IKChain& getChain(int idx);
	void blendingIKRotation();
	std::vector<XMFLOAT4>& getNowRotation();

private:
	void initLeftFootChains(Skeleton& skeleton);
	void initRightFootChains(Skeleton& skeleton);
	void quaternionToEuler(const XMFLOAT4& q, float* eulerDeg);
	void clampBoneAngle(IKBone& bone, XMFLOAT4& quat, Pose& pose);
	XMVECTOR ClampSwingBySphericalPolygon(XMVECTOR& swing, XMVECTOR& twistClamped, XMVECTOR twistAxis, const std::vector<XMVECTOR>& polygon);
	XMVECTOR ClampDirectionToSphericalPolygon(XMVECTOR D, const std::vector<XMVECTOR>& polygon);
	void makePolygon(std::vector<XMVECTOR>& polygon, XMVECTOR& twistClamped, float xMax, float xMin, float zMax, float zMin);
	void leftFootChainBufferUpdate(IKChain& chain, bool start);
	void rightFootChainBufferUpdate(IKChain& chain, bool start);
	void initLeftFootChainInfo(RaycastingManager& raycastingManager, Skeleton& skeleton, Pose& pose, XMMATRIX& worldMatrix);
	XMVECTOR divideQuaternionToYXZ(XMVECTOR& D, XMVECTOR& twist);
	XMFLOAT4 recreateD(IKBone& bone, XMVECTOR& D, Pose& pose);
	void initRightFootChainInfo(RaycastingManager& raycastingManager, Skeleton& skeleton, Pose& pose, XMMATRIX& worldMatrix);


	JacobianMatrix J;
	JacobianMatrix JTJ;
	JacobianMatrix JTJInv;
	std::vector<float> W;
	std::vector<float> dP;
	std::vector<float> dTheta;
	std::vector<float> JTx;
	std::vector<IKChain> m_chains;
	std::vector<XMFLOAT4> m_nowRotation;
	std::vector<XMFLOAT4> m_prevRotation;

	int m_chainNum{ 0 };
	int m_rowNum{ 0 };
	int m_colNum{ 0 };
};