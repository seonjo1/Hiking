#pragma once
#include <DirectXMath.h>
#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <PxSceneDesc.h>
#include <PxMaterial.h>
#include <PxShape.h>
#include "Common.h"

using namespace DirectX;

class PhysicsManager {
public:
	physx::PxFoundation* m_Foundation = nullptr;
	physx::PxPhysics* m_Physics = nullptr;
	physx::PxScene* m_Scene = nullptr;
	physx::PxPvd* m_Pvd = nullptr;
	physx::PxCpuDispatcher* m_Dispatcher = nullptr;

	PhysicsManager() = default;

	bool initialize();
	void stepSimulation(float deltaTime);
	void shutdown();
};

class PhysicsObject {
public:
	physx::PxRigidActor* m_actor;  // 물리 객체 (RigidBody 등)
	physx::PxShape* m_shape;       // 충돌체
	physx::PxMaterial* m_material; // 물리 재질

	// rigidbody -> material -> shape -> mass
	void createDynamicObject(physx::PxPhysics* physics);
	void createStaticObject(physx::PxPhysics* physics);
	void createSphereShape(physx::PxPhysics* physics, float radius);
	void createBoxShape(physx::PxPhysics* physics, XMFLOAT3 scal);
	void setMass(float mass);
	void setMaterial(physx::PxPhysics* physics, float staticFriction = 0.5f, float dynamicFriction = 0.5f, float restitution = 0.6f);
	void updatePosition(const physx::PxVec3& newPos);
	void updateRotation(const physx::PxQuat& newRot);
	void updateScale(physx::PxPhysics* physics, XMFLOAT3& newScale);
	void addToScene(physx::PxScene* scene);
	void shutdown();
};
