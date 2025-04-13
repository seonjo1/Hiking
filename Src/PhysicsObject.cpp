#include "PhysicsObject.h"
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
		const physx::PxGeometry& geometry = m_shape->getGeometry();
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