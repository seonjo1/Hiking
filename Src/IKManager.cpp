#include "IKManager.h"
#include "AnimationData.h"
#include "Pose.h"

void IKManager::initLeftFootChains(Skeleton& skeleton)
{
	int idx = static_cast<int>(EChainPart::LEFT_FOOT);
	m_chains[idx].Bones.resize(4);
	m_chains[idx].DoFNum = 12;

	m_chains[idx].clampingIdx = 2;

	// LeftUpLeg
	int boneIdx = skeleton.GetBoneIndex("mixamorig:LeftUpLeg");

	m_chains[idx].Bones[0].idx = boneIdx;
	m_chains[idx].Bones[0].parentsIdx = skeleton.bones[boneIdx].parentIndex;
	m_chains[idx].Bones[0].xMax = skeleton.bones[boneIdx].xMax;
	m_chains[idx].Bones[0].xMin = skeleton.bones[boneIdx].xMin;
	m_chains[idx].Bones[0].zMax = skeleton.bones[boneIdx].zMax;
	m_chains[idx].Bones[0].zMin = skeleton.bones[boneIdx].zMin;
	m_chains[idx].Bones[0].axis = skeleton.bones[boneIdx].axis;
	m_chains[idx].Bones[0].twist = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(skeleton.bones[boneIdx].twist));

	// LeftLeg
	boneIdx = skeleton.GetBoneIndex("mixamorig:LeftLeg");

	m_chains[idx].Bones[1].idx = boneIdx;
	m_chains[idx].Bones[1].parentsIdx = skeleton.bones[boneIdx].parentIndex;
	m_chains[idx].Bones[1].xMax = skeleton.bones[boneIdx].xMax;
	m_chains[idx].Bones[1].xMin = skeleton.bones[boneIdx].xMin;
	m_chains[idx].Bones[1].zMax = skeleton.bones[boneIdx].zMax;
	m_chains[idx].Bones[1].zMin = skeleton.bones[boneIdx].zMin;
	m_chains[idx].Bones[1].axis = skeleton.bones[boneIdx].axis;
	m_chains[idx].Bones[1].twist = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(skeleton.bones[boneIdx].twist));

	// LeftFoot
	boneIdx = skeleton.GetBoneIndex("mixamorig:LeftFoot");

	m_chains[idx].Bones[2].idx = boneIdx;
	m_chains[idx].Bones[2].parentsIdx = skeleton.bones[boneIdx].parentIndex;
	m_chains[idx].Bones[2].childIdx = skeleton.GetBoneIndex("mixamorig:LeftToeBase");
	m_chains[idx].Bones[2].xMax = skeleton.bones[boneIdx].xMax;
	m_chains[idx].Bones[2].xMin = skeleton.bones[boneIdx].xMin;
	m_chains[idx].Bones[2].zMax = skeleton.bones[boneIdx].zMax;
	m_chains[idx].Bones[2].zMin = skeleton.bones[boneIdx].zMin;
	m_chains[idx].Bones[2].axis = skeleton.bones[boneIdx].axis;
	m_chains[idx].Bones[2].twist = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(skeleton.bones[boneIdx].twist));


	// LeftToeBase
	boneIdx = skeleton.GetBoneIndex("mixamorig:LeftToeBase");

	m_chains[idx].Bones[3].idx = boneIdx;
	m_chains[idx].Bones[3].parentsIdx = skeleton.bones[boneIdx].parentIndex;
	m_chains[idx].Bones[3].childIdx = skeleton.GetBoneIndex("mixamorig:LeftToe_End");
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
	m_chains[idx].DoFNum = 12;

	m_chains[idx].clampingIdx = 2;

	// RightUpLeg
	int boneIdx = skeleton.GetBoneIndex("mixamorig:RightUpLeg");

	m_chains[idx].Bones[0].idx = boneIdx;
	m_chains[idx].Bones[0].parentsIdx = skeleton.bones[boneIdx].parentIndex;
	m_chains[idx].Bones[0].xMax = skeleton.bones[boneIdx].xMax;
	m_chains[idx].Bones[0].xMin = skeleton.bones[boneIdx].xMin;
	m_chains[idx].Bones[0].zMax = skeleton.bones[boneIdx].zMax;
	m_chains[idx].Bones[0].zMin = skeleton.bones[boneIdx].zMin;
	m_chains[idx].Bones[0].axis = skeleton.bones[boneIdx].axis;
	m_chains[idx].Bones[0].twist = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(skeleton.bones[boneIdx].twist));

	// RightLeg
	boneIdx = skeleton.GetBoneIndex("mixamorig:RightLeg");

	m_chains[idx].Bones[1].idx = boneIdx;
	m_chains[idx].Bones[1].parentsIdx = skeleton.bones[boneIdx].parentIndex;
	m_chains[idx].Bones[1].xMax = skeleton.bones[boneIdx].xMax;
	m_chains[idx].Bones[1].xMin = skeleton.bones[boneIdx].xMin;
	m_chains[idx].Bones[1].zMax = skeleton.bones[boneIdx].zMax;
	m_chains[idx].Bones[1].zMin = skeleton.bones[boneIdx].zMin;
	m_chains[idx].Bones[1].axis = skeleton.bones[boneIdx].axis;
	m_chains[idx].Bones[1].twist = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(skeleton.bones[boneIdx].twist));

	// RightFoot
	boneIdx = skeleton.GetBoneIndex("mixamorig:RightFoot");

	m_chains[idx].Bones[2].idx = boneIdx;
	m_chains[idx].Bones[2].parentsIdx = skeleton.bones[boneIdx].parentIndex;
	m_chains[idx].Bones[2].childIdx = skeleton.GetBoneIndex("mixamorig:RightToeBase");
	m_chains[idx].Bones[2].xMax = skeleton.bones[boneIdx].xMax;
	m_chains[idx].Bones[2].xMin = skeleton.bones[boneIdx].xMin;
	m_chains[idx].Bones[2].zMax = skeleton.bones[boneIdx].zMax;
	m_chains[idx].Bones[2].zMin = skeleton.bones[boneIdx].zMin;
	m_chains[idx].Bones[2].axis = skeleton.bones[boneIdx].axis;
	m_chains[idx].Bones[2].twist = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(skeleton.bones[boneIdx].twist));


	// RightToeBase
	boneIdx = skeleton.GetBoneIndex("mixamorig:RightToeBase");

	m_chains[idx].Bones[3].idx = boneIdx;
	m_chains[idx].Bones[3].parentsIdx = skeleton.bones[boneIdx].parentIndex;
	m_chains[idx].Bones[3].childIdx = skeleton.GetBoneIndex("mixamorig:RightToe_End");
	m_chains[idx].Bones[3].xMax = skeleton.bones[boneIdx].xMax;
	m_chains[idx].Bones[3].xMin = skeleton.bones[boneIdx].xMin;
	m_chains[idx].Bones[3].zMax = skeleton.bones[boneIdx].zMax;
	m_chains[idx].Bones[3].zMin = skeleton.bones[boneIdx].zMin;
	m_chains[idx].Bones[3].axis = skeleton.bones[boneIdx].axis;
	m_chains[idx].Bones[3].twist = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(skeleton.bones[boneIdx].twist));

}

void IKManager::initIKChains(Skeleton& skeleton)
{
	m_chainNum = 2;
	m_rowNum = m_chainNum * 3;
	m_chains.resize(m_chainNum);
	m_nowRotation.resize(skeleton.bones.size(), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	m_prevRotation.resize(skeleton.bones.size(), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	initLeftFootChains(skeleton);
	initRightFootChains(skeleton);
	
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
	}
}

void IKManager::calculateJacobianMatrix(Pose& pose, XMMATRIX& worldMatrix)
{
	static const XMFLOAT3 axes[3] = { XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) };
	J.SetZero();
	int colIdx = 0;
	for (int i = 0; i < m_chainNum; ++i)
	{
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

void IKManager::updateAngle(Pose& pose, XMMATRIX& worldMatrix, Skeleton& skeleton)
{
	for (int i = 0; i < m_chainNum; ++i)
	{
		int idx = i * 12;
		int count = m_chains[i].Bones.size();
		bool start = m_chains[i].start;
		m_chains[i].start = false;

		switch (i)
		{
			case 0:
			{
				leftFootChainBufferUpdate(m_chains[i], start);
				break;
			}
			case 1:
			{
				rightFootChainBufferUpdate(m_chains[i], start);
				break;
			}
			default:
			{
				break;
			}
		}

		int clampingIdx = m_chains[i].clampingIdx;

		for (int j = 0; j < clampingIdx; ++j)
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
		}

		for (int j = clampingIdx; j < count; ++j)
		{
			IKBone& bone = m_chains[i].Bones[j];

			// world 좌표계에서의 bone 벡터
			XMFLOAT3 worldDest = bone.worldDest;
			XMFLOAT3 worldChildDest = bone.worldChildDest;

			// local로 변환 (부모의 변환은 현재 변경된 quat으로 계산)
			int boneIdx = bone.idx;
			XMMATRIX transform = XMMatrixIdentity();

			{
				XMMATRIX local = pose.getLocalTranslationMatrix(boneIdx);
				transform = XMMatrixMultiply(transform, local);
				boneIdx = skeleton.bones[boneIdx].parentIndex;
			}

			XMMATRIX parent = XMMatrixIdentity();
			while (boneIdx != -1)
			{
				XMMATRIX T = XMMatrixTranslation(pose.local[boneIdx].position.x, pose.local[boneIdx].position.y, pose.local[boneIdx].position.z);
				XMVECTOR quat = XMLoadFloat4(&m_nowRotation[boneIdx]);
				XMMATRIX R = XMMatrixRotationQuaternion(quat);
				XMMATRIX S = XMMatrixScaling(pose.local[boneIdx].scale.x, pose.local[boneIdx].scale.y, pose.local[boneIdx].scale.z);
				XMMATRIX local = XMMatrixMultiply(XMMatrixMultiply(S, R), T);

				parent = XMMatrixMultiply(parent, local);
				boneIdx = skeleton.bones[boneIdx].parentIndex;
			}
			transform = XMMatrixMultiply(transform, parent);
			transform = XMMatrixMultiply(transform, worldMatrix);

			XMVECTOR det;
			transform = XMMatrixInverse(&det, transform);
			XMVECTOR localDest = XMVector3TransformNormal(XMLoadFloat3(&worldDest), transform);
			XMVECTOR localChildDest = XMVector3TransformNormal(XMLoadFloat3(&worldChildDest), transform);
			XMVECTOR localBoneVector = XMVector3Normalize(XMVectorSubtract(localChildDest, localDest));

			XMFLOAT3 y = { 0.0f, 1.0f, 0.0f };
			XMFLOAT3 dest;
			XMStoreFloat3(&dest, localBoneVector);

			// 0, 1, 0에서 local 로 변환하는 거 찾기
			XMVECTOR toDestQuat = getQuatFromTo(y, dest);

			// 해당 quat으로 이동된 D를 통해 최종 변환 구하기
			XMVECTOR D = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), toDestQuat);  // swing에 의해 이동된 방향
			m_nowRotation[bone.idx] = recreateD(bone, D, pose);
		}
	}
}

XMFLOAT4 IKManager::recreateD(IKBone& bone, XMVECTOR& D, Pose& pose)
{
	// local 축 정의
	XMVECTOR twistAxis = XMLoadFloat3(&bone.axis);
	XMVECTOR& twist = bone.twist;

	XMVECTOR qFinal = divideQuaternionToYXZ(D, twist);
	qFinal = XMQuaternionNormalize(qFinal);

	pose.twist[bone.idx] = twist;

	XMFLOAT4 quat;
	XMStoreFloat4(&quat, qFinal);
	return quat;
}

bool IKManager::isFinish(Pose& pose, XMMATRIX& worldMatrix)
{
	const float THRESHOLD = 0.005f;

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
		//p("length: " + std::to_string(length) + "\n");
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

void IKManager::blendingIKRotation()
{
	static bool init = false;
	static float alpha = 0.5f;
	
	if (init == false)
	{
		init = true;
		for (int i = 0; i < m_chainNum; ++i)
		{
			IKChain& chain = m_chains[i];
			for (int j = 0; j < chain.Bones.size(); ++j)
			{
				IKBone& bone = chain.Bones[j];
				int idx = bone.idx;
				m_prevRotation[idx] = m_nowRotation[idx];
			}
		}
		return;
	}
	
	for (int i = 0; i < m_chainNum; ++i)
	{
		IKChain& chain = m_chains[i];
		for (int j = 0; j < chain.Bones.size(); ++j)
		{
			IKBone& bone = chain.Bones[j];
			int idx = bone.idx;

			XMVECTOR prevRotation = XMLoadFloat4(&m_prevRotation[idx]);
			XMVECTOR nowRotation = XMLoadFloat4(&m_nowRotation[idx]);
			nowRotation = XMQuaternionSlerp(prevRotation, nowRotation, alpha);
			XMStoreFloat4(&m_prevRotation[idx], nowRotation);
			XMStoreFloat4(&m_nowRotation[idx], nowRotation);
		}
	}
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

	XMVECTOR front = XMVector3Rotate(XMVectorSet(0, 0, -1, 0), twist);
	XMVECTOR q2 = XMQuaternionNormalize(XMQuaternionMultiply(qx, qz));

	//p("thetaX: " + std::to_string(thetaX) + "\n");
	//p("thetaY: " + std::to_string(thetaY) + "\n");
	//p("front: " + std::to_string(XMVectorGetX(front)) + " " + std::to_string(XMVectorGetY(front)) + " " + std::to_string(XMVectorGetZ(front)) + "\n");

	XMVECTOR v1 = XMVector3Rotate(front, qx);
	XMVECTOR v2 = XMVector3Rotate(front, q2);

	//p("v1: " + std::to_string(XMVectorGetX(v1)) + " " + std::to_string(XMVectorGetY(v1)) + " " + std::to_string(XMVectorGetZ(v1)) + "\n");
	//p("v2: " + std::to_string(XMVectorGetX(v2)) + " " + std::to_string(XMVectorGetY(v2)) + " " + std::to_string(XMVectorGetZ(v2)) + "\n");

	XMVECTOR axis = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), qthetaX);
	float dotV1V2 = XMVectorGetX(XMVector3Dot(v1, v2));
	yAngle = safeAcosf(dotV1V2);
	XMVECTOR crossVal = XMVector3Cross(v1, v2);
	if (XMVectorGetX(XMVector3Dot(axis, crossVal)) > 0.0f) { yAngle = -yAngle; }


	qy = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), yAngle);

	//p("xAngle: " + std::to_string(xAngle) + "\n");
	//p("yAngle: " + std::to_string(yAngle) + "\n");
	//p("zAngle: " + std::to_string(zAngle) + "\n");

	XMVECTOR qFinal =  XMQuaternionNormalize(XMQuaternionMultiply(XMQuaternionMultiply(XMQuaternionMultiply(twist, qy), qx), qz));

	return qFinal;  // x 먼저, z 나중
}

XMVECTOR IKManager::ClampSwingBySphericalPolygon(XMVECTOR& swing, XMVECTOR& twist, XMVECTOR twistAxis, const std::vector<XMVECTOR>& polygon)
{
	twistAxis = XMVector3Normalize(twistAxis);
	XMVECTOR D = XMVector3Rotate(twistAxis, swing);  // swing에 의해 이동된 방향
	XMVECTOR D_clamped = ClampDirectionToSphericalPolygon(D, polygon);
	//p("D: " + std::to_string(XMVectorGetX(D)) + " " + std::to_string(XMVectorGetY(D)) + " " + std::to_string(XMVectorGetZ(D)) + "\n");
	//p("D_clamped: " + std::to_string(XMVectorGetX(D_clamped)) + " " + std::to_string(XMVectorGetY(D_clamped)) + " " + std::to_string(XMVectorGetZ(D_clamped)) + "\n");
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

void IKManager::resetValuesForIK(RaycastingManager& raycastingManager, Skeleton& skeleton, float walkPhase, Pose& pose, XMMATRIX& worldMatrix)
{
	int chainCount = m_chains.size();

	for (int i = 0; i < chainCount; ++i)
	{
		m_chains[i].start = true;
	}
	
	initLeftFootChainInfo(raycastingManager, skeleton, pose, worldMatrix);
	initRightFootChainInfo(raycastingManager, skeleton, pose, worldMatrix);
}

XMVECTOR IKManager::getQuatFromTo(XMFLOAT3 from, XMFLOAT3 to)
{
	XMVECTOR vFrom = XMVector3Normalize(XMLoadFloat3(&from)); // 예: (0, 1, 0)
	XMVECTOR vTo = XMVector3Normalize(XMLoadFloat3(&to)); // 예: (0, 1, 0)

	float dot = XMVectorGetX(XMVector3Dot(vFrom, vTo));

	XMVECTOR quat;

	if (dot > 0.9999f)
	{
		return XMQuaternionIdentity(); // 동일 방향
	}
	else
	{
		XMVECTOR axis = XMVector3Normalize(XMVector3Cross(vFrom, vTo));
		float angle = safeAcosf(dot);
		return XMQuaternionRotationAxis(axis, angle);
	}
}

void IKManager::initLeftFootChainInfo(RaycastingManager& raycastingManager, Skeleton& skeleton, Pose& pose, XMMATRIX& worldMatrix)
{
	IKChain& chain = m_chains[0];
	RaycastingInfo& raycastingInfo = raycastingManager.m_LeftFoot;

	// 타겟 설정
	chain.Target = raycastingInfo.target;

	XMVECTOR normalQuat = getQuatFromTo(XMFLOAT3(0, 1, 0), raycastingInfo.normal);
	XMVECTOR local = XMVectorSet(0, 0, 0, 1);

	{
		// 기존 애니메이션의 Foot 방향벡터 계산 (Normal 차이 적용)
		XMMATRIX transform = XMMatrixMultiply(pose.world[chain.Bones[2].idx], worldMatrix);
		XMVECTOR worldFoot = XMVector3TransformCoord(local, transform);
		worldFoot = XMVector3Rotate(worldFoot, normalQuat);
		XMStoreFloat3(&chain.Bones[2].worldDest, worldFoot);

		XMMATRIX transformChild = XMMatrixMultiply(pose.world[chain.Bones[2].childIdx], worldMatrix);
		XMVECTOR worldToeBase = XMVector3TransformCoord(local, transformChild);
		worldToeBase = XMVector3Rotate(worldToeBase, normalQuat);
		XMStoreFloat3(&chain.Bones[2].worldChildDest, worldToeBase);
	}

	{
		// 기존 애니메이션의 ToeBase 방향벡터 계산 (Normal 차이 적용)
		XMMATRIX transform = XMMatrixMultiply(pose.world[chain.Bones[3].idx], worldMatrix);
		XMVECTOR worldToeBase = XMVector3TransformCoord(local, transform);
		worldToeBase = XMVector3Rotate(worldToeBase, normalQuat);
		XMStoreFloat3(&chain.Bones[3].worldDest, worldToeBase);

		XMMATRIX transformChild = XMMatrixMultiply(pose.world[chain.Bones[3].childIdx], worldMatrix);
		XMVECTOR worldToeEnd = XMVector3TransformCoord(local, transformChild);
		worldToeEnd = XMVector3Rotate(worldToeEnd, normalQuat);
		XMStoreFloat3(&chain.Bones[3].worldChildDest, worldToeEnd);
	}

	chain.EndEffectorIdx = skeleton.GetBoneIndex("mixamorig:LeftToeBase");

	// UPLEG x, y, z
	W[0] = 3.0f;
	W[1] = 3.0f;
	W[2] = 0.3f;

	// LEG x, y, z
	W[3] = 3.0f;
	W[4] = 3.0f;
	W[5] = 1.0f;

	// FOOT x, y, z
	W[6] = 0.0f;
	W[7] = 0.0f;
	W[8] = 0.0f;

	// TOEBASE x, y, z
	W[9] = 0.0f;
	W[10] = 0.0f;
	W[11] = 0.0f;
}

void IKManager::initRightFootChainInfo(RaycastingManager& raycastingManager, Skeleton& skeleton, Pose& pose, XMMATRIX& worldMatrix)
{
	IKChain& chain = m_chains[1];
	RaycastingInfo& raycastingInfo = raycastingManager.m_RightFoot;

	// 타겟 설정
	chain.Target = raycastingInfo.target;

	XMVECTOR normalQuat = getQuatFromTo(XMFLOAT3(0, 1, 0), raycastingInfo.normal);
	XMVECTOR local = XMVectorSet(0, 0, 0, 1);

	{
		// 기존 애니메이션의 Foot 방향벡터 계산 (Normal 차이 적용)
		XMMATRIX transform = XMMatrixMultiply(pose.world[chain.Bones[2].idx], worldMatrix);
		XMVECTOR worldFoot = XMVector3TransformCoord(local, transform);
		worldFoot = XMVector3Rotate(worldFoot, normalQuat);
		XMStoreFloat3(&chain.Bones[2].worldDest, worldFoot);

		XMMATRIX transformChild = XMMatrixMultiply(pose.world[chain.Bones[2].childIdx], worldMatrix);
		XMVECTOR worldToeBase = XMVector3TransformCoord(local, transformChild);
		worldToeBase = XMVector3Rotate(worldToeBase, normalQuat);
		XMStoreFloat3(&chain.Bones[2].worldChildDest, worldToeBase);
	}

	{
		// 기존 애니메이션의 ToeBase 방향벡터 계산 (Normal 차이 적용)
		XMMATRIX transform = XMMatrixMultiply(pose.world[chain.Bones[3].idx], worldMatrix);
		XMVECTOR worldToeBase = XMVector3TransformCoord(local, transform);
		worldToeBase = XMVector3Rotate(worldToeBase, normalQuat);
		XMStoreFloat3(&chain.Bones[3].worldDest, worldToeBase);

		XMMATRIX transformChild = XMMatrixMultiply(pose.world[chain.Bones[3].childIdx], worldMatrix);
		XMVECTOR worldToeEnd = XMVector3TransformCoord(local, transformChild);
		worldToeEnd = XMVector3Rotate(worldToeEnd, normalQuat);
		XMStoreFloat3(&chain.Bones[3].worldChildDest, worldToeEnd);
	}

	chain.EndEffectorIdx = skeleton.GetBoneIndex("mixamorig:RightToeBase");

	// UPLEG x, y, z
	W[12] = 3.0f;
	W[13] = 3.0f;
	W[14] = 0.3f;

	// LEG x, y, z
	W[15] = 3.0f;
	W[16] = 3.0f;
	W[17] = 1.0f;

	// FOOT x, y, z
	W[18] = 0.0f;
	W[19] = 0.0f;
	W[20] = 0.0f;

	// TOEBASE x, y, z
	W[21] = 0.0f;
	W[22] = 0.0f;
	W[23] = 0.0f;
}

void IKManager::leftFootChainBufferUpdate(IKChain& chain, bool start)
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

	// 2. start == true인 경우 위에서 부터 발을 내림
	if (start == true)
	{
		IKBone& upLegBone = chain.Bones[0];
		IKBone& legBone = chain.Bones[1];

		upLegBone.angleBuffer.xMax = upLegBone.xMin + 0.01f;
		upLegBone.angleBuffer.xMin = upLegBone.xMin;
		upLegBone.angleBuffer.zMax = upLegBone.zMax + 0.01f;
		upLegBone.angleBuffer.zMin = upLegBone.zMax;

		legBone.angleBuffer.xMax = legBone.xMax + 0.01f;
		legBone.angleBuffer.xMin = legBone.xMax;
		legBone.angleBuffer.zMax = legBone.zMax + 0.01f;
		legBone.angleBuffer.zMin = legBone.zMax;
	}
}


void IKManager::rightFootChainBufferUpdate(IKChain& chain, bool start)
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

	// 2. start == true인 경우 위에서 부터 발을 내림
	if (start == true)
	{
		IKBone& upLegBone = chain.Bones[0];
		IKBone& legBone = chain.Bones[1];

		upLegBone.angleBuffer.xMax = upLegBone.xMin + 0.01f;
		upLegBone.angleBuffer.xMin = upLegBone.xMin;
		upLegBone.angleBuffer.zMax = upLegBone.zMin + 0.01f;
		upLegBone.angleBuffer.zMin = upLegBone.zMin;

		legBone.angleBuffer.xMax = legBone.xMax + 0.01f;
		legBone.angleBuffer.xMin = legBone.xMax;
		legBone.angleBuffer.zMax = legBone.zMin + 0.01f;
		legBone.angleBuffer.zMin = legBone.zMin;
	}
}

float IKManager::safeAcosf(float dotResult)
{
	return acosf(std::clamp(dotResult, -1.0f, 1.0f));
}
