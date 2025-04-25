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
	m_chains[idx].Bones[0].yMax = skeleton.bones[boneIdx].yMax;
	m_chains[idx].Bones[0].yMin = skeleton.bones[boneIdx].yMin;
	m_chains[idx].Bones[0].zMax = skeleton.bones[boneIdx].zMax;
	m_chains[idx].Bones[0].zMin = skeleton.bones[boneIdx].zMin;
	m_chains[idx].Bones[0].axis = skeleton.bones[boneIdx].axis;

	// LeftFoot
	boneIdx = skeleton.GetBoneIndex("mixamorig:LeftFoot");
	m_chains[idx].Bones[1].idx = boneIdx;
	m_chains[idx].Bones[1].parentsIdx = skeleton.bones[boneIdx].parentIndex;
	m_chains[idx].Bones[1].xMax = skeleton.bones[boneIdx].xMax;
	m_chains[idx].Bones[1].xMin = skeleton.bones[boneIdx].xMin;
	m_chains[idx].Bones[1].yMax = skeleton.bones[boneIdx].yMax;
	m_chains[idx].Bones[1].yMin = skeleton.bones[boneIdx].yMin;
	m_chains[idx].Bones[1].zMax = skeleton.bones[boneIdx].zMax;
	m_chains[idx].Bones[1].zMin = skeleton.bones[boneIdx].zMin;
	m_chains[idx].Bones[1].axis = skeleton.bones[boneIdx].axis;

	// LeftLeg
	boneIdx = skeleton.GetBoneIndex("mixamorig:LeftLeg");
	m_chains[idx].Bones[2].idx = boneIdx;
	m_chains[idx].Bones[2].parentsIdx = skeleton.bones[boneIdx].parentIndex;
	m_chains[idx].Bones[2].xMax = skeleton.bones[boneIdx].xMax;
	m_chains[idx].Bones[2].xMin = skeleton.bones[boneIdx].xMin;
	m_chains[idx].Bones[2].yMax = skeleton.bones[boneIdx].yMax;
	m_chains[idx].Bones[2].yMin = skeleton.bones[boneIdx].yMin;
	m_chains[idx].Bones[2].zMax = skeleton.bones[boneIdx].zMax;
	m_chains[idx].Bones[2].zMin = skeleton.bones[boneIdx].zMin;
	m_chains[idx].Bones[2].axis = skeleton.bones[boneIdx].axis;

	// LeftUpLeg
	boneIdx = skeleton.GetBoneIndex("mixamorig:LeftUpLeg");
	m_chains[idx].Bones[3].idx = boneIdx;
	m_chains[idx].Bones[3].parentsIdx = skeleton.bones[boneIdx].parentIndex;
	m_chains[idx].Bones[3].xMax = skeleton.bones[boneIdx].xMax;
	m_chains[idx].Bones[3].xMin = skeleton.bones[boneIdx].xMin;
	m_chains[idx].Bones[3].yMax = skeleton.bones[boneIdx].yMax;
	m_chains[idx].Bones[3].yMin = skeleton.bones[boneIdx].yMin;
	m_chains[idx].Bones[3].zMax = skeleton.bones[boneIdx].zMax;
	m_chains[idx].Bones[3].zMin = skeleton.bones[boneIdx].zMin;
	m_chains[idx].Bones[3].axis = skeleton.bones[boneIdx].axis;
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
			XMMATRIX transform = XMMatrixMultiply(pose.world[bone.parentsIdx], worldMatrix);
			bonePos = XMVector3TransformCoord(bonePos, transform);

			XMVECTOR toEffector = XMVectorSubtract(effectPos, bonePos);

			p("bone[" + std::to_string(bone.idx) + "]\n");
			p("bonePos: " + std::to_string(XMVectorGetX(bonePos)) + " " + std::to_string(XMVectorGetY(bonePos)) + " " + std::to_string(XMVectorGetZ(bonePos)) + "\n");
			p("effectPos: " + std::to_string(XMVectorGetX(effectPos)) + " " + std::to_string(XMVectorGetY(effectPos)) + " " + std::to_string(XMVectorGetZ(effectPos)) + "\n");
			p("targetPos: " + std::to_string(XMVectorGetX(targetPos)) + " " + std::to_string(XMVectorGetY(targetPos)) + " " + std::to_string(XMVectorGetZ(targetPos)) + "\n");
			p("toEffector: " + std::to_string(XMVectorGetX(toEffector)) + " " + std::to_string(XMVectorGetY(toEffector)) + " " + std::to_string(XMVectorGetZ(toEffector)) + "\n");

			//p("axse\n");
			for (int k = 0; k < 3; ++k)
			{
				const XMFLOAT3& axis = axes[k];

				// 로컬 회전축 → 월드 회전축
				XMVECTOR localAxis = XMLoadFloat3(&axis);
				XMVECTOR worldAxis = XMVector3TransformNormal(localAxis, transform);
				worldAxis = XMVector3Normalize(worldAxis);

				//XMFLOAT3 debugAxis;
				//XMStoreFloat3(&debugAxis, worldAxis);

				// 위치 변화 = 회전축 × effector 방향
				XMVECTOR dPdTheta = XMVector3Cross(worldAxis, toEffector);
				
				p("worldAxis: " + std::to_string(XMVectorGetX(worldAxis)) + " " + std::to_string(XMVectorGetY(worldAxis)) + " " + std::to_string(XMVectorGetZ(worldAxis)) + "\n");
				p("dPdTheta: " + std::to_string(XMVectorGetX(dPdTheta)) + " " + std::to_string(XMVectorGetY(dPdTheta)) + " " + std::to_string(XMVectorGetZ(dPdTheta)) + "\n");

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
			JTJ.Set(i, j, sum * W[i]);
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
			JTx[i] += W[i] * J.Get(j, i) * dP[j];
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

			// 기존 quaternion을 pitch yaw roll로 변환

			for (int k = 0; k < 3; ++k)
			{
				angle[k] = XMConvertToDegrees(dTheta[idx]);
				idx++;
			}

			p("bone[" + std::to_string(bone.idx) + "]\n");
			p("dthetaX: " + std::to_string(angle[0]) + "\n");
			p("dthetaY: " + std::to_string(angle[1]) + "\n");
			p("dthetaZ: " + std::to_string(angle[2]) + "\n");

			XMVECTOR qOld = XMLoadFloat4(&m_nowRotation[bone.idx]);

			//로컬 축 구하기
			//XMVECTOR localX = XMVector3Rotate(XMVectorSet(1, 0, 0, 0), qOld);
			//XMVECTOR localY = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), qOld);
			//XMVECTOR localZ = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), qOld);
			XMVECTOR localX = XMVectorSet(1, 0, 0, 0);
			XMVECTOR localY = XMVectorSet(0, 1, 0, 0);
			XMVECTOR localZ = XMVectorSet(0, 0, 1, 0);

			//p("localX: " + std::to_string(XMVectorGetX(localX)) + " " + std::to_string(XMVectorGetY(localX)) + " " + std::to_string(XMVectorGetZ(localX)) + "\n");
			//p("localY: " + std::to_string(XMVectorGetX(localY)) + " " + std::to_string(XMVectorGetY(localY)) + " " + std::to_string(XMVectorGetZ(localY)) + "\n");
			//p("localZ: " + std::to_string(XMVectorGetX(localZ)) + " " + std::to_string(XMVectorGetY(localZ)) + " " + std::to_string(XMVectorGetZ(localZ)) + "\n");

			//// 로컬 축 기준 델타 쿼터니언
			XMVECTOR dqX = XMQuaternionRotationAxis(localX, XMConvertToRadians(angle[0]));
			XMVECTOR dqY = XMQuaternionRotationAxis(localY, XMConvertToRadians(angle[1]));
			XMVECTOR dqZ = XMQuaternionRotationAxis(localZ, XMConvertToRadians(angle[2]));
	
			// 합성 및 적용
			XMVECTOR newQuat = XMQuaternionMultiply(XMQuaternionMultiply(dqX, dqY), dqZ);
			XMVECTOR qNew = XMQuaternionNormalize(XMQuaternionMultiply(qOld, newQuat));

			XMStoreFloat4(&m_nowRotation[bone.idx], qNew);

			// debug
			//XMMATRIX xM = XMMatrixRotationQuaternion(dqX);
			//XMMATRIX yM = XMMatrixRotationQuaternion(dqY);
			//XMMATRIX zM = XMMatrixRotationQuaternion(dqZ);
			//XMMATRIX xyzM = XMMatrixRotationQuaternion(newQuat);
			//XMVECTOR xV = XMVector3TransformNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), xM);
			//XMVECTOR yV = XMVector3TransformNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), yM);
			//XMVECTOR zV = XMVector3TransformNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), zM);
			//XMVECTOR xyzV = XMVector3TransformNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), xyzM);
			//p("xV: " + std::to_string(XMVectorGetX(xV)) + " " + std::to_string(XMVectorGetY(xV)) + " " + std::to_string(XMVectorGetZ(xV)) + "\n");
			//p("yV: " + std::to_string(XMVectorGetX(yV)) + " " + std::to_string(XMVectorGetY(yV)) + " " + std::to_string(XMVectorGetZ(yV)) + "\n");
			//p("zV: " + std::to_string(XMVectorGetX(zV)) + " " + std::to_string(XMVectorGetY(zV)) + " " + std::to_string(XMVectorGetZ(zV)) + "\n");
			//p("xyzV: " + std::to_string(XMVectorGetX(xyzV)) + " " + std::to_string(XMVectorGetY(xyzV)) + " " + std::to_string(XMVectorGetZ(xyzV)) + "\n");
			

			// clamping
			clampBoneAngle(bone, m_nowRotation[bone.idx], pose);

			// total Delta
			XMMATRIX oldM = XMMatrixRotationQuaternion(qOld);
			XMMATRIX newM = XMMatrixRotationQuaternion(XMLoadFloat4(&m_nowRotation[bone.idx]));

			XMVECTOR oldV = XMVector3TransformNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), oldM);
			XMVECTOR newV = XMVector3TransformNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), newM);
			float deltaAngle = XMVectorGetX(XMVector3Dot(oldV, newV));
			
			//p("oldV: " + std::to_string(XMVectorGetX(oldV)) + " " + std::to_string(XMVectorGetY(oldV)) + " " + std::to_string(XMVectorGetZ(oldV)) + "\n");
			//p("newV: " + std::to_string(XMVectorGetX(newV)) + " " + std::to_string(XMVectorGetY(newV)) + " " + std::to_string(XMVectorGetZ(newV)) + "\n");

			//p("deltaAngle: " + std::to_string(deltaAngle) + "\n");
			//if (deltaAngle < 1.0f)
			{
				//p("bone[" + std::to_string(bone.idx) + "] is changed!!\n");
				m_chains[i].isChanged = true;
			}

			//if (bone.idx == 57)
			//{
			//	XMVECTOR test = XMQuaternionRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XM_PI);
			//	XMStoreFloat4(&m_nowRotation[bone.idx], test);
			//}
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

	//p("outTwist: " + std::to_string(XMVectorGetX(outTwist)) + " " + std::to_string(XMVectorGetY(outTwist)) + " " + std::to_string(XMVectorGetZ(outTwist)) + " " + std::to_string(XMVectorGetW(outTwist)) + "\n");
	//p("outSwing: " + std::to_string(XMVectorGetX(outSwing)) + " " + std::to_string(XMVectorGetY(outSwing)) + " " + std::to_string(XMVectorGetZ(outSwing)) + " " + std::to_string(XMVectorGetW(outSwing)) + "\n");
	//XMVECTOR out = XMQuaternionMultiply(outSwing, outTwist);
	//p("out: " + std::to_string(XMVectorGetX(out)) + " " + std::to_string(XMVectorGetY(out)) + " " + std::to_string(XMVectorGetZ(out)) + " " + std::to_string(XMVectorGetW(out)) + "\n");

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

void IKManager::clampBoneAngle(IKBone& bone, XMFLOAT4& quat, Pose& pose)
{
	//p("bone " + std::to_string(bone.idx) + " start clamping!!\n");
	// IK로 계산된 최종 회전 쿼터니언
	XMVECTOR qIK = XMLoadFloat4(&quat);

	// local 축 정의
	XMVECTOR twistAxis = XMLoadFloat3(&bone.axis);

	float xMax, xMin, yMax, yMin, zMax, zMin;
	xMax = bone.angleBuffer.xMax;
	xMin = bone.angleBuffer.xMin;
	yMax = bone.angleBuffer.yMax;
	yMin = bone.angleBuffer.yMin;
	zMax = bone.angleBuffer.zMax;
	zMin = bone.angleBuffer.zMin;

	XMVECTOR swing, twist;

	// 기존 quat을 Swing과 Twist quaternion으로 분리
	DecomposeSwingTwist(qIK, twistAxis, swing, twist);

	std::vector<XMVECTOR> polygon;
	XMVECTOR swingClamped, twistClamped;

	// Twist 회전 clamping
	twistClamped = ClampTwist(twist, twistAxis, yMax, yMin);
	twistClamped = XMQuaternionNormalize(twistClamped);

	// Swing 회전 clamping
	makePolygon(polygon, xMax, xMin, zMax, zMin);
	swingClamped = ClampSwingBySphericalPolygon(swing, twistAxis, polygon);

	pose.twist[bone.idx] = twistClamped;

	XMVECTOR qFinal;
	qFinal = XMQuaternionNormalize(XMQuaternionMultiply(swingClamped, twistClamped));

	XMMATRIX qsM = XMMatrixRotationQuaternion(swingClamped);
	XMMATRIX qtM = XMMatrixRotationQuaternion(twistClamped);

	XMVECTOR qsV = XMVector3TransformNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), qsM);
	XMVECTOR qtV = XMVector3TransformNormal(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), qtM);
	XMVECTOR qstV = XMVector3TransformNormal(qsV, qtM);

	//p("qsV: " + std::to_string(XMVectorGetX(qsV)) + " " + std::to_string(XMVectorGetY(qsV)) + " " + std::to_string(XMVectorGetZ(qsV)) + "\n");
	//p("qtV: " + std::to_string(XMVectorGetX(qtV)) + " " + std::to_string(XMVectorGetY(qtV)) + " " + std::to_string(XMVectorGetZ(qtV)) + "\n");
	//p("qstV: " + std::to_string(XMVectorGetX(qstV)) + " " + std::to_string(XMVectorGetY(qstV)) + " " + std::to_string(XMVectorGetZ(qstV)) + "\n");

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

		XMVECTOR edgeNormal = XMVector3Normalize(XMVector3Cross(b, a));
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

XMVECTOR IKManager::ClampSwingBySphericalPolygon(XMVECTOR swing, XMVECTOR twistAxis, const std::vector<XMVECTOR>& polygon)
{
	twistAxis = XMVector3Normalize(twistAxis);
	XMVECTOR D = XMVector3Rotate(twistAxis, swing);  // swing에 의해 이동된 방향
	XMVECTOR D_clamped = ClampDirectionToSphericalPolygon(D, polygon);

	float xAngle, zAngle;

	XMVECTOR Y = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR dXY = XMVector2Normalize(XMVectorSet(XMVectorGetX(D_clamped), XMVectorGetY(D_clamped), 0.0f, 0.0f));

	if (fabs(XMVectorGetX(D_clamped)) < 1e-6f)
	{
		zAngle = 0.0f;
	}
	else
	{
		zAngle = acosf(std::abs(XMVectorGetY(dXY)));
		if (XMVectorGetY(D_clamped) < 0.0f) { zAngle = -zAngle; }
		if (XMVectorGetX(D_clamped) < 0.0f) { zAngle = -zAngle; }
	}

	XMVECTOR qzInv = XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), zAngle);
	XMMATRIX qzInvTransform = XMMatrixRotationQuaternion(qzInv);
	XMVECTOR D_ZRotataed = XMVector3TransformNormal(D_clamped, qzInvTransform);

	XMVECTOR dZY = XMVector2Normalize(XMVectorSet(XMVectorGetZ(D_ZRotataed), XMVectorGetY(D_ZRotataed), 0.0f, 0.0f));
	xAngle = acosf(XMVectorGetX(XMVector2Dot(Y, dZY)));

	if (XMVectorGetZ(D_ZRotataed) < 0.0f) { xAngle = -xAngle; }

	XMVECTOR qy = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), zAngle);
	XMVECTOR qx = XMQuaternionRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), xAngle);
	XMVECTOR qz = XMQuaternionRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), -zAngle);

	XMVECTOR clampedSwing = XMQuaternionNormalize(XMQuaternionMultiply(qy, XMQuaternionMultiply(qx, qz)));  // x 먼저, z 나중

	p("D: " + std::to_string(XMVectorGetX(D)) + " " + std::to_string(XMVectorGetY(D)) + " " + std::to_string(XMVectorGetZ(D)) + "\n");
	p("D_clamped: " + std::to_string(XMVectorGetX(D_clamped)) + " " + std::to_string(XMVectorGetY(D_clamped)) + " " + std::to_string(XMVectorGetZ(D_clamped)) + "\n");
	//p("xAngle: " + std::to_string(xAngle) + "\n");
	//p("zAngle: " + std::to_string(zAngle) + "\n");

	//XMMATRIX qxM = XMMatrixRotationQuaternion(qx);
	//XMMATRIX qzM = XMMatrixRotationQuaternion(qz);

	//XMVECTOR qxV = XMVector3TransformNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), qxM);
	//XMVECTOR qzV = XMVector3TransformNormal(qxV, qzM);


	//p("qxV: " + std::to_string(XMVectorGetX(qxV)) + " " + std::to_string(XMVectorGetY(qxV)) + " " + std::to_string(XMVectorGetZ(qxV)) + "\n");
	//p("qzV: " + std::to_string(XMVectorGetX(qzV)) + " " + std::to_string(XMVectorGetY(qzV)) + " " + std::to_string(XMVectorGetZ(qzV)) + "\n");

	//XMVECTOR qxz = XMQuaternionNormalize(XMQuaternionMultiply(qx, qz));
	//XMMATRIX qxzM = XMMatrixRotationQuaternion(qxz);
	//XMVECTOR qxzV = XMVector3TransformNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), qxzM);
	//p("qxzV: " + std::to_string(XMVectorGetX(qxzV)) + " " + std::to_string(XMVectorGetY(qxzV)) + " " + std::to_string(XMVectorGetZ(qxzV)) + "\n");

	return clampedSwing;
}

void IKManager::makePolygon(std::vector<XMVECTOR>& polygon, float xMax, float xMin, float zMax, float zMin)
{
	int numSegments = 4;

	//p("makePolygon!!\n");
	for (int i = 0; i < numSegments + 1; ++i)
	{
		float t = (float)i / numSegments;  // 0 ~ 1
		float xDeg = Lerp(xMin, xMax, (cosf(t * XM_2PI) + 1.0f) * 0.5f);
		float zDeg = Lerp(zMin, zMax, (sinf(t * XM_2PI) + 1.0f) * 0.5f);

		XMVECTOR qx = XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), XMConvertToRadians(xDeg));
		XMVECTOR qz = XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), XMConvertToRadians(zDeg));

		XMVECTOR D = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), qx);
		D = XMVector3Rotate(D, qz);

		//p("i : " + std::to_string(i) + "\n");
		//p("D: " + std::to_string(XMVectorGetX(D)) + " " + std::to_string(XMVectorGetY(D)) + " " + std::to_string(XMVectorGetZ(D)) + "\n");

		polygon.push_back(XMVector3Normalize(D));
	}
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

	// UpLeg x, y, z
	if (raycastingInfo.part == EIKPart::FOOT)
	{
		chain.EndEffectorIdx = skeleton.GetBoneIndex(("mixamorig:LeftFoot"));
		W[0] = 0.0f;
		W[1] = 0.0f;
		W[2] = 0.0f;
	}
	else
	{
		chain.EndEffectorIdx = skeleton.GetBoneIndex(("mixamorig:LeftToe_End"));
		W[0] = 1.0f;
		W[1] = 1.0f;
		W[2] = 1.0f;
	}

	// Leg x, y, z
	W[3] = 1.0f;
	W[4] = 1.0f;
	W[5] = 1.0f;

	// Foot x, y, z
	W[6] = 1.0f;
	W[7] = 1.0f;
	W[8] = 1.0f;

	// Toe_Base x, y, z
	W[9] = 1.0f;
	W[10] = 1.0f;
	W[11] = 1.0f;
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
		bone.angleBuffer.yMax = bone.yMax;
		bone.angleBuffer.yMin = bone.yMin;
		bone.angleBuffer.zMax = bone.zMax;
		bone.angleBuffer.zMin = bone.zMin;
	}

	// constraint 시작

	// 1. 발각도 지면에 맞추기 (추가 예정)

	// 2. start == true인 경우 위에서 부터 발을 내림
	if (start == true)
	{
		IKBone& legBone = chain.Bones[2];
		IKBone& upLegBone = chain.Bones[3];

		upLegBone.angleBuffer.xMax = upLegBone.xMin;
		upLegBone.angleBuffer.xMin = upLegBone.xMin;
		legBone.angleBuffer.xMax = legBone.xMax;
		legBone.angleBuffer.xMin = legBone.xMax;
	}

	if (wasChanged == false)
	{
		IKBone& upLegBone = chain.Bones[3];

		upLegBone.angleBuffer.xMax = upLegBone.xMax;
		upLegBone.angleBuffer.xMin = upLegBone.xMax;

	}
}