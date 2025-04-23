#include "IKManager.h"
#include "AnimationData.h"
#include "Pose.h"
#include "RaycastingManager.h"

void IKManager::initLeftFootChains(Skeleton& skeleton)
{
	int idx = static_cast<int>(EChainPart::LEFT_FOOT);
	m_chains[idx].Bones.resize(4);
	m_chains[idx].DoFNum = 12;
	m_chains[idx].EndEffectorIdx = skeleton.GetBoneIndex(("mixamorig:LeftToe_End"));

	// LeftToeBase
	int boneIdx = skeleton.GetBoneIndex("mixamorig:LeftToeBase");
	m_chains[idx].Bones[0].idx = boneIdx;
	m_chains[idx].Bones[0].xMax = skeleton.bones[boneIdx].xMax;
	m_chains[idx].Bones[0].xMin = skeleton.bones[boneIdx].xMin;
	m_chains[idx].Bones[0].yMax = skeleton.bones[boneIdx].yMax;
	m_chains[idx].Bones[0].yMin = skeleton.bones[boneIdx].yMin;
	m_chains[idx].Bones[0].zMax = skeleton.bones[boneIdx].zMax;
	m_chains[idx].Bones[0].zMin = skeleton.bones[boneIdx].zMin;
	m_chains[idx].Bones[0].axis = skeleton.bones[boneIdx].axis;

	// x�� default 45��
	m_chains[idx].Bones[0].angleEnable[0] = true;
	m_chains[idx].Bones[0].angleEnable[1] = true;
	m_chains[idx].Bones[0].angleEnable[2] = true;

	// LeftFoot
	boneIdx = skeleton.GetBoneIndex("mixamorig:LeftFoot");
	m_chains[idx].Bones[1].idx = boneIdx;
	m_chains[idx].Bones[1].xMax = skeleton.bones[boneIdx].xMax;
	m_chains[idx].Bones[1].xMin = skeleton.bones[boneIdx].xMin;
	m_chains[idx].Bones[1].yMax = skeleton.bones[boneIdx].yMax;
	m_chains[idx].Bones[1].yMin = skeleton.bones[boneIdx].yMin;
	m_chains[idx].Bones[1].zMax = skeleton.bones[boneIdx].zMax;
	m_chains[idx].Bones[1].zMin = skeleton.bones[boneIdx].zMin;
	m_chains[idx].Bones[1].axis = skeleton.bones[boneIdx].axis;

	// x�� default 50��
	m_chains[idx].Bones[1].angleEnable[0] = true;
	m_chains[idx].Bones[1].angleEnable[1] = true;
	m_chains[idx].Bones[1].angleEnable[2] = true;

	// LeftLeg
	boneIdx = skeleton.GetBoneIndex("mixamorig:LeftLeg");
	m_chains[idx].Bones[2].idx = boneIdx;
	m_chains[idx].Bones[2].xMax = skeleton.bones[boneIdx].xMax;
	m_chains[idx].Bones[2].xMin = skeleton.bones[boneIdx].xMin;
	m_chains[idx].Bones[2].yMax = skeleton.bones[boneIdx].yMax;
	m_chains[idx].Bones[2].yMin = skeleton.bones[boneIdx].yMin;
	m_chains[idx].Bones[2].zMax = skeleton.bones[boneIdx].zMax;
	m_chains[idx].Bones[2].zMin = skeleton.bones[boneIdx].zMin;
	m_chains[idx].Bones[2].axis = skeleton.bones[boneIdx].axis;

	m_chains[idx].Bones[2].angleEnable[0] = true;
	m_chains[idx].Bones[2].angleEnable[1] = true;
	m_chains[idx].Bones[2].angleEnable[2] = true;

	// LeftUpLeg
	boneIdx = skeleton.GetBoneIndex("mixamorig:LeftUpLeg");
	m_chains[idx].Bones[3].idx = boneIdx;
	m_chains[idx].Bones[3].xMax = skeleton.bones[boneIdx].xMax;
	m_chains[idx].Bones[3].xMin = skeleton.bones[boneIdx].xMin;
	m_chains[idx].Bones[3].yMax = skeleton.bones[boneIdx].yMax;
	m_chains[idx].Bones[3].yMin = skeleton.bones[boneIdx].yMin;
	m_chains[idx].Bones[3].zMax = skeleton.bones[boneIdx].zMax;
	m_chains[idx].Bones[3].zMin = skeleton.bones[boneIdx].zMin;
	m_chains[idx].Bones[3].axis = skeleton.bones[boneIdx].axis;

	m_chains[idx].Bones[3].angleEnable[0] = true;
	m_chains[idx].Bones[3].angleEnable[1] = true;
	m_chains[idx].Bones[3].angleEnable[2] = true;
}

void IKManager::initRightFootChains(Skeleton& skeleton)
{
	int idx = static_cast<int>(EChainPart::RIGHT_FOOT);

	m_chains[idx].Bones.resize(4);
	m_chains[idx].DoFNum = 8;
	m_chains[idx].EndEffectorIdx = skeleton.GetBoneIndex(("mixamorig:RightToe_End"));

	// RightToeBase
	m_chains[idx].Bones[0].idx = skeleton.GetBoneIndex("mixamorig:RightToeBase");

	m_chains[idx].Bones[0].angleEnable[0] = true;
	m_chains[idx].Bones[0].anglePlusLimits[0] = 0.0f;
	m_chains[idx].Bones[0].angleMinusLimits[0] = -60.0f;

	m_chains[idx].Bones[0].angleEnable[1] = false;

	m_chains[idx].Bones[0].angleEnable[2] = false;

	// RightFoot
	m_chains[idx].Bones[1].idx = skeleton.GetBoneIndex("mixamorig:RightFoot");

	m_chains[idx].Bones[1].angleEnable[0] = true;
	m_chains[idx].Bones[1].anglePlusLimits[0] = 40.0f;
	m_chains[idx].Bones[1].angleMinusLimits[0] = -30.0f;

	m_chains[idx].Bones[1].angleEnable[1] = true;
	m_chains[idx].Bones[1].anglePlusLimits[1] = 30.0f;
	m_chains[idx].Bones[1].angleMinusLimits[1] = -30.0f;

	m_chains[idx].Bones[1].angleEnable[2] = true;
	m_chains[idx].Bones[1].anglePlusLimits[2] = 0.0f;
	m_chains[idx].Bones[1].angleMinusLimits[2] = -10.0f;

	// RightLeg
	m_chains[idx].Bones[2].idx = skeleton.GetBoneIndex("mixamorig:RightLeg");

	m_chains[idx].Bones[2].angleEnable[0] = true;
	m_chains[idx].Bones[2].anglePlusLimits[0] = 150.0f;
	m_chains[idx].Bones[2].angleMinusLimits[0] = 0.0f;

	m_chains[idx].Bones[2].angleEnable[1] = false;

	m_chains[idx].Bones[2].angleEnable[2] = false;

	// RightUpLeg
	m_chains[idx].Bones[3].idx = skeleton.GetBoneIndex("mixamorig:RightUpLeg");

	m_chains[idx].Bones[3].angleEnable[0] = true;
	m_chains[idx].Bones[3].anglePlusLimits[0] = 45.0f;
	m_chains[idx].Bones[3].angleMinusLimits[0] = -100.0f;

	m_chains[idx].Bones[3].angleEnable[1] = true;
	m_chains[idx].Bones[3].anglePlusLimits[1] = 30.0f;
	m_chains[idx].Bones[3].angleMinusLimits[1] = -30.0f;

	m_chains[idx].Bones[3].angleEnable[2] = true;
	m_chains[idx].Bones[3].anglePlusLimits[2] = 30.0f;
	m_chains[idx].Bones[3].angleMinusLimits[2] = -10.0f;
}

void IKManager::initIKChains(Skeleton& skeleton)
{
	m_chainNum = 1;
	m_rowNum = m_chainNum * 3;
	m_chains.resize(m_chainNum);
	m_nowRotation.resize(skeleton.bones.size(), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	initLeftFootChains(skeleton);
	//initRightFootChains(skeleton);
	
	for (int i = 0; i < m_chainNum; i++)
	{
		m_colNum += m_chains[i].DoFNum;
	}

	// left chain�� ����
	int M = m_rowNum;
	int N = m_colNum;

	J.Resize(M, N);			// M X N
	JTJ.Resize(N, N);		// N x N
	JTJInv.Resize(N, N);	// N x N
	dP.resize(M, 0.0f);		// M x 1
	JTx.resize(N, 0.0f);	// N X 1
	dTheta.resize(N, 0.0f);	// N X 1
}

void IKManager::calculateTarget(Pose& pose, XMMATRIX& worldMatrix, RaycastingManager& raycastingManager)
{
	// endEffector ���
	for (int i = 0; i < m_chainNum; i++)
	{
		int boneIdx = m_chains[i].EndEffectorIdx;
		XMVECTOR endEffector = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMMATRIX transform = XMMatrixMultiply(pose.world[boneIdx], worldMatrix);
		endEffector = XMVector4Transform(endEffector, transform);
		XMStoreFloat3(&m_chains[i].EndEffector, endEffector);
		// target ���
		m_chains[i].Target = raycastingManager.m_LeftFoot.pos;
	}
}

void IKManager::calculateJacobianMatrix(Pose& pose, XMMATRIX& worldMatrix)
{
	static const XMFLOAT3 axes[3] = { XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) };
	for (int i = 0; i < m_chainNum; ++i)
	{
		int colIdx = 0;
		int boneNum = m_chains[i].Bones.size();
		
		XMVECTOR effectPos = XMLoadFloat3(&m_chains[i].EndEffector);
		XMVECTOR targetPos = XMLoadFloat3(&m_chains[i].Target);
		XMVECTOR toTarget = XMVectorSubtract(targetPos, effectPos);

		dP[i * 3] = XMVectorGetX(toTarget);
		dP[i * 3 + 1] = XMVectorGetY(toTarget);
		dP[i * 3 + 2] = XMVectorGetZ(toTarget);

		for (int j = 0; j < boneNum; ++j)
		{
			IKBone& bone = m_chains[i].Bones[j];
			XMVECTOR bonePos = XMVectorZero();
			XMMATRIX transform = XMMatrixMultiply(pose.world[bone.idx], worldMatrix);
			bonePos = XMVector3TransformCoord(bonePos, transform);

			XMFLOAT3 debugBonePose;
			XMStoreFloat3(&debugBonePose, bonePos);

			XMVECTOR toEffector = XMVectorSubtract(effectPos, bonePos);
			XMFLOAT3 debugToEffector;
			XMStoreFloat3(&debugToEffector, toEffector);

			//p("axse\n");
			for (int k = 0; k < 3; ++k)
			{
				const XMFLOAT3& axis = axes[k];

				// �ش� axis�� �������� ������ continue
				if (bone.angleEnable[k] == false)
				{
					continue;
				}

				// ���� ȸ���� �� ���� ȸ����
				XMVECTOR localAxis = XMLoadFloat3(&axis);
				XMVECTOR worldAxis = XMVector3TransformNormal(localAxis, transform);
				worldAxis = XMVector3Normalize(worldAxis);

				XMFLOAT3 debugAxis;
				XMStoreFloat3(&debugAxis, worldAxis);


				// ��ġ ��ȭ = ȸ���� �� effector ����
				XMVECTOR dPdTheta = XMVector3Cross(worldAxis, toEffector);

				// Jacobian ��Ŀ� �� ����
				J.Set(i * 3, colIdx, XMVectorGetX(dPdTheta)); // dx/d��
				J.Set(i * 3 + 1, colIdx, XMVectorGetY(dPdTheta)); // dy/d��
				J.Set(i * 3 + 2, colIdx, XMVectorGetZ(dPdTheta)); // dz/d��
				++colIdx;
			}
		}
	}
}

void IKManager::solveDLS()
{
	static const float lambda = 1.0f;
	static const float thetaAlpha = 1.0f;
	float w[12] = { 
		0.1f, 0.01f, 0.1f,
		0.1f, 0.01f, 0.1f,
		6.0f, 0.01f, 0.1f,
		6.0f, 0.01f, 0.1f
	};

	// JTJ ���ϱ�
	JTJ.SetZero();
	for (int i = 0; i < m_colNum; ++i)
	{
		for (int j = 0; j < m_colNum; ++j)
		{
			float sum = 0.0f;
			for (int k = 0; k < m_rowNum; ++k)
			{
				sum += J.Get(k, i) * J.Get(k, j);
			}
			JTJ.Set(i, j, sum * w[i]);
		}
	}


	// JTJ + lambda^2 * I (���� �� �߰�)
	for (int i = 0; i < m_colNum; ++i)
	{
		JTJ.Set(i, i, JTJ.Get(i, i) + lambda * lambda);
	}

	// JT * dP ���
	for (int i = 0; i < m_colNum; ++i)
	{
		JTx[i] = 0.0f;
		for (int j = 0; j < m_rowNum; ++j)
		{
			JTx[i] += w[i] * J.Get(j, i) * dP[j];
		}
	}

	// JTJ ����� ��� (���콺-���� �ҰŹ�)
	JTJInv.SetZero();
	for (int i = 0; i < m_colNum; ++i)
	{
		JTJInv.Set(i, i, 1.0f);
	}
	// JTJ ���� ���߿� ������ JTJ�� �����ؼ� �Ʒ��� ������Ѿ��� (JTJ �� �ı�)
	for (int i = 0; i < m_colNum; ++i)
	{
		float pivot = JTJ.Get(i, i);

		// singular ��ȣ
		if (fabs(pivot) < 1e-6f)
		{
			continue;
		}

		for (int j = 0; j < m_colNum; ++j)
		{
			JTJ.Set(i, j, JTJ.Get(i, j) / pivot);
			JTJInv.Set(i, j, JTJInv.Get(i, j) / pivot);
		}

		for (int j = 0; j < m_colNum; ++j)
		{
			if (i == j)
			{
				continue;
			}

			float factor = JTJ.Get(j, i);

			for (int k = 0; k < m_colNum; ++k)
			{
				JTJ.Set(j, k, JTJ.Get(j, k) - JTJ.Get(i, k) * factor);
				JTJInv.Set(j, k, JTJInv.Get(j, k) - JTJInv.Get(i, k) * factor);
			}
		}
	}

	// JTJInv * JTx �� ���� dTheta ���

	for (int i = 0; i < m_colNum; ++i)
	{
		dTheta[i] = 0.0f;
		for (int j = 0; j < m_colNum; ++j)
		{
			dTheta[i] += JTJInv.Get(i, j) * JTx[j];
		}
		dTheta[i] = dTheta[i] * thetaAlpha;
		//p("dTheta[" + std::to_string(i) + "]" + std::to_string(dTheta[i]) + "\n");
	}
}

void IKManager::updateNowRotation(Pose& pose)
{
	for (int i = 0; i < pose.count; ++i)
	{
		m_nowRotation[i] = pose.local[i].rotation;
	}
}

void IKManager::updateAngle()
{
	for (int i = 0; i < m_chainNum; ++i)
	{
		int idx = 0;
		int count = m_chains[i].Bones.size();
		for (int j = 0; j < count; ++j)
		{
			IKBone& bone = m_chains[i].Bones[j];

			float angle[3] = { 0.0f, 0.0f, 0.0f };

			// ���� quaternion�� pitch yaw roll�� ��ȯ
			
			for (int k = 0; k < 3; ++k)
			{
				if (bone.angleEnable[k] == true)
				{
					angle[k] = XMConvertToDegrees(dTheta[idx]);
					idx++;
				}
			}

			XMVECTOR qOld = XMLoadFloat4(&m_nowRotation[bone.idx]);
			 //���� �� ���ϱ�
			XMVECTOR localX = XMVectorSet(1, 0, 0, 0);
			XMVECTOR localY = XMVectorSet(0, 1, 0, 0);
			XMVECTOR localZ = XMVectorSet(0, 0, 1, 0);

			//// ���� �� ���� ��Ÿ ���ʹϾ�
			XMVECTOR dqX = XMQuaternionRotationAxis(localX, XMConvertToRadians(angle[0]));
			XMVECTOR dqY = XMQuaternionRotationAxis(localY, XMConvertToRadians(angle[1]));
			XMVECTOR dqZ = XMQuaternionRotationAxis(localZ, XMConvertToRadians(angle[2]));

			// �ռ� �� ����
			XMVECTOR newQuat = XMQuaternionMultiply(dqZ, XMQuaternionMultiply(dqY, dqX));
			XMVECTOR qNew = XMQuaternionNormalize(XMQuaternionMultiply(newQuat, qOld));
			XMStoreFloat4(&m_nowRotation[bone.idx], qNew);

			// clamping
			clampBoneAngle(bone, m_nowRotation[bone.idx]);

		}

	}
}

bool IKManager::isFinish(Pose& pose, XMMATRIX& worldMatrix)
{
	const float THRESHOLD = 0.1f;

	bool success = true;
	for (int i = 0; i < m_chainNum; i++)
	{
		int boneIdx = m_chains[i].EndEffectorIdx;
		XMVECTOR endEffector = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMMATRIX transform = XMMatrixMultiply(pose.world[boneIdx], worldMatrix);
		endEffector = XMVector3TransformCoord(endEffector, transform);
		XMVECTOR target = XMLoadFloat3(&(m_chains[i].Target));
		XMVECTOR diffVec = XMVectorSubtract(target, endEffector);
		XMVECTOR lengthVec = XMVector3LengthSq(diffVec);
		
		float length;
		XMStoreFloat(&length, lengthVec);
		if (length > THRESHOLD)
		{
			success = false;
		}
	}
	return success;
}

IKChain& IKManager::getChain(int idx)
{
	return m_chains[idx];
}

std::vector<XMFLOAT4>& IKManager::getNowRotation()
{
	return m_nowRotation;
}

// X -> Y -> Z ����
void IKManager::quaternionToEuler(const XMFLOAT4& q, float* eulerDeg)
{
	float sqw = q.w * q.w;
	float sqx = q.x * q.x;
	float sqy = q.y * q.y;
	float sqz = q.z * q.z;


	eulerDeg[0] = XMConvertToDegrees(asinf(2.0f * (q.w * q.x - q.y * q.z)));  // Pitch
	eulerDeg[1] = XMConvertToDegrees(atan2f(2.0f * (q.x * q.z + q.w * q.y), (-sqx - sqy + sqz + sqw)));  // Yaw
	eulerDeg[2] = XMConvertToDegrees(atan2f(2.0f * (q.x * q.y + q.w * q.z), (-sqx + sqy - sqz + sqw)));  // Roll
}

void IKManager::DecomposeSwingTwist(XMVECTOR q, XMVECTOR twistAxis, XMVECTOR& outSwing, XMVECTOR& outTwist)
{
	// 1. twist �� ����ȭ
	twistAxis = XMVector3Normalize(twistAxis);

	// 2. q �� ���� �κ��� twist �࿡ �翵 (���ʹϾ��� x, y, z�� �ٸ� �࿡ �����ϸ� �࿡���� ȸ���� �и��� �� ����)
	XMVECTOR qVec = XMVectorSet(XMVectorGetX(q), XMVectorGetY(q), XMVectorGetZ(q), 0.0f);
	float dotVT = XMVectorGetX(XMVector3Dot(qVec, twistAxis));
	XMVECTOR proj = XMVectorScale(twistAxis, dotVT);

	// 3. twist ���ʹϾ� (proj xyz + q.w) ����ȭ
	outTwist = XMQuaternionNormalize(
		XMVectorSet(
			XMVectorGetX(proj),
			XMVectorGetY(proj),
			XMVectorGetZ(proj),
			XMVectorGetW(q)
		)
	);

	// 4. swing = q * twist^-1 (���� ȸ������ twist ȸ���� �� ���� swing���� ����)
	outSwing = XMQuaternionNormalize(XMQuaternionMultiply(q, XMQuaternionInverse(outTwist)));
}

XMVECTOR IKManager::ClampTwist(FXMVECTOR twist, FXMVECTOR twistAxis, float yMax, float yMin)
{
	float qw = XMVectorGetW(twist);
	float angle = 2.0f * acosf(std::clamp(qw, -1.0f, 1.0f));
	//p("before y axis angle: " + std::to_string(XMConvertToDegrees(angle)) + "\n");

	if (angle >= XM_2PI)
	{
		angle -= XM_2PI;
	}
	if (angle <= -XM_2PI)
	{
		angle += XM_2PI;
	}

	//p("after y axis angle: " + std::to_string(XMConvertToDegrees(angle)) + "\n");
	// clamp
	float deg = std::clamp(XMConvertToDegrees(angle), yMin, yMax);
	float rad = XMConvertToRadians(deg);
	//p("yMax: " + std::to_string(yMax) + "\n");
	//p("yMin: " + std::to_string(yMin) + "\n");
	//p("Clamped y axis angle: " + std::to_string(deg) + "\n");

	return XMQuaternionRotationAxis(twistAxis, rad);
}

void IKManager::clampBoneAngle(IKBone& bone, XMFLOAT4& quat)
{
	// IK�� ���� ���� ȸ�� ���ʹϾ�
	XMVECTOR qIK = XMLoadFloat4(&quat);

	// local �� ����
	XMVECTOR twistAxis = XMLoadFloat3(&bone.axis);

	float xMax = bone.xMax;
	float xMin = bone.xMin;
	float yMax = bone.yMax;
	float yMin = bone.yMin;
	float zMax = bone.zMax;
	float zMin = bone.zMin;

	XMVECTOR swing, twist;

	// ���� quat�� Swing�� Twist quaternion���� �и�
	DecomposeSwingTwist(qIK, twistAxis, swing, twist);

	std::vector<XMVECTOR> polygon;
	XMVECTOR swingClamped, twistClamped;

	// Twist ȸ�� clamping
	twistClamped = ClampTwist(twist, twistAxis, yMax, yMin);
	twistClamped = XMQuaternionNormalize(twistClamped);

	// Swing ȸ�� clamping
	makePolygon(twistClamped, polygon, xMax, xMin, zMax, zMin);
	swingClamped = ClampSwingBySphericalPolygon(swing, twistAxis, polygon);
	swingClamped = XMQuaternionNormalize(swingClamped);


	XMVECTOR qFinal;
	qFinal = XMQuaternionMultiply(swingClamped, twistClamped);

	XMStoreFloat4(&quat, XMQuaternionNormalize(qFinal));
}


float IKManager::SafeACos(float x)
{
	return acosf(std::clamp(x, -1.0f, 1.0f));
}

XMVECTOR IKManager::ClampDirectionToSphericalPolygon(XMVECTOR D, const std::vector<XMVECTOR>& polygon)
{
	D = XMVector3Normalize(D);
	float maxDot = -1.1f;
	XMVECTOR closest = D;

	size_t count = polygon.size() - 1;
	for (size_t i = 0; i < count; ++i)
	{
		XMVECTOR a = polygon[i];
		XMVECTOR b = polygon[i + 1];

		XMVECTOR edgeNormal = XMVector3Normalize(XMVector3Cross(b, a));
		float side = XMVectorGetX(XMVector3Dot(edgeNormal, D));

		if (side > 0.0f) // �ٱ���
		{
			float dotEN = XMVectorGetX(XMVector3Dot(D, edgeNormal));
			XMVECTOR proj = XMVector3Normalize(D - edgeNormal * dotEN);

			XMVECTOR AtoB = XMVector3Cross(a, b);
			XMVECTOR AtoProj = XMVector3Cross(a, proj);
			XMVECTOR ProjtoB = XMVector3Cross(proj, b);

			float AtoProjDot = XMVectorGetX(XMVector3Dot(AtoB, AtoProj));
			float ProjtoBDot = XMVectorGetX(XMVector3Dot(AtoB, ProjtoB));

			
			XMVECTOR candidate;
			if (AtoProjDot >= 0.0f && ProjtoBDot >= 0.0f)
			{
				candidate = proj;
			}
			else
			{
				float dotA = XMVectorGetX(XMVector3Dot(D, a));
				float dotB = XMVectorGetX(XMVector3Dot(D, b));
				candidate = (dotA > dotB) ? a : b;
			}

			float dot = XMVectorGetX(XMVector3Dot(D, candidate));
			if (dot > maxDot)
			{
				maxDot = dot;
				closest = candidate;
			}
		}
	}

	return closest;
}

XMVECTOR IKManager::ClampSwingBySphericalPolygon(XMVECTOR swing, XMVECTOR twistAxis, const std::vector<XMVECTOR>& polygon)
{
	twistAxis = XMVector3Normalize(twistAxis);
	XMVECTOR D = XMVector3Rotate(twistAxis, swing);  // swing�� ���� �̵��� ����
	XMVECTOR D_clamped = ClampDirectionToSphericalPolygon(D, polygon);
	p("D: " + std::to_string(XMVectorGetX(D)) + " " + std::to_string(XMVectorGetY(D)) + " " + std::to_string(XMVectorGetZ(D)) + "\n");
	p("D_clampe: " + std::to_string(XMVectorGetX(D_clamped)) + " " + std::to_string(XMVectorGetY(D_clamped)) + " " + std::to_string(XMVectorGetZ(D_clamped)) + "\n");

	float xAngle, zAngle;

	XMVECTOR Y = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR dXY = XMVector2Normalize(XMVectorSet(XMVectorGetX(D_clamped), XMVectorGetY(D_clamped), 0.0f, 0.0f));
	zAngle = acosf(std::abs(XMVectorGetX(XMVector2Dot(Y, dXY))));

	bool sign = 1.0f;
	if (XMVectorGetY(D_clamped) < 0.0f) { sign = -sign; }
	if (XMVectorGetX(D_clamped) < 0.0f) { sign = -sign; }

	XMVECTOR qzInv = XMQuaternionRotationAxis(XMVectorSet(0, 0, -1, 0), zAngle * sign);
	XMMATRIX qzInvTransform = XMMatrixRotationQuaternion(qzInv);
	XMVECTOR D_ZRotataed = XMVector3TransformNormal(D_clamped, qzInvTransform);

	XMVECTOR dZY = XMVector2Normalize(XMVectorSet(XMVectorGetZ(D_ZRotataed), XMVectorGetY(D_ZRotataed), 0.0f, 0.0f));
	xAngle = acosf(XMVectorGetX(XMVector2Dot(Y, dZY)));

	if (XMVectorGetZ(D_ZRotataed) < 0.0f) { xAngle = -xAngle; }

	p("xAngle: " + std::to_string(xAngle) + "\n");
	p("zAngle: " + std::to_string(zAngle) + "\n");

	XMVECTOR qx = XMQuaternionRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), xAngle);
	XMVECTOR qz = XMQuaternionRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), zAngle);

	XMVECTOR clampedSwing = XMQuaternionMultiply(qz, qx);  // x ����, z ����

	return clampedSwing;
}

void IKManager::makePolygon(FXMVECTOR twistClamped, std::vector<XMVECTOR>& polygon, float xMax, float xMin, float zMax, float zMin)
{
	int numSegments = 4;

	for (int i = 0; i < numSegments + 1; ++i)
	{
		float t = (float)i / numSegments;  // 0 ~ 1
		float xDeg = Lerp(xMin, xMax, (cosf(t * XM_2PI) + 1.0f) * 0.5f);
		float zDeg = Lerp(zMin, zMax, (sinf(t * XM_2PI) + 1.0f) * 0.5f);

		XMVECTOR qx = XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), XMConvertToRadians(xDeg));
		XMVECTOR qz = XMQuaternionRotationAxis(XMVectorSet(0, 0, -1, 0), XMConvertToRadians(zDeg));
		XMVECTOR swing = XMQuaternionMultiply(qz, qx);  // x ����, z ����

		XMVECTOR D = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), qx);
		D = XMVector3Rotate(D, qz);
		XMVECTOR D_rotated = XMVector3Rotate(D, twistClamped);

		//p("i : " + std::to_string(i) + "\n");
		//p("D_rotated: " + std::to_string(XMVectorGetX(D_rotated)) + " " + std::to_string(XMVectorGetY(D_rotated)) + " " + std::to_string(XMVectorGetZ(D_rotated)) + "\n");

		polygon.push_back(XMVector3Normalize(D_rotated));
	}
}

// X -> Y -> Z ���� quat�� x, y, z�� ��ȣ�� �ݴ�� ����
//void IKManager::quaternionToEuler(const XMFLOAT4& q, float* eulerDeg)
//{
//	XMVECTOR quat = XMLoadFloat4(&q);
//	XMMATRIX m = XMMatrixRotationQuaternion(quat);
//
//	XMFLOAT4X4 mat;
//	XMStoreFloat4x4(&mat, m);
//
//	float pitch, yaw, roll;
//
//	if (fabsf(mat._23) < 0.9999f)
//	{
//		pitch = asinf(-mat._23);              // X
//		yaw = atan2f(mat._13, mat._33);     // Y
//		roll = atan2f(mat._21, mat._22);     // Z
//	}
//	else
//	{
//		pitch = (mat._23 < 0.0f) ? XM_PIDIV2 : -XM_PIDIV2;
//		yaw = atan2f(-mat._31, mat._11);
//		roll = 0.0f;
//	}
//
//	eulerDeg[0] = XMConvertToDegrees(pitch); // X
//	eulerDeg[1] = XMConvertToDegrees(yaw);   // Y
//	eulerDeg[2] = XMConvertToDegrees(roll);  // Z
//}
