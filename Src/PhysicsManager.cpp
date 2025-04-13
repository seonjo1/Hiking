#include "PhysicsManager.h"

void PhysicsManager::shutdown() {
	if (m_Scene) {
		m_Scene->release(); // PxScene ����
	}
	if (m_Physics) {
		m_Physics->release(); // PxPhysics ����
	}
	if (m_Pvd) {
		m_Pvd->release(); // PxPvd ����
	}
	if (m_Foundation) {
		m_Foundation->release(); // PxFoundation ����
	}
}

bool PhysicsManager::initialize() {
	// PxFoundation ��ü ����
	m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);
	if (!m_Foundation) {
		p("PxCreateFoundation failed!\n");
		return false;
	}

	// PVD(PhysX Visual Debugger) �ʱ�ȭ (������)
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	if (!transport) {
		return false;
	}
	m_Pvd = PxCreatePvd(*m_Foundation);
	m_Pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

	
	physx::PxTolerancesScale       mToleranceScale;
	mToleranceScale.length = 100;
	mToleranceScale.speed = 981;

	// ���� ���� ����
	m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, mToleranceScale, true, m_Pvd);
	if (!m_Physics) {
		p("PxCreatePhysics failed!\n");
		return false;
	}

	// ���� �ùķ��̼� �� ����
	physx::PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f); // �߷� ����
														   
    // CPU ����ó ����
	m_Dispatcher = physx::PxDefaultCpuDispatcherCreate(2); // 2�� CPU �ھ� ��
	if (!m_Dispatcher) {
		p("PxDefaultCpuDispatcherCreate failed!\n");
		return false;
	}

	sceneDesc.cpuDispatcher = m_Dispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	m_Scene = m_Physics->createScene(sceneDesc);
	if (!m_Scene) {
		p("PxPhysics::createScene failed!\n");
		return false;
	}

	physx::PxPvdSceneClient* pvdClient = m_Scene->getScenePvdClient();
	if (pvdClient)
	{
		// ���� ����, �浹 �� ���� ������ PVD�� ����
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	return true;
}

void PhysicsManager::stepSimulation(float deltaTime) {
	m_Scene->simulate(deltaTime);
	m_Scene->fetchResults(true);
}
