#include "IKManager.h"
#include "AnimationData.h"
#include "Pose.h"

void IKManager::initLeftFootChains(Skeleton& skeleton)
{
	int idx = static_cast<int>(EChainPart::LEFT_FOOT);
	m_chains[idx].Bones.resize(4);
	m_chains[idx].DoFNum = 12;

	// LeftToeBase
	int boneIdx = skeleton.GetBoneIndex("mixamorig:LeftToeBase");
	m_chains[idx].Bones[0].idx = boneIdx;
	m_chains[idx].Bones[0].parentsIdx = skeleton.bones[boneIdx].parentIndex;
	m_chains[idx].Bones[0].xMax = skeleton.bones[boneIdx].xMax;
	m_chains[idx].Bones[0].xMin = skeleton.bones[boneIdx].xMin;
	m_chains[idx].Bones[0].zMax = skeleton.bones[boneIdx].zMax;
	m_chains[idx].Bones[0].zMin = skeleton.bones[boneIdx].zMin;
	m_chains[idx].Bones[0].axis = skeleton.bones[boneIdx].axis;
	m_chains[idx].Bones[0].twist = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(skeleton.bones[boneIdx].twist));

	// LeftFoot
	boneIdx = skeleton.GetBoneIndex("mixamorig:LeftFoot");
	m_chains[idx].Bones[1].idx = boneIdx;
	m_chains[idx].Bones[1].parentsIdx = skeleton.bones[boneIdx].parentIndex;
	m_chains[idx].Bones[1].xMax = skeleton.bones[boneIdx].xMax;
	m_chains[idx].Bones[1].xMin = skeleton.bones[boneIdx].xMin;
	m_chains[idx].Bones[1].zMax = skeleton.bones[boneIdx].zMax;
	m_chains[idx].Bones[1].zMin = skeleton.bones[boneIdx].zMin;
	m_chains[idx].Bones[1].axis = skeleton.bones[boneIdx].axis;
	m_chains[idx].Bones[1].twist = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(skeleton.bones[boneIdx].twist));

	// LeftLeg
	boneIdx = skeleton.GetBoneIndex("mixamorig:LeftLeg");
	m_chains[idx].Bones[2].idx = boneIdx;
	m_chains[idx].Bones[2].parentsIdx = skeleton.bones[boneIdx].parentIndex;
	m_chains[idx].Bones[2].xMax = skeleton.bones[boneIdx].xMax;
	m_chains[idx].Bones[2].xMin = skeleton.bones[boneIdx].xMin;
	m_chains[idx].Bones[2].zMax = skeleton.bones[boneIdx].zMax;
	m_chains[idx].Bones[2].zMin = skeleton.bones[boneIdx].zMin;
	m_chains[idx].Bones[2].axis = skeleton.bones[boneIdx].axis;
	m_chains[idx].Bones[2].twist = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(skeleton.bones[boneIdx].twist));


	// LeftUpLeg
	boneIdx = skeleton.GetBoneIndex("mixamorig:LeftUpLeg");
	m_chains[idx].Bones[3].idx = boneIdx;
	m_chains[idx].Bones[3].parentsIdx = skeleton.bones[boneIdx].parentIndex;
	m_chains[idx].Bones[3].xMax = skeleton.bones[boneIdx].xMax;
	m_chains[idx].Bones[3].xMin = skeleton.bones[boneIdx].xMin;
	m_chains[idx].Bones[3].zMax = skeleton.bones[boneIdx].zMax;
	m_chains[idx].Bones[3].zMin = skeleton.bones[boneIdx].zMin;
	m_chains[idx].Bones[3].axis = skeleton.bones[boneIdx].axis;
	m_chains[idx].Bones[3].twist = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(skeleton.bones[boneIdx].twist));
}

void IKManager::initRightFootChains(Skeleton& skeleton)
{
	int idx = static_cast<int>(EChainPart::RIGHT_FOOT);

	m_chains[idx].Bones.resize(4);
	m_chains[idx].DoFNum = 8;
	m_chains[idx].EndEffectorIdx = skeleton.GetBoneIndex(("mixamorig:RightToe_End"));

	// RightToeBase
	m_chains[idx].Bones[0].idx = skeleton.GetBoneIndex("mixamorig:RightToeBase");

	// RightFoot
	m_chains[idx].Bones[1].idx = skeleton.GetBoneIndex("mixamorig:RightFoot");

	// RightLeg
	m_chains[idx].Bones[2].idx = skeleton.GetBoneIndex("mixamorig:RightLeg");

	// RightUpLeg
	m_chains[idx].Bones[3].idx = skeleton.GetBoneIndex("mixamorig:RightUpLeg");

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
	W.resize(N, 0.0f);
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
		m_chains[i].Target = raycastingManager.m_LeftFoot.target;
		
	}

	// left chain Normal 설정
	{
		XMVECTOR baseNormal = XMVectorSet(0, 1, 0, 0);
		XMVECTOR normal = XMLoadFloat3(&raycastingManager.m_LeftFoot.normal);

		XMVECTOR det;
		int footParentIdx = m_chains[0].Bones[2].idx;
		XMMATRIX footParentTransform = XMMatrixMultiply(pose.world[footParentIdx], worldMatrix);
		XMVECTOR localNormal = XMVector3Normalize(XMVector3TransformNormal(normal, XMMatrixInverse(&det, footParentTransform)));
		
		// X
		XMVECTOR localNormalZY = XMVector2Normalize(XMVectorSet(XMVectorGetZ(localNormal), XMVectorGetY(localNormal), 0.0f, 0.0f));

		float dot = XMVectorGetX(XMVector2Dot(localNormalZY, baseNormal));  // 두 벡터 사이의 코사인
		float angle = safeAcosf(dot);
		if (XMVectorGetZ(localNormal) < 0.0f)
		{
			angle = XM_2PI - angle;
		}
		m_chains[0].footXAngle = -XMConvertToDegrees(angle);

		// Z
		XMVECTOR localNormalXY = XMVector2Normalize(XMVectorSet(XMVectorGetX(localNormal), XMVectorGetY(localNormal), 0.0f, 0.0f));

		dot = XMVectorGetX(XMVector2Dot(localNormalXY, baseNormal));  // 두 벡터 사이의 코사인
		angle = safeAcosf(dot);
		if (XMVectorGetX(localNormal) < 0.0f)
		{
			angle = XM_2PI - angle;
		}
		m_chains[0].footZAngle = -XMConvertToDegrees(angle);
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

			XMVECTOR toEffector = XMVectorSubtract(effectPos, bonePos);

			for (int k = 0; k < 3; ++k)
			{
				const XMFLOAT3& axis = axes[k];

				// 로컬 회전축 → 월드 회전축
				XMVECTOR localAxis = XMLoadFloat3(&axis);
				XMVECTOR worldAxis = XMVector3TransformNormal(localAxis, transform);
				worldAxis = XMVector3Normalize(worldAxis);

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
	static const float thetaAlpha = 0.3f;

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
			JTJ.Set(i, j, sum);
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
			JTx[i] += J.Get(j, i) * dP[j];
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
		dTheta[i] = dTheta[i] * thetaAlpha * W[i];
	}
}

void IKManager::updateNowRotation(Pose& pose)
{
	for (int i = 0; i < pose.count; ++i)
	{
		m_nowRotation[i] = pose.local[i].rotation;
	}
}

void IKManager::updateAngle(Pose& pose)
{
	for (int i = 0; i < m_chainNum; ++i)
	{
		int idx = 0;
		int count = m_chains[i].Bones.size();
		bool wasChanged = m_chains[i].isChanged;
		bool start = m_chains[i].start;
		m_chains[i].isChanged = false;
		m_chains[i].start = false;

		switch (i)
		{
			case 0:
			{
				footChainBufferUpdate(m_chains[i], start, wasChanged);
				break;
			}
			default:
			{
				break;
			}
		}


		for (int j = 0; j < count; ++j)
		{
			IKBone& bone = m_chains[i].Bones[j];

			float angle[3] = { 0.0f, 0.0f, 0.0f };

			for (int k = 0; k < 3; ++k)
			{
				angle[k] = XMConvertToDegrees(dTheta[idx]);
				idx++;
			}

			XMVECTOR qOld = XMLoadFloat4(&m_nowRotation[bone.idx]);

			//로컬 축 구하기
			XMVECTOR localX = XMVector3Rotate(XMVectorSet(1, 0, 0, 0), qOld);
			XMVECTOR localY = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), qOld);
			XMVECTOR localZ = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), qOld);

			//// 로컬 축 기준 델타 쿼터니언
			XMVECTOR dqX = XMQuaternionRotationAxis(localX, XMConvertToRadians(angle[0]));
			XMVECTOR dqY = XMQuaternionRotationAxis(localY, XMConvertToRadians(angle[1]));
			XMVECTOR dqZ = XMQuaternionRotationAxis(localZ, XMConvertToRadians(angle[2]));
	
			// 합성 및 적용
			XMVECTOR newQuat = XMQuaternionMultiply(XMQuaternionMultiply(dqZ, dqY), dqX);
			XMVECTOR qNew = XMQuaternionNormalize(XMQuaternionMultiply(qOld, newQuat));
			
			XMStoreFloat4(&m_nowRotation[bone.idx], qNew);

			// clamping
			clampBoneAngle(bone, m_nowRotation[bone.idx], pose);

			m_chains[i].isChanged = true;
		}
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

void IKManager::clampBoneAngle(IKBone& bone, XMFLOAT4& quat, Pose& pose)
{
	//p("bone " + std::to_string(bone.idx) + " start clamping!!\n");
	// IK로 계산된 최종 회전 쿼터니언
	XMVECTOR qIK = XMLoadFloat4(&quat);

	// local 축 정의
	XMVECTOR twistAxis = XMLoadFloat3(&bone.axis);

	float xMax, xMin, zMax, zMin;
	xMax = bone.angleBuffer.xMax;
	xMin = bone.angleBuffer.xMin;
	zMax = bone.angleBuffer.zMax;
	zMin = bone.angleBuffer.zMin;

	XMVECTOR& twist = bone.twist;

	std::vector<XMVECTOR> polygon;
	makePolygon(polygon, twist, xMax, xMin, zMax, zMin);

	XMVECTOR qFinal = ClampSwingBySphericalPolygon(qIK, twist, twistAxis, polygon);
	qFinal = XMQuaternionNormalize(qFinal);

	pose.twist[bone.idx] = twist;

	XMStoreFloat4(&quat, XMQuaternionNormalize(qFinal));
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

		XMVECTOR edgeNormal = XMVector3Normalize(XMVector3Cross(a, b));
		float side = XMVectorGetX(XMVector3Dot(edgeNormal, D));

		if (side > 0.0f) // 바깥쪽
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

XMVECTOR IKManager::divideQuaternionToYXZ(XMVECTOR& D, XMVECTOR& twist)
{
	float xAngle, yAngle, zAngle;

	XMVECTOR Y = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR dXY = XMVector2Normalize(XMVectorSet(XMVectorGetX(D), XMVectorGetY(D), 0.0f, 0.0f));
	if (fabs(XMVectorGetX(D)) < 1e-6f)
	{
		zAngle = 0.0f;
	}
	else
	{
		zAngle = safeAcosf(std::abs(XMVectorGetY(dXY)));
		if (XMVectorGetY(D) < 0.0f) { zAngle = -zAngle; }
		if (XMVectorGetX(D) < 0.0f) { zAngle = -zAngle; }
	}

	XMVECTOR qzInv = XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), zAngle);
	XMMATRIX qzInvTransform = XMMatrixRotationQuaternion(qzInv);
	XMVECTOR D_ZRotated = XMVector3TransformNormal(D, qzInvTransform);

	XMVECTOR dZY = XMVector2Normalize(XMVectorSet(XMVectorGetZ(D_ZRotated), XMVectorGetY(D_ZRotated), 0.0f, 0.0f));
	xAngle = safeAcosf(XMVectorGetX(XMVector2Dot(Y, dZY)));

	if (XMVectorGetZ(D_ZRotated) < 0.0f) { xAngle = -xAngle; }

	XMVECTOR qx, qy, qz;

	zAngle = -zAngle;

	qx = XMQuaternionRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), xAngle);
	qz = XMQuaternionRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), zAngle);



	float thetaX, thetaY;

	if (fabs(XMVectorGetX(D)) < 1e-6f)
	{
		thetaY = 0.0f;
	}
	else
	{
		thetaY = safeAcosf(fabs((XMVectorGetZ(D)) / sqrt(pow(XMVectorGetX(D), 2) + pow(XMVectorGetZ(D), 2))));
		if (XMVectorGetZ(D) > 0.0f) { thetaY = -thetaY; }
		if (XMVectorGetX(D) < 0.0f) { thetaY = -thetaY; }
	}

	XMVECTOR qy2 = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), thetaY);
	XMVECTOR D_YRotated = XMVector3Rotate(D, qy2);
	XMVECTOR dZY3 = XMVector2Normalize(XMVectorSet(XMVectorGetZ(D_YRotated), XMVectorGetY(D_YRotated), 0.0f, 0.0f));

	thetaX = safeAcosf(XMVectorGetX(XMVector2Dot(Y, dZY3)));
	if (XMVectorGetZ(D_YRotated) < 0.0f) { thetaX = -thetaX; }

	XMVECTOR qthetaX = XMQuaternionRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), thetaX);
	XMVECTOR qthetaY = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), thetaY);

	XMVECTOR front1 = XMVector3Rotate(XMVectorSet(0, 0, -1, 0), twist);
	XMVECTOR front2 = XMVectorSet(0, 0, -1, 0);
	XMVECTOR q2 = XMQuaternionNormalize(XMQuaternionMultiply(XMQuaternionMultiply(qx, qz), qthetaY));

	XMVECTOR v1 = XMVector3Rotate(front1, qthetaX);
	XMVECTOR v2 = XMVector3Rotate(front2, q2);

	XMVECTOR axis = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), qthetaX);
	float dotV1V2 = XMVectorGetX(XMVector3Dot(v1, v2));
	yAngle = safeAcosf(dotV1V2);
	XMVECTOR crossVal = XMVector3Cross(v1, v2);
	if (XMVectorGetX(XMVector3Dot(axis, crossVal)) > 0.0f) { yAngle = -yAngle; }

	qy = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), yAngle);

	//p("D: " + std::to_string(XMVectorGetX(D)) + " " + std::to_string(XMVectorGetY(D)) + " " + std::to_string(XMVectorGetZ(D)) + "\n");
	//p("D_clamped: " + std::to_string(XMVectorGetX(D_clamped)) + " " + std::to_string(XMVectorGetY(D_clamped)) + " " + std::to_string(XMVectorGetZ(D_clamped)) + "\n");
	//p("xAngle: " + std::to_string(xAngle) + "\n");
	//p("yAngle: " + std::to_string(yAngle) + "\n");
	//p("zAngle: " + std::to_string(zAngle) + "\n");

	return XMQuaternionNormalize(XMQuaternionMultiply(XMQuaternionMultiply(qy, qx), qz));  // x 먼저, z 나중
}

XMVECTOR IKManager::ClampSwingBySphericalPolygon(XMVECTOR& swing, XMVECTOR& twist, XMVECTOR twistAxis, const std::vector<XMVECTOR>& polygon)
{
	twistAxis = XMVector3Normalize(twistAxis);
	XMVECTOR D = XMVector3Rotate(twistAxis, swing);  // swing에 의해 이동된 방향
	XMVECTOR D_clamped = ClampDirectionToSphericalPolygon(D, polygon);
	return divideQuaternionToYXZ(D_clamped, twist);
}

void IKManager::makePolygon(std::vector<XMVECTOR>& polygon, XMVECTOR& twistClamped, float xMax, float xMin, float zMax, float zMin)
{
	float xDeg = xMax;
	float zDeg = zMax;

	XMVECTOR qx = XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), XMConvertToRadians(xDeg));
	XMVECTOR qz = XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), XMConvertToRadians(zDeg));
	XMVECTOR D = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), qz);
	D = XMVector3Rotate(D, qx);
	D = XMVector3Rotate(D, twistClamped);
	polygon.push_back(XMVector3Normalize(D));

	xDeg = xMin;
	qx = XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), XMConvertToRadians(xDeg));
	qz = XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), XMConvertToRadians(zDeg));
	D = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), qz);
	D = XMVector3Rotate(D, qx);
	D = XMVector3Rotate(D, twistClamped);
	polygon.push_back(XMVector3Normalize(D));


	zDeg = zMin;
	qx = XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), XMConvertToRadians(xDeg));
	qz = XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), XMConvertToRadians(zDeg));
	D = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), qz);
	D = XMVector3Rotate(D, qx);
	D = XMVector3Rotate(D, twistClamped);
	polygon.push_back(XMVector3Normalize(D));

	xDeg = xMax;
	qx = XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), XMConvertToRadians(xDeg));
	qz = XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), XMConvertToRadians(zDeg));
	D = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), qz);
	D = XMVector3Rotate(D, qx);
	D = XMVector3Rotate(D, twistClamped);
	polygon.push_back(XMVector3Normalize(D));

	polygon.push_back(polygon[0]);

	//for (int i = 0; i < polygon.size(); i++)
	//{
	//	p("polygon " + std::to_string(i) + " : " + std::to_string(XMVectorGetX(polygon[i])) + " " + std::to_string(XMVectorGetY(polygon[i])) + " " + std::to_string(XMVectorGetZ(polygon[i])) + "\n");
	//}
}

void IKManager::resetValuesForIK(RaycastingManager& raycastingManager, Skeleton& skeleton)
{
	int chainCount = m_chains.size();
	for (int i = 0; i < chainCount; ++i)
	{
		m_chains[i].isChanged = true;
		m_chains[i].start = true;
		m_chains[i].angleBufferSign = true;
	}
	
	initLeftFootChainInfo(raycastingManager, skeleton);
}

void IKManager::initLeftFootChainInfo(RaycastingManager& raycastingManager, Skeleton& skeleton)
{
	IKChain& chain = m_chains[0];
	RaycastingInfo& raycastingInfo = raycastingManager.m_LeftFoot;

	chain.EndEffectorIdx = skeleton.GetBoneIndex("mixamorig:LeftToeBase");

	// ToeBase x, y, z
	W[0] = 0.0f;
	W[1] = 0.0f;
	W[2] = 0.0f;

	// Foot x, y, z
	W[3] = 1.0f;
	W[4] = 1.0f;
	W[5] = 1.0f;

	// Leg x, y, z
	W[6] = 3.0f;
	W[7] = 3.0f;
	W[8] = 3.0f;

	// UpLeg x, y, z
	W[9] = 3.0f;
	W[10] = 3.0f;
	W[11] = 3.0f;
}

void IKManager::footChainBufferUpdate(IKChain& chain, bool start, bool wasChanged)
{
	// angleBuffer 초기화
	int count = chain.Bones.size();
	for (int i = 0; i < count; ++i)
	{
		IKBone& bone = chain.Bones[i];

		bone.angleBuffer.xMax = bone.xMax;
		bone.angleBuffer.xMin = bone.xMin;
		bone.angleBuffer.zMax = bone.zMax;
		bone.angleBuffer.zMin = bone.zMin;
	}

	// 1. 지면에 발목 각도 맞추기
	{
		float footXangle = chain.footXAngle;
		float footZangle = chain.footZAngle;

		footXangle = -240.0f - footXangle;
		footZangle = -180.0f + footZangle;

		IKBone& footBone = chain.Bones[1];

		footBone.angleBuffer.xMax = footXangle + 0.01f;
		footBone.angleBuffer.xMin = footXangle;
		footBone.angleBuffer.zMax = footZangle + 0.01f;
		footBone.angleBuffer.zMin = footZangle;
	}

	// 2. start == true인 경우 위에서 부터 발을 내림
	if (start == true)
	{
		IKBone& legBone = chain.Bones[2];
		IKBone& upLegBone = chain.Bones[3];

		upLegBone.angleBuffer.xMax = upLegBone.xMin + 0.01f;
		upLegBone.angleBuffer.xMin = upLegBone.xMin;
		upLegBone.angleBuffer.zMax = upLegBone.zMax + 0.01f;
		upLegBone.angleBuffer.zMin = upLegBone.zMax;

		legBone.angleBuffer.xMax = legBone.xMax + 0.01f;
		legBone.angleBuffer.xMin = legBone.xMax;
		legBone.angleBuffer.zMax = legBone.zMax + 0.01f;
		legBone.angleBuffer.zMin = legBone.zMax;
	}

	if (wasChanged == false)
	{
		IKBone& upLegBone = chain.Bones[3];
		IKBone& legBone = chain.Bones[2];

		upLegBone.angleBuffer.zMax = upLegBone.zMax + 0.01f;
		upLegBone.angleBuffer.zMin = upLegBone.zMax;

		legBone.angleBuffer.zMax = legBone.zMax + 0.01f;
		legBone.angleBuffer.zMin = legBone.zMax;
	}
}

float IKManager::safeAcosf(float dotResult)
{
	return acosf(std::clamp(dotResult, -1.0f, 1.0f));
}
