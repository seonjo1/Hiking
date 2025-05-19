#include "PhysicsObject.h"

void PhysicsObject::createDynamicObject(physx::PxPhysics* physics) {
	// 동적 물리 객체 생성
	physx::PxTransform transform(physx::PxVec3(0.0f), physx::PxQuat(0.0f, 0.0f, 0.0f, 1.0f));
	physx::PxRigidDynamic* dynamicActor = physics->createRigidDynamic(transform);
	m_actor = dynamicActor;
	isDynamic = true;
}

void PhysicsObject::createStaticObject(physx::PxPhysics* physics) {
	// 정적 물리 객체 생성
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

void PhysicsObject::initSphereMesh(std::vector<physx::PxVec3>& vertices, std::vector<physx::PxU32>& indices)
{
	uint32_t latiSegmentCount = 16;
	uint32_t longiSegmentCount = 32;

	uint32_t circleVertCount = longiSegmentCount + 1;
	vertices.resize((latiSegmentCount + 1) * circleVertCount);
	for (uint32_t i = 0; i <= latiSegmentCount; i++)
	{
		float v = (float)i / (float)latiSegmentCount;
		float phi = (v - 0.5f) * XM_PI;
		auto cosPhi = cosf(phi);
		auto sinPhi = sinf(phi);
		for (uint32_t j = 0; j <= longiSegmentCount; j++)
		{
			float u = (float)j / (float)longiSegmentCount;
			float theta = u * XM_PI * 2.0f;
			auto cosTheta = cosf(theta);
			auto sinTheta = sinf(theta);
			vertices[i * circleVertCount + j] = physx::PxVec3(cosPhi * cosTheta * 0.5f, sinPhi * 0.5f, -cosPhi * sinTheta * 0.5f);
		}
	}

	indices.resize(latiSegmentCount * longiSegmentCount * 6);
	for (uint32_t i = 0; i < latiSegmentCount; i++)
	{
		for (uint32_t j = 0; j < longiSegmentCount; j++)
		{
			uint32_t vertexOffset = i * circleVertCount + j;
			uint32_t indexOffset = (i * longiSegmentCount + j) * 6;
			indices[indexOffset] = vertexOffset;
			indices[indexOffset + 1] = vertexOffset + 1;
			indices[indexOffset + 2] = vertexOffset + 1 + circleVertCount;
			indices[indexOffset + 3] = vertexOffset;
			indices[indexOffset + 4] = vertexOffset + 1 + circleVertCount;
			indices[indexOffset + 5] = vertexOffset + circleVertCount;
		}
	}
}

//void PhysicsObject::createMeshCollider(physx::PxPhysics* physics, std::vector<physx::PxVec3>& vertices, std::vector<physx::PxU32>& indices, physx::PxMeshScale meshScale)
//{
//	// 1. ConvexMesh 설명자 준비
//	physx::PxConvexMeshDesc convexDesc{};
//	convexDesc.points.count = static_cast<uint32_t>(vertices.size());
//	convexDesc.points.stride = sizeof(physx::PxVec3);
//	convexDesc.points.data = vertices.data();
//	convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;
//
//	// 2. Cooking 파라미터 설정
//	physx::PxCookingParams cookParams(physics->getTolerancesScale());
//	cookParams.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eWELD_VERTICES;
//	cookParams.meshWeldTolerance = 0.0001f;
//
//	// 3. ConvexMesh 생성
//	physx::PxDefaultMemoryOutputStream outStream;
//	if (!PxCookConvexMesh(cookParams, convexDesc, outStream)) {
//		p("CookConvexMesh failed!\n");
//		return;
//	}
//
//	physx::PxDefaultMemoryInputData inputStream(outStream.getData(), outStream.getSize());
//	physx::PxConvexMesh* convexMesh = physics->createConvexMesh(inputStream);
//	if (!convexMesh) {
//		p("createConvexMesh failed!\n");
//		return;
//	}
//
//	// 4. ConvexMeshGeometry로 Shape 생성 및 부착
//	physx::PxConvexMeshGeometry geometry(convexMesh, meshScale);
//	m_shape = physics->createShape(geometry, *m_material);
//	m_actor->attachShape(*m_shape);
//}


void PhysicsObject::createMeshCollider(physx::PxPhysics* physics, std::vector<physx::PxVec3>& vertices, std::vector<physx::PxU32>& indices, physx::PxMeshScale meshScale)
{
	physx::PxTriangleMeshDesc meshDesc{};
	meshDesc.points.count = static_cast<uint32_t>(vertices.size());
	meshDesc.points.stride = sizeof(physx::PxVec3);
	meshDesc.points.data = vertices.data();

	meshDesc.triangles.count = static_cast<uint32_t>(indices.size() / 3);
	meshDesc.triangles.stride = sizeof(uint32_t) * 3;
	meshDesc.triangles.data = indices.data();

	physx::PxCookingParams cookParams(physics->getTolerancesScale());
	cookParams.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eWELD_VERTICES;
	cookParams.meshWeldTolerance = 0.0001f;  // 너무 가까운 정점 병합 방지용


	physx::PxDefaultMemoryOutputStream outStream;
	bool result = PxCookTriangleMesh(cookParams, meshDesc, outStream);

	if (!result) {
		p("CookTriangleMesh failed!\n");
		return;
	}

	physx::PxDefaultMemoryInputData inputStream(outStream.getData(), outStream.getSize());
	physx::PxTriangleMesh* triMesh = physics->createTriangleMesh(inputStream);

	physx::PxTriangleMeshGeometry geometry(triMesh, meshScale);
	m_shape = physics->createShape(geometry, *m_material);
	m_actor->attachShape(*m_shape);
}

void PhysicsObject::createSphereShape(physx::PxPhysics* physics, physx::PxMeshScale meshScale)
{
	static std::vector<physx::PxVec3> vertices;
	static std::vector <uint32_t> indices;
	static bool init = false;
	
	if (init != true)
	{
		initSphereMesh(vertices, indices);
	}

	createMeshCollider(physics, vertices, indices, meshScale);
	m_collider = ECollider::SPHERE;
}

void PhysicsObject::createBoxShape(physx::PxPhysics* physics, physx::PxMeshScale meshScale)
{
	static std::vector<physx::PxVec3> vertices = {
		physx::PxVec3(-0.5f, -0.5f, -0.5f), physx::PxVec3(0.5f, -0.5f, -0.5f),
		physx::PxVec3(0.5f, 0.5f, -0.5f), physx::PxVec3(-0.5f, 0.5f, -0.5f),
		physx::PxVec3(-0.5f, -0.5f, 0.5f), physx::PxVec3(0.5f, -0.5f, 0.5f),
		physx::PxVec3(0.5f, 0.5f, 0.5f), physx::PxVec3(-0.5f, 0.5f, 0.5f),
		physx::PxVec3(-0.5f, 0.5f, 0.5f), physx::PxVec3(-0.5f, 0.5f, -0.5f),
		physx::PxVec3(-0.5f, -0.5f, -0.5f), physx::PxVec3(-0.5f, -0.5f, 0.5f),
		physx::PxVec3(0.5f, 0.5f, 0.5f), physx::PxVec3(0.5f, 0.5f, -0.5f),
		physx::PxVec3(0.5f, -0.5f, -0.5f), physx::PxVec3(0.5f, -0.5f, 0.5f),
		physx::PxVec3(-0.5f, -0.5f, -0.5f), physx::PxVec3(0.5f, -0.5f, -0.5f),
		physx::PxVec3(0.5f, -0.5f, 0.5f), physx::PxVec3(-0.5f, -0.5f, 0.5f),
		physx::PxVec3(-0.5f, 0.5f, -0.5f), physx::PxVec3(0.5f, 0.5f, -0.5f),
		physx::PxVec3(0.5f, 0.5f, 0.5f), physx::PxVec3(-0.5f, 0.5f, 0.5f),
	};

	static std::vector<uint32_t> indices = {
		0,	2,	1,	2,	0,	3,	4,	5,	6,	6,	7,	4,	8,	9,	10, 10, 11, 8,
		12, 14, 13, 14, 12, 15, 16, 17, 18, 18, 19, 16, 20, 22, 21, 22, 20, 23,
	};

	createMeshCollider(physics, vertices, indices, meshScale);
	m_collider = ECollider::BOX;

	// 충돌체 생성 (예: 박스 형태)
	//physx::PxBoxGeometry boxGeometry(scale.x, scale.y, scale.z);  // 크기 설정
	//m_shape = physics->createShape(boxGeometry, *m_material);  // 재질을 설정하여 충돌체 생성
	//m_shape->setContactOffset(0.01f);
	//m_actor->attachShape(*m_shape);
}

void PhysicsObject::setMass(float mass)
{
	this->mass = mass;
	physx::PxRigidDynamic* dynamicActor = static_cast<physx::PxRigidDynamic*>(m_actor);
	physx::PxRigidBodyExt::updateMassAndInertia(*dynamicActor, mass);
}

void PhysicsObject::updateMass()
{
	physx::PxRigidDynamic* dynamicActor = static_cast<physx::PxRigidDynamic*>(m_actor);
	physx::PxRigidBodyExt::updateMassAndInertia(*dynamicActor, this->mass);
}

void PhysicsObject::createCapsuleShape(physx::PxPhysics* physics, float radius, float halfHeight)
{
	m_shape = physx::PxRigidActorExt::createExclusiveShape(*m_actor, physx::PxCapsuleGeometry(radius, halfHeight), *m_material);
	m_shape->setLocalPose(physx::PxTransform(physx::PxQuat(physx::PxPi / 2, physx::PxVec3(0, 0, 1))));
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
		switch (m_collider) {
		case ECollider::BOX:
		{
			physx::PxMeshScale meshScale(1.0f);
			meshScale.scale.x *= newScale.x;
			meshScale.scale.y *= newScale.y;
			meshScale.scale.z *= newScale.z;
			m_actor->detachShape(*m_shape);
			m_shape->release();
			createBoxShape(physics, meshScale);
			break;
		}
		case ECollider::SPHERE:
		{
			physx::PxMeshScale meshScale(1.0f);
			meshScale.scale.x *= newScale.x;
			meshScale.scale.y *= newScale.y;
			meshScale.scale.z *= newScale.z;
			m_actor->detachShape(*m_shape);
			m_shape->release();
			createSphereShape(physics, meshScale);
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
		m_actor->release();  // RigidActor 해제
	}
	if (m_shape) {
		m_shape->release();  // Shape 해제
	}
	if (m_material) {
		m_material->release();  // Material 해제
	}
}