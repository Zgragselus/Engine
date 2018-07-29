#ifndef __SIMULATION__H__
#define __SIMULATION__H__

#include "Core/Math/Numeric/Float4.h"
#include "Core/Math/Numeric/Quat.h"
#include <vector>
#include "Bullet/include/btBulletDynamicsCommon.h"
#include "Shapes/Shape.h"
#include "Shapes/Compound/Compound.h"
#include "Shapes/Primitive/Box.h"
#include "Shapes/Primitive/Capsule.h"
#include "Shapes/Primitive/Cone.h"
#include "Shapes/Primitive/Cylinder.h"
#include "Shapes/Primitive/Sphere.h"
#include "Shapes/Mesh/ConvexHull.h"
#include "Shapes/Mesh/TriangleMesh.h"
#include "Body/RigidBody.h"
#include "Game/Entity/Entity.h"

namespace Engine
{
	class Simulation
	{
	private:
		btBroadphaseInterface* mBroadphase;					//
		btDefaultCollisionConfiguration* mCollisionConf;	//
		btCollisionDispatcher* mCollisionDispatcher;		//
		btSequentialImpulseConstraintSolver* mSolver;		//
		btDiscreteDynamicsWorld* mWorld;					// 

		std::vector<RigidBody*> mRigidBodies;
		std::vector<btCollisionShape*> mShapes;
		std::vector<btTriangleMesh*> mTriMeshes;

		bool mRunning;

	public:
		Simulation()
		{
			mRunning = false;
		}

		~Simulation()
		{

		}

		void Init()
		{
			mBroadphase = new btDbvtBroadphase();
			mCollisionConf = new btDefaultCollisionConfiguration();
			mCollisionDispatcher = new btCollisionDispatcher(mCollisionConf);
			mSolver = new btSequentialImpulseConstraintSolver();
			mWorld = new btDiscreteDynamicsWorld(mCollisionDispatcher, mBroadphase, mSolver, mCollisionConf);

			SetGravity(Engine::float4(0.0f, -20.0f, 0.0f, 0.0f));
		}

		void Shutdown()
		{
			for (size_t i = 0; i < mTriMeshes.size(); i++)
			{
				delete mTriMeshes[i];
			}
			mTriMeshes.clear();

			for (size_t i = 0; i < mShapes.size(); i++)
			{
				delete mShapes[i];
			}
			mShapes.clear();

			for (size_t i = 0; i < mRigidBodies.size(); i++)
			{
				delete mRigidBodies[i]->GetBody()->getMotionState();
				delete mRigidBodies[i]->GetBody();
				mRigidBodies[i]->SetBody(nullptr);
			}
			mRigidBodies.clear();

			delete mWorld;
			delete mSolver;
			delete mCollisionDispatcher;
			delete mCollisionConf;
			delete mBroadphase;
		}

		inline void StartSimulation()
		{
			mRunning = true;
		}

		inline void StepSimulation(float deltaTime)
		{
			if (mRunning)
			{
				mWorld->stepSimulation(deltaTime);

				for (size_t i = 0; i < mRigidBodies.size(); i++)
				{
					btTransform trans;
					mRigidBodies[i]->GetBody()->getMotionState()->getWorldTransform(trans);

					btVector3 origin = trans.getOrigin();
					btQuaternion orientation = trans.getRotation();
					mRigidBodies[i]->UpdateTransform(float4(origin.getX(), origin.getY(), origin.getZ(), 1.0f), quat(orientation.getX(), orientation.getY(), orientation.getZ(), orientation.getW()));
				}
			}
		}

		inline void StopSimulation()
		{
			mRunning = false;
		}

		inline bool IsRunning()
		{
			return mRunning;
		}

		inline void SetGravity(const float4& gravity)
		{
			mWorld->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
		}

		inline void AddRigidBody(Shape* shape, RigidBody* body)
		{
			btCollisionShape* shp;

			switch (shape->GetType())
			{
			case Shape::Type::BOX:
				shp = new btBoxShape(btVector3(((Box*)shape)->GetExtents().x, ((Box*)shape)->GetExtents().y, ((Box*)shape)->GetExtents().z));
				break;

			case Shape::Type::CAPSULE:
				shp = new btCapsuleShape(((Capsule*)shape)->GetRadius(), ((Capsule*)shape)->GetHeight());
				break;

			case Shape::Type::COMPOUND:
				break;

			case Shape::Type::CONE:
				shp = new btConeShape(((Cone*)shape)->GetRadius(), ((Cone*)shape)->GetHeight());
				break;

			case Shape::Type::CONVEX_HULL:
				break;

			case Shape::Type::CYLINDER:
				shp = new btCylinderShape(btVector3(((Cylinder*)shape)->GetHalfExtents().x, ((Cylinder*)shape)->GetHalfExtents().y, ((Cylinder*)shape)->GetHalfExtents().z));
				break;

			case Shape::Type::SPHERE:
				shp = new btSphereShape(((Sphere*)shape)->GetRadius());
				break;

			case Shape::Type::TRIANGLE_MESH:
				{
					TriangleMesh* tm = ((TriangleMesh*)shape);
					btTriangleMesh* trimesh = new btTriangleMesh();

					size_t offset = tm->mVertexSize / sizeof(float);
					for (int i = 0; i < tm->mNumTriangles; i++)
					{
						unsigned int idx[3] = { tm->mIndices[i * 3 + 0], tm->mIndices[i * 3 + 1], tm->mIndices[i * 3 + 2] };
						
						trimesh->addTriangle(btVector3(tm->mVertices[idx[0] * offset + 0], tm->mVertices[idx[0] * offset + 1], tm->mVertices[idx[0] * offset + 2]),
							btVector3(tm->mVertices[idx[1] * offset + 0], tm->mVertices[idx[1] * offset + 1], tm->mVertices[idx[1] * offset + 2]),
							btVector3(tm->mVertices[idx[2] * offset + 0], tm->mVertices[idx[2] * offset + 1], tm->mVertices[idx[2] * offset + 2]), true);
					}
					mTriMeshes.push_back(trimesh);

					shp = new btBvhTriangleMeshShape(trimesh, true, true);
				}
				break;
			}

			mShapes.push_back(shp);

			btDefaultMotionState* ms = new btDefaultMotionState(
				btTransform(btQuaternion(body->GetRotation().x, body->GetRotation().y, body->GetRotation().z, body->GetRotation().w),
					btVector3(body->GetTranslation().x, body->GetTranslation().y, body->GetTranslation().z)));

			btVector3 inertia = btVector3(0.0f, 0.0f, 0.0f);
			if (shape->GetType() != Shape::Type::TRIANGLE_MESH)
			{
				shp->calculateLocalInertia(body->GetMass(), inertia);
			}

			btRigidBody::btRigidBodyConstructionInfo bodyInfo = btRigidBody::btRigidBodyConstructionInfo(body->GetMass(), ms, shp, inertia);
			btRigidBody* rb = new btRigidBody(bodyInfo);

			body->SetBody(rb);
			mRigidBodies.push_back(body);

			mWorld->addRigidBody(rb);
		}
	};
}

#endif
