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
	m_chains[idx].Bones[0].idx = skeleton.GetBoneIndex("mixamorig:LeftToeBase");
	
	// x�� default 45��
	m_chains[idx].Bones[0].angleEnable[0] = true;
	m_chains[idx].Bones[0].anglePlusLimits[0] = -40.0f;
	m_chains[idx].Bones[0].angleMinusLimits[0] = -60.0f;

	m_chains[idx].Bones[0].angleEnable[1] = true;
	m_chains[idx].Bones[0].anglePlusLimits[1] = 10.0f;
	m_chains[idx].Bones[0].angleMinusLimits[1] = -10.0f;

	m_chains[idx].Bones[0].angleEnable[2] = true;
	m_chains[idx].Bones[0].anglePlusLimits[2] = 10.0f;
	m_chains[idx].Bones[0].angleMinusLimits[2] = -10.0f;

	// LeftFoot
	m_chains[idx].Bones[1].idx = skeleton.GetBoneIndex("mixamorig:LeftFoot");

	// x�� default 50��
	m_chains[idx].Bones[1].angleEnable[0] = true;
	m_chains[idx].Bones[1].anglePlusLimits[0] = -30.0f;
	m_chains[idx].Bones[1].angleMinusLimits[0] = -60.0f;

	m_chains[idx].Bones[1].angleEnable[1] = true;
	m_chains[idx].Bones[1].anglePlusLimits[1] = 30.0f;
	m_chains[idx].Bones[1].angleMinusLimits[1] = -30.0f;

	m_chains[idx].Bones[1].angleEnable[2] = true;
	m_chains[idx].Bones[1].anglePlusLimits[2] = 0.0f;
	m_chains[idx].Bones[1].angleMinusLimits[2] = -10.0f;

	// LeftLeg
	m_chains[idx].Bones[2].idx = skeleton.GetBoneIndex("mixamorig:LeftLeg");
	
	m_chains[idx].Bones[2].angleEnable[0] = true;
	m_chains[idx].Bones[2].anglePlusLimits[0] = 150.0f;
	m_chains[idx].Bones[2].angleMinusLimits[0] = 0.0f;

	m_chains[idx].Bones[2].angleEnable[1] = true;
	m_chains[idx].Bones[2].anglePlusLimits[1] = 10.0f;
	m_chains[idx].Bones[2].angleMinusLimits[1] = -10.0f;

	m_chains[idx].Bones[2].angleEnable[2] = true;
	m_chains[idx].Bones[2].anglePlusLimits[2] = 10.0f;
	m_chains[idx].Bones[2].angleMinusLimits[2] = -10.0f;

	// LeftUpLeg
	m_chains[idx].Bones[3].idx = skeleton.GetBoneIndex("mixamorig:LeftUpLeg");

	m_chains[idx].Bones[3].angleEnable[0] = true;
	m_chains[idx].Bones[3].anglePlusLimits[0] = 100.0f;
	m_chains[idx].Bones[3].angleMinusLimits[0] = -45.0f;

	m_chains[idx].Bones[3].angleEnable[1] = true;
	m_chains[idx].Bones[3].anglePlusLimits[1] = 30.0f;
	m_chains[idx].Bones[3].angleMinusLimits[1] = -30.0f;

	// z�� default -170��
	m_chains[idx].Bones[3].angleEnable[2] = true;
	m_chains[idx].Bones[3].anglePlusLimits[2] = -160.0f;
	m_chains[idx].Bones[3].angleMinusLimits[2] = -180.0f;
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
		//p("endEffector : " + std::to_string(m_chains[i].EndEffector.x) + " " + std::to_string(m_chains[i].EndEffector.y) + " " + std::to_string(m_chains[i].EndEffector.z) + "\n");
		//p("target : " + std::to_string(m_chains[i].Target.x) + " " + std::to_string(m_chains[i].Target.y) + " " + std::to_string(m_chains[i].Target.z) + "\n");
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
			//p("bone[" + std::to_string(bone.idx) + "]\n");
			//p("Pose: " + std::to_string(debugBonePose.x) + " " + std::to_string(debugBonePose.y) + " " + std::to_string(debugBonePose.z) + "\n");

			XMVECTOR toEffector = XMVectorSubtract(effectPos, bonePos);
			XMFLOAT3 debugToEffector;
			XMStoreFloat3(&debugToEffector, toEffector);
			//p("toEffector: " + std::to_string(debugToEffector.x) + " " + std::to_string(debugToEffector.y) + " " + std::to_string(debugToEffector.z) + "\n");

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
				//p("Axis[" + std::to_string(k) + "]: " + std::to_string(debugAxis.x) + " " + std::to_string(debugAxis.y) + " " + std::to_string(debugAxis.z) + "\n");


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

	// debuging
	//p("print J!!\n");
	//J.print();
	//p("print dP!!\n");
	//for (int i = 0; i < dP.size(); i++)
	//{
	//	p(std::to_string(dP[i]) + " ");
	//}
	//p("\n");
}

void IKManager::solveDLS()
{
	static const float lambda = 1.0f;
	static const float thetaAlpha = 0.5f;
	float w[12] = { 
		0.1f, 0.1f, 0.1f,
		0.1f, 0.1f, 0.1f,
		1.5f, 0.1f, 0.1f,
		1.5f, 0.1f, 0.1f
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

	// TEST
	//p("TEST\n");
	//float a[3] = { 0.0f, 0.0f, 0.0f };
	//XMVECTOR quat = XMQuaternionRotationRollPitchYaw(
	//	XMConvertToRadians(35.0f),
	//	XMConvertToRadians(-30.0f),
	//	XMConvertToRadians(130.0f)
	//);
	//XMFLOAT4 q1;
	//XMStoreFloat4(&q1, quat);
	//quaternionToEuler(q1, a);
	//p("before q: " + std::to_string(q1.x) + " " + std::to_string(q1.y) + " " + std::to_string(q1.z) + " " + std::to_string(q1.w) + "\n");
	//p("before a: " + std::to_string(a[0]) + " " + std::to_string(a[1]) + " " + std::to_string(a[2]) + "\n");
	//quat = XMQuaternionRotationRollPitchYaw(
	//	XMConvertToRadians(a[0]),
	//	XMConvertToRadians(a[1]),
	//	XMConvertToRadians(a[2])
	//);
	//XMStoreFloat4(&q1, quat);
	//quaternionToEuler(q1, a);
	//p("after q: " + std::to_string(q1.x) + " " + std::to_string(q1.y) + " " + std::to_string(q1.z) + " " + std::to_string(q1.w) + "\n");
	//p("after a: " + std::to_string(a[0]) + " " + std::to_string(a[1]) + " " + std::to_string(a[2]) + "\n");
	
	for (int i = 0; i < m_chainNum; ++i)
	{
		int idx = 0;
		int count = m_chains[i].Bones.size();
		for (int j = 0; j < count; ++j)
		{
			IKBone& bone = m_chains[i].Bones[j];

			float debugAngle[3] = { 0.0f, 0.0f, 0.0f };
			quaternionToEuler(m_nowRotation[bone.idx], debugAngle);
			p("before bone[" + std::to_string(bone.idx) + "]\n");
			p("x: " + std::to_string(debugAngle[0]) + " " + std::to_string(debugAngle[1]) + " " + std::to_string(debugAngle[2]) + "\n");
			
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

			p("angle: " + std::to_string(angle[0]) + " " + std::to_string(angle[1]) + " " + std::to_string(angle[2]) + "\n");

			XMVECTOR quat = XMQuaternionRotationRollPitchYaw(
				XMConvertToRadians(angle[0]),
				XMConvertToRadians(angle[1]),
				XMConvertToRadians(angle[2])
			);

			XMVECTOR nowQuat = XMLoadFloat4(&m_nowRotation[bone.idx]);
			XMVECTOR newQuat = XMQuaternionMultiply(quat, nowQuat);
			XMStoreFloat4(&m_nowRotation[bone.idx], newQuat);
			quaternionToEuler(m_nowRotation[bone.idx], debugAngle);
			p("before clamping\n");
			p("x: " + std::to_string(debugAngle[0]) + " " + std::to_string(debugAngle[1]) + " " + std::to_string(debugAngle[2]) + "\n");
			// clamping
			clampBoneAngle(bone, m_nowRotation[bone.idx]);

			// debuging
			quaternionToEuler(m_nowRotation[bone.idx], angle);
			p("after bone[" + std::to_string(bone.idx) + "]\n");
			p("x: " + std::to_string(angle[0]) + " " + std::to_string(angle[1]) + " " + std::to_string(angle[2]) + "\n");
		}
		p("\n");

	}
}

bool IKManager::isFinish(Pose& pose, XMMATRIX& worldMatrix)
{
	const float THRESHOLD = 0.001f;

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

inline float IKManager::ClampF(float v, float lo, float hi) {
	return v < lo ? lo : (v > hi ? hi : v);
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
		XMVectorSet(XMVectorGetX(proj),
			XMVectorGetY(proj),
			XMVectorGetZ(proj),
			XMVectorGetW(q))
	);

	// 4. swing = q * twist^-1 (���� ȸ������ twist ȸ���� �� ���� swing���� ����)
	outSwing = XMQuaternionMultiply(q, XMQuaternionInverse(outTwist));
}

XMVECTOR IKManager::ClampSwingAsymmetric(
	XMVECTOR swing, XMVECTOR twistAxis,
	XMVECTOR localX, XMVECTOR localZ,
	float xMin, float xMax,
	float zMin, float zMax)
{
	XMVECTOR swungDir = XMVector3Rotate(twistAxis, swing);

	float projScale = XMVectorGetX(XMVector3Dot(swungDir, twistAxis));
	XMVECTOR proj = XMVectorSubtract(swungDir, XMVectorScale(twistAxis, projScale));

	float len = XMVectorGetX(XMVector3Length(proj));
	if (len < 1e-6f)
		return XMQuaternionIdentity();
	proj = XMVectorScale(proj, 1.0f / len);

	float dotX = ClampF(XMVectorGetX(XMVector3Dot(proj, localX)), -1.0f, 1.0f);
	float dotZ = ClampF(XMVectorGetX(XMVector3Dot(proj, localZ)), -1.0f, 1.0f);
	float angX = acosf(dotX);
	float angZ = acosf(dotZ);

	float signX = XMVectorGetX(XMVector3Dot(XMVector3Cross(localX, proj), twistAxis)) < 0 ? -1.0f : +1.0f;
	float signZ = XMVectorGetX(XMVector3Dot(XMVector3Cross(localZ, proj), twistAxis)) < 0 ? -1.0f : +1.0f;
	angX *= signX;
	angZ *= signZ;

	float degX = ClampF(XMConvertToDegrees(angX), xMin, xMax);
	float degZ = ClampF(XMConvertToDegrees(angZ), zMin, zMax);

	float radX = XMConvertToRadians(degX);
	float radZ = XMConvertToRadians(degZ);
	XMVECTOR qX = XMQuaternionRotationAxis(localX, radX);
	XMVECTOR qZ = XMQuaternionRotationAxis(localZ, radZ);
	XMVECTOR swingClamped = XMQuaternionMultiply(qZ, qX);

	return XMQuaternionNormalize(swingClamped);
}

XMVECTOR IKManager::ClampTwist(FXMVECTOR twist, FXMVECTOR twistAxis, float minDeg, float maxDeg)
{
	float qw = XMVectorGetW(twist);
	float angle = 2.0f * acosf(ClampF(qw, -1.0f, 1.0f));
	if (angle > XM_PI) // wrap to [-pi, pi]
		angle -= XM_2PI;

	float deg = ClampF(XMConvertToDegrees(angle), minDeg, maxDeg);
	float rad = XMConvertToRadians(deg);

	return XMQuaternionRotationAxis(twistAxis, rad);
}

void IKManager::clampBoneAngle(IKBone& bone, XMFLOAT4& quat)
{
	// IK�� ���� ���� ȸ�� ���ʹϾ�
	XMVECTOR qIK = XMLoadFloat4(&quat);

	// local �� ����
	XMVECTOR twistAxis = XMVectorSet(0, 1, 0, 0);
	XMVECTOR localX = XMVectorSet(1, 0, 0, 0);
	XMVECTOR localZ = XMVectorSet(0, 0, 1, 0);

	float xMin = bone.angleMinusLimits[0];
	float xMax = bone.anglePlusLimits[0];
	float tMin = bone.angleMinusLimits[1];
	float tMax = bone.anglePlusLimits[1];
	float zMin = bone.angleMinusLimits[2];
	float zMax = bone.anglePlusLimits[2];

	XMVECTOR swing, twist;

	// ���� quat�� Swing�� Twist quaternion���� �и�
	DecomposeSwingTwist(qIK, twistAxis, swing, twist);

	XMFLOAT4 dgTA, dgSW;
	float angle[3] = { 0.0f, 0.0f, 0.0f };
	XMStoreFloat4(&dgTA, twist);
	quaternionToEuler(dgTA, angle);
	p("before twist\n");
	p(std::to_string(angle[0]) + " " + std::to_string(angle[1]) + " " + std::to_string(angle[2]) + "\n");
	XMStoreFloat4(&dgSW, swing);
	quaternionToEuler(dgSW, angle);
	p("before swing\n");
	p(std::to_string(angle[0]) + " " + std::to_string(angle[1]) + " " + std::to_string(angle[2]) + "\n");

	p("merge twist and swing\n");
	XMVECTOR merge = XMQuaternionMultiply(swing, twist);
	XMStoreFloat4(&dgSW, merge);
	quaternionToEuler(dgSW, angle);
	p(std::to_string(angle[0]) + " " + std::to_string(angle[1]) + " " + std::to_string(angle[2]) + "\n");



	// Swing ȸ�� clamping
	XMVECTOR swingClamped = ClampSwingAsymmetric(
		swing, twistAxis, localX, localZ,
		xMin, xMax, zMin, zMax);

	// Twist ȸ�� clamping
	XMVECTOR twistClamped = ClampTwist(twist, twistAxis, tMin, tMax);


	XMStoreFloat4(&dgTA, twistClamped);
	quaternionToEuler(dgTA, angle);
	p("after twist\n");
	p(std::to_string(angle[0]) + " " + std::to_string(angle[1]) + " " + std::to_string(angle[2]) + "\n");
	XMStoreFloat4(&dgSW, swingClamped);
	quaternionToEuler(dgSW, angle);
	p("after swing\n");
	p(std::to_string(angle[0]) + " " + std::to_string(angle[1]) + " " + std::to_string(angle[2]) + "\n");
	XMVECTOR qFinal = XMQuaternionMultiply(swingClamped, twistClamped);

	XMStoreFloat4(&quat, qFinal);
}