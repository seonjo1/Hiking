#include "Physics.h"

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
	// �⺻ �Ҵ��ڿ� ���� ó���� ����
	physx::PxDefaultAllocator allocator;
	physx::PxDefaultErrorCallback errorCallback;

	// PxFoundation ��ü ����
	m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, allocator, errorCallback);
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


// PhysicsObject!!

void PhysicsObject::createDynamicObject(physx::PxPhysics* physics) {
	// ���� ���� ��ü ����
	physx::PxTransform transform(physx::PxVec3(0.0f), physx::PxQuat(0.0f, 0.0f, 0.0f, 1.0f));
	physx::PxRigidDynamic* dynamicActor = physics->createRigidDynamic(transform);
	m_actor = dynamicActor;
}

void PhysicsObject::createStaticObject(physx::PxPhysics* physics) {
	// ���� ���� ��ü ����
	physx::PxTransform transform(physx::PxVec3(0.0f), physx::PxQuat(0.0f, 0.0f, 0.0f, 1.0f));
	physx::PxRigidStatic* staticActor = physics->createRigidStatic(transform);

	m_actor = staticActor;
}

void PhysicsObject::setMaterial(physx::PxPhysics* physics, float staticFriction, float dynamicFriction, float restitution)
{
	physx::PxReal frictionCombineMode = physx::PxCombineMode::eMULTIPLY;
	physx::PxReal restitutionCombineMode = physx::PxCombineMode::eMULTIPLY;

	m_material = physics->createMaterial(staticFriction, dynamicFriction, restitution);
}

void PhysicsObject::createSphereShape(physx::PxPhysics* physics, float radius)
{
	// �� ������ �浹ü ����
	physx::PxSphereGeometry sphereGeometry(radius);  // ������ ����
	m_shape = physics->createShape(sphereGeometry, *m_material);  // ������ �����Ͽ� �浹ü ����
	m_actor->attachShape(*m_shape);
}

void PhysicsObject::createBoxShape(physx::PxPhysics* physics, XMFLOAT3 scale)
{
	// �浹ü ���� (��: �ڽ� ����)
	physx::PxBoxGeometry boxGeometry(scale.x, scale.y, scale.z);  // ũ�� ����
	m_shape = physics->createShape(boxGeometry, *m_material);  // ������ �����Ͽ� �浹ü ����
	m_actor->attachShape(*m_shape);
}

void PhysicsObject::setMass(float mass)
{
	physx::PxRigidDynamic* dynamicActor = static_cast<physx::PxRigidDynamic*>(m_actor);
	physx::PxRigidBodyExt::updateMassAndInertia(*dynamicActor, mass);
}

void PhysicsObject::updatePosition(const physx::PxVec3& newPos) {
	physx::PxTransform newTransform(newPos);
	m_actor->setGlobalPose(newTransform);
}

void PhysicsObject::updateRotation(const physx::PxQuat& newRot) {
	physx::PxTransform newTransform(m_actor->getGlobalPose().p, newRot);
	m_actor->setGlobalPose(newTransform);
}

void PhysicsObject::updateScale(physx::PxPhysics* physics, XMFLOAT3& newScale)
{
	if (m_shape)
	{
		const physx::PxGeometry &geometry = m_shape->getGeometry();
		physx::PxGeometryType::Enum shapeType = geometry.getType();
		switch (shapeType) {
			case physx::PxGeometryType::eBOX:
			{
				const physx::PxBoxGeometry& boxGeometry = static_cast<const physx::PxBoxGeometry&>(geometry);
				newScale.x *= boxGeometry.halfExtents.x;
				newScale.y *= boxGeometry.halfExtents.y;
				newScale.z *= boxGeometry.halfExtents.z;
				m_actor->detachShape(*m_shape);
				m_shape->release();
				createBoxShape(physics, newScale);
				break;
			}
			case physx::PxGeometryType::eSPHERE:
			{
				const physx::PxSphereGeometry& sphereGeometry = static_cast<const physx::PxSphereGeometry&>(geometry);
				float radius = sphereGeometry.radius * newScale.x;
				m_actor->detachShape(*m_shape);
				m_shape->release();
				createSphereShape(physics, radius);
				break;
			}
			default:
				break;
		}
	}
}


void PhysicsObject::addToScene(physx::PxScene* scene) {
	scene->addActor(*m_actor);
}

void PhysicsObject::shutdown()
{
	if (m_actor) {
		m_actor->release();  // RigidActor ����
	}
	if (m_shape) {
		m_shape->release();  // Shape ����
	}
	if (m_material) {
		m_material->release();  // Material ����
	}
}