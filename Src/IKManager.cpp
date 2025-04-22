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

	// x축 default 45도
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

	// x축 default 50도
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

	// left chain만 적용
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
	// endEffector 등록
	for (int i = 0; i < m_chainNum; i++)
	{
		int boneIdx = m_chains[i].EndEffectorIdx;
		XMVECTOR endEffector = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMMATRIX transform = XMMatrixMultiply(pose.world[boneIdx], worldMatrix);
		endEffector = XMVector4Transform(endEffector, transform);
		XMStoreFloat3(&m_chains[i].EndEffector, endEffector);
		// target 등록
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

				// 해당 axis의 자유도가 없으면 continue
				if (bone.angleEnable[k] == false)
				{
					continue;
				}

				// 로컬 회전축 → 월드 회전축
				XMVECTOR localAxis = XMLoadFloat3(&axis);
				XMVECTOR worldAxis = XMVector3TransformNormal(localAxis, transform);
				worldAxis = XMVector3Normalize(worldAxis);

				XMFLOAT3 debugAxis;
				XMStoreFloat3(&debugAxis, worldAxis);


				// 위치 변화 = 회전축 × effector 방향
				XMVECTOR dPdTheta = XMVector3Cross(worldAxis, toEffector);

				// Jacobian 행렬에 값 저장
				J.Set(i * 3, colIdx, XMVectorGetX(dPdTheta)); // dx/dθ
				J.Set(i * 3 + 1, colIdx, XMVectorGetY(dPdTheta)); // dy/dθ
				J.Set(i * 3 + 2, colIdx, XMVectorGetZ(dPdTheta)); // dz/dθ
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

	// JTJ 구하기
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


	// JTJ + lambda^2 * I (감쇠 항 추가)
	for (int i = 0; i < m_colNum; ++i)
	{
		JTJ.Set(i, i, JTJ.Get(i, i) + lambda * lambda);
	}

	// JT * dP 계산
	for (int i = 0; i < m_colNum; ++i)
	{
		JTx[i] = 0.0f;
		for (int j = 0; j < m_rowNum; ++j)
		{
			JTx[i] += w[i] * J.Get(j, i) * dP[j];
		}
	}

	// JTJ 역행렬 계산 (가우스-조던 소거법)
	JTJInv.SetZero();
	for (int i = 0; i < m_colNum; ++i)
	{
		JTJInv.Set(i, i, 1.0f);
	}
	// JTJ 값을 나중에 쓰려면 JTJ를 복사해서 아래를 진행시켜야함 (JTJ 값 파괴)
	for (int i = 0; i < m_colNum; ++i)
	{
		float pivot = JTJ.Get(i, i);

		// singular 보호
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

	// JTJInv * JTx 를 통해 dTheta 계산

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

			// 기존 quaternion을 pitch yaw roll로 변환
			
			for (int k = 0; k < 3; ++k)
			{
				if (bone.angleEnable[k] == true)
				{
					angle[k] = XMConvertToDegrees(dTheta[idx]);
					idx++;
				}
			}

			XMVECTOR qOld = XMLoadFloat4(&m_nowRotation[bone.idx]);
			 //로컬 축 구하기
			XMVECTOR localX = XMVectorSet(1, 0, 0, 0);
			XMVECTOR localY = XMVectorSet(0, 1, 0, 0);
			XMVECTOR localZ = XMVectorSet(0, 0, 1, 0);

			//// 로컬 축 기준 델타 쿼터니언
			XMVECTOR dqX = XMQuaternionRotationAxis(localX, XMConvertToRadians(angle[0]));
			XMVECTOR dqY = XMQuaternionRotationAxis(localY, XMConvertToRadians(angle[1]));
			XMVECTOR dqZ = XMQuaternionRotationAxis(localZ, XMConvertToRadians(angle[2]));

			// 합성 및 적용
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

// X -> Y -> Z 순서
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
	// 1. twist 축 정규화
	twistAxis = XMVector3Normalize(twistAxis);

	// 2. q 의 벡터 부분을 twist 축에 사영 (쿼터니언의 x, y, z를 다른 축에 내적하면 축에대한 회전을 분리할 수 있음)
	XMVECTOR qVec = XMVectorSet(XMVectorGetX(q), XMVectorGetY(q), XMVectorGetZ(q), 0.0f);
	float dotVT = XMVectorGetX(XMVector3Dot(qVec, twistAxis));
	XMVECTOR proj = XMVectorScale(twistAxis, dotVT);

	// 3. twist 쿼터니언 (proj xyz + q.w) 정규화
	outTwist = XMQuaternionNormalize(
		XMVectorSet(
			XMVectorGetX(proj),
			XMVectorGetY(proj),
			XMVectorGetZ(proj),
			XMVectorGetW(q)
		)
	);

	// 4. swing = q * twist^-1 (기존 회전에서 twist 회전을 뺀 것을 swing으로 추출)
	outSwing = XMQuaternionNormalize(XMQuaternionMultiply(q, XMQuaternionInverse(outTwist)));
}

XMVECTOR IKManager::ClampTwist(FXMVECTOR twist, FXMVECTOR twistAxis, float minDeg, float maxDeg)
{
	float qw = XMVectorGetW(twist);
	float angle = 2.0f * acosf(std::clamp(qw, -1.0f, 1.0f));
	if (angle >= XM_2PI)
	{
		angle -= XM_2PI;
	}
	if (angle <= -XM_2PI)
	{
		angle += XM_2PI;
	}

	// clamp
	float deg = std::clamp(XMConvertToDegrees(angle), minDeg, maxDeg);
	float rad = XMConvertToRadians(deg);

	return XMQuaternionRotationAxis(twistAxis, rad);
}

void IKManager::clampBoneAngle(IKBone& bone, XMFLOAT4& quat)
{
	// IK로 계산된 최종 회전 쿼터니언
	XMVECTOR qIK = XMLoadFloat4(&quat);

	// local 축 정의
	XMVECTOR twistAxis = XMLoadFloat3(&bone.axis);

	float xMax = bone.xMax;
	float xMin = bone.xMin;
	float yMax = bone.yMax;
	float yMin = bone.yMin;
	float zMax = bone.zMax;
	float zMin = bone.zMin;

	XMVECTOR swing, twist;

	// 기존 quat을 Swing과 Twist quaternion으로 분리
	DecomposeSwingTwist(qIK, twistAxis, swing, twist);


	std::vector<XMVECTOR> polygon;
	makePolygon(polygon, xMax, xMin, zMax, zMin);

	// Swing 회전 clamping
	XMVECTOR swingClamped = ClampSwingBySphericalPolygon(swing, twistAxis, polygon);

	// Twist 회전 clamping
	XMVECTOR twistClamped = ClampTwist(twist, twistAxis, yMin, yMax);

	swingClamped = XMQuaternionNormalize(swingClamped);
	twistClamped = XMQuaternionNormalize(twistClamped);

	XMVECTOR qFinal;
	if (bone.idx == 57)
		qFinal = XMQuaternionMultiply(swingClamped, twistClamped);
	else
		qFinal = twistClamped;

	XMStoreFloat4(&quat, XMQuaternionNormalize(qFinal));
}


float IKManager::SafeACos(float x)
{
	return acosf(std::clamp(x, -1.0f, 1.0f));
}

XMVECTOR IKManager::ClampDirectionToSphericalPolygon(XMVECTOR D, const std::vector<XMVECTOR>& polygon)
{
	D = XMVector3Normalize(D);
	bool inside = true;
	float minAngle = XM_PI;
	XMVECTOR closest = D;

	size_t count = polygon.size();
	for (size_t i = 0; i < count; ++i)
	{
		XMVECTOR a = polygon[i];
		XMVECTOR b = polygon[(i + 1) % count];

		XMVECTOR edgeNormal = XMVector3Normalize(XMVector3Cross(a, b));
		float side = XMVectorGetX(XMVector3Dot(edgeNormal, D));

		if (side < 0.0f) // 바깥쪽
		{
			inside = false;

			float dotEN = XMVectorGetX(XMVector3Dot(D, edgeNormal));
			XMVECTOR proj = XMVector3Normalize(D - edgeNormal * dotEN);

			float angle_a_proj = SafeACos(XMVectorGetX(XMVector3Dot(proj, a)));
			float angle_b_proj = SafeACos(XMVectorGetX(XMVector3Dot(proj, b)));
			float angle_ab = SafeACos(XMVectorGetX(XMVector3Dot(a, b)));

			XMVECTOR candidate;
			if (angle_a_proj + angle_b_proj <= angle_ab + 1e-3f)
				candidate = proj;
			else
			{
				float da = SafeACos(XMVectorGetX(XMVector3Dot(D, a)));
				float db = SafeACos(XMVectorGetX(XMVector3Dot(D, b)));
				candidate = (da < db) ? a : b;
			}

			float dist = SafeACos(XMVectorGetX(XMVector3Dot(D, candidate)));
			if (dist < minAngle)
			{
				minAngle = dist;
				closest = candidate;
			}
		}
	}

	return inside ? D : closest;
}

XMVECTOR IKManager::ClampSwingBySphericalPolygon(XMVECTOR swing, XMVECTOR twistAxis, const std::vector<XMVECTOR>& polygon)
{
	twistAxis = XMVector3Normalize(twistAxis);
	XMVECTOR D = XMVector3Rotate(twistAxis, swing);  // swing에 의해 이동된 방향
	XMVECTOR D_clamped = ClampDirectionToSphericalPolygon(D, polygon);

	float dot = std::clamp(XMVectorGetX(XMVector3Dot(twistAxis, D_clamped)), -1.0f, 1.0f);
	float angle = acosf(dot);

	XMVECTOR axis = XMVector3Cross(twistAxis, D_clamped);
	if (XMVector3LengthSq(axis).m128_f32[0] < 1e-6f)
		return XMQuaternionIdentity();  // 회전 없음

	axis = XMVector3Normalize(axis);
	return XMQuaternionRotationAxis(axis, angle);
}

void IKManager::makePolygon(std::vector<XMVECTOR>& polygon, float xMax, float xMin, float zMax, float zMin)
{
	int numSegments = 16;

	for (int i = 0; i < numSegments; ++i)
	{
		float t = (float)i / numSegments;  // 0 ~ 1
		float xDeg = Lerp(xMin, xMax, (cosf(t * XM_2PI) + 1.0f) * 0.5f);
		float zDeg = Lerp(zMin, zMax, (sinf(t * XM_2PI) + 1.0f) * 0.5f);

		XMVECTOR qx = XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), XMConvertToRadians(xDeg));
		XMVECTOR qz = XMQuaternionRotationAxis(XMVectorSet(0, 0, -1, 0), XMConvertToRadians(zDeg));
		XMVECTOR swing = XMQuaternionMultiply(qz, qx);  // x 먼저, z 나중

		XMVECTOR D = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), swing); // swing에 의해 이동된 Y축
		polygon.push_back(XMVector3Normalize(D));
	}
}

// X -> Y -> Z 순서 quat의 x, y, z의 부호가 반대로 나옴
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
