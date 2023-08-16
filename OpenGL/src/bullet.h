#pragma once
#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
#include <BulletDynamics\Character\btKinematicCharacterController.h>
#include <BulletCollision\CollisionDispatch\btGhostObject.h>

class Bt
{
private:
	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	btDefaultCollisionConfiguration* collisionConfiguration;

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher;

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface* overlappingPairCache;

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* solver;

	
public:
	btKinematicCharacterController* characterContronller;
	btDiscreteDynamicsWorld* dynamicsWorld;
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
	Bt()
	{
		collisionConfiguration = new btDefaultCollisionConfiguration();
		dispatcher = new btCollisionDispatcher(collisionConfiguration);
		overlappingPairCache = new btDbvtBroadphase();
		solver = new btSequentialImpulseConstraintSolver;
		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
		dynamicsWorld->setGravity(btVector3(0, -100, 0));
	}
	~Bt()
	{
		for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			//std::cout << i << std::endl;
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);
			if (body && body->getMotionState())
			{
				delete body->getMotionState();
			}
			dynamicsWorld->removeCollisionObject(obj);
			delete obj;
		}

		//delete collision shapes
		for (int j = 0; j < collisionShapes.size(); j++)
		{
			btCollisionShape* shape = collisionShapes[j];
			collisionShapes[j] = 0;
			delete shape;
		}

		//delete dynamics world
		delete dynamicsWorld;

		//delete solver
		delete solver;

		//delete broadphase
		delete overlappingPairCache;

		//delete dispatcher
		delete dispatcher;

		delete collisionConfiguration;

		delete characterContronller;
		//next line is optional: it will be cleared by the destructor when the array goes out of scope
		collisionShapes.clear();
	}
	void addBox(btVector3 origin, btVector3 size)
	{
		btCollisionShape* groundShape = new btBoxShape(size);
		collisionShapes.push_back(groundShape);

		btTransform Transform;
		Transform.setIdentity();
		Transform.setOrigin(origin);
		btDefaultMotionState* myMotionState = new btDefaultMotionState(Transform);
		btScalar mass(0.);

		btVector3 localInertia(0, 0, 0);
		if (mass != 0.f)
			groundShape->calculateLocalInertia(mass, localInertia);


		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);
	}
	void addPlane()
	{
		btStaticPlaneShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
		collisionShapes.push_back(groundShape);

		btTransform Transform;
		Transform.setIdentity();
		Transform.setOrigin(btVector3(0, 0, 0));
		btDefaultMotionState* myMotionState = new btDefaultMotionState(Transform);
		btScalar mass(0.);

		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape);
		btRigidBody* body = new btRigidBody(rbInfo);
		body->setRestitution(btScalar(0.5));
		body->setFriction(btScalar(20));
		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);
	}
	void addTerrain()
	{
		btStaticPlaneShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
		btTriangleMesh* terrainMesh = new btTriangleMesh();
		collisionShapes.push_back(groundShape);

		btTransform Transform;
		Transform.setIdentity();
		Transform.setOrigin(btVector3(0, 0, 0));
		btDefaultMotionState* myMotionState = new btDefaultMotionState(Transform);
		btScalar mass(0.);

		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape);
		btRigidBody* body = new btRigidBody(rbInfo);
		body->setRestitution(btScalar(0.5));
		body->setFriction(btScalar(0.2));
		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);
	}
	void addSphere(float radius = 1.0f, btVector3 origin = btVector3(0, 0, 0), float mass = 1.0f)
	{
		btCollisionShape* colShape = new btSphereShape(btScalar(radius));
		collisionShapes.push_back(colShape);

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(origin);

		btVector3 localInertia(0, 0, 0);
		if (mass != 0.f)
			colShape->calculateLocalInertia(mass, localInertia);

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);
		//body->setRestitution(btScalar(0.1));
		body->setFriction(btScalar(100));
		dynamicsWorld->addRigidBody(body);
	}

	void addCharacterCapsule(float radius = 0.5f, btVector3 origin = btVector3(0, 0, 0), float height = 1.0f, float mass = 5)
	{

		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(origin);

		//capsule
		btConvexShape* capsuleShape = new btCapsuleShape(radius, height);
		collisionShapes.push_back(capsuleShape);

		//ghost
		btPairCachingGhostObject* ghostObject = new btPairCachingGhostObject();
		ghostObject->setWorldTransform(startTransform);
		dynamicsWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

		ghostObject->setCollisionShape(capsuleShape);
		ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
		characterContronller = new btKinematicCharacterController(ghostObject, capsuleShape, 0.35);
		characterContronller->setGravity(btVector3(0, -10, 0));
		characterContronller->setJumpSpeed(10);
		//characterContronller->setMaxJumpHeight(2);
		//characterContronller->setMaxSlope(btScalar(45.0));
		dynamicsWorld->addCollisionObject(ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
		dynamicsWorld->addAction(characterContronller);

	}

	void addCapsule(float radius = 0.5f, btVector3 origin = btVector3(0, 0, 0), float height = 1.0f, float mass = 1.0f)
	{
		btCapsuleShape* capsuleShape = new btCapsuleShape(radius, height);
		collisionShapes.push_back(capsuleShape);

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(origin);

		btVector3 localInertia(0, 0, 0);
		if (mass != 0.f)
			capsuleShape->calculateLocalInertia(mass, localInertia);

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, capsuleShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);
		body->setRestitution(btScalar(0.5));
		dynamicsWorld->addRigidBody(body);
	}

	void addSDF()
	{
		//btWorldImporter importer(dynamicsWorld);
	}
};