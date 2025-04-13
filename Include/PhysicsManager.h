#pragma once

#include <DirectXMath.h>
#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <PxSceneDesc.h>
#include "Common.h"

using namespace DirectX;

class PhysicsManager {
public:
	physx::PxDefaultAllocator m_Allocator;
	physx::PxDefaultErrorCallback m_ErrorCallback;
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

