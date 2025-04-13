#pragma once

#include <DirectXMath.h>
#include <PxPhysicsAPI.h>
#include <PxMaterial.h>
#include <PxShape.h>
#include <vector>
#include <PxCooking.h>
#include "Common.h"

using namespace DirectX;

enum class ECollider
{
	BOX,
	SPHERE,
};

class PhysicsObject {
public:
	physx::PxRigidActor* m_actor = nullptr;  // 물리 객체 (RigidBody 등)
	physx::PxShape* m_shape = nullptr;       // 충돌체
	physx::PxMaterial* m_material = nullptr; // 물리 재질
	ECollider m_collider;

	// rigidbody -> material -> shape -> mass
	void createDynamicObject(physx::PxPhysics* physics);
	void createStaticObject(physx::PxPhysics* physics);
	void initSphereMesh(std::vector<physx::PxVec3>& vertices, std::vector<physx::PxU32>& indices);
	void createMeshCollider(physx::PxPhysics* physics, std::vector<physx::PxVec3>& vertices, std::vector<physx::PxU32>& indices, physx::PxMeshScale meshScale);
	void createSphereShape(physx::PxPhysics* physics, physx::PxMeshScale meshScale);
	void createBoxShape(physx::PxPhysics* physics, physx::PxMeshScale meshScale);
	void setMass(float mass);
	void setMaterial(physx::PxPhysics* physics, float staticFriction = 0.5f, float dynamicFriction = 0.5f, float restitution = 0.6f);
	void updatePosition(const physx::PxVec3& newPos);
	void updateRotation(const physx::PxQuat& newRot);
	void updateScale(physx::PxPhysics* physics, XMFLOAT3& newScale);
	void addToScene(physx::PxScene* scene);
	void shutdown();
};