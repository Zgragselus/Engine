///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Scene.h
//
// Defines a scene class.
// Scene class is a container for objects, entities. These entities are basically game objects that
// have some purpose in application itself.
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __SCENE__H__
#define __SCENE__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include <map>
#include <stack>
#include "Core/Util/ID.h"
#include "SceneMap.h"
#include "../Entity/Entity.h"
#include "RenderNode.h"
#include "UndoRedo.h"
#include "State.h"
#include "Physics/Simulation.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	///	Scene-class definition
	/// </summary>
	class Scene
	{
	public:
		enum class PhysicsState
		{
			STOPPED = 0,
			RUNNING,
			PAUSED
		};

	private:
		/// <summary>State average position</summary>
		Engine::float4 mSelectionPosition;
		Engine::mat4 mMeanMatrix;
		
		/// <summary>Temporary matrix to show preview of transformation</summary>
		Engine::mat4 mSubstituteMatrix;

		/// <summary>Helper class to assign IDs to newly added entities</summary>
		ID mId;

		/// <summary>Search structure for entities</summary>
		SceneMap<Entity> mSearch;

		/// <summary>Scenegraph</summary>
		Entity* mSceneGraph;

		/// <summary>Undo/Redo class</summary>
		UndoRedo* mUndoRedo;

		/// <summary>Selection state class</summary>
		State* mState;

		/// <summary>Physics simulation</summary>
		Simulation* mSimulation;

		/// <summary>Physics sim state</summary>
		PhysicsState mSimulationState;

		/// <summary>Entities to be removed</summary>
		/// Entities can't be removed right away, but only at sync point after rendering has been finished
		std::vector<int> mToRemove;

		/// <summary>Replacement scene flag</summary>
		/// If set to non-nullptr, the replacement scene has to contain new scene root with which we want to replace 
		Entity* mToReload;

		void RecalculateGizmoPosition()
		{
			mMeanMatrix = mat4(0.0f);
			for (auto it = mState->GetSelection().begin(); it != mState->GetSelection().end(); it++)
			{
				mMeanMatrix = mMeanMatrix + GetEntity(*it)->GetWorldMatrix();
			}
			mMeanMatrix /= (float)mState->GetSelection().size();
			float4* fhelper = (float4*)&mMeanMatrix;
			mSelectionPosition = float4(fhelper[0].w, fhelper[1].w, fhelper[2].w, 1.0f);
		}

	public:
		inline PhysicsState GetSimulationState()
		{
			return mSimulationState;
		}

		inline void LoadScene(Entity* root)
		{
			mToReload = root;
		}

		inline void ProcessChanges()
		{
			for (int id : mToRemove)
			{
				RemoveEntity(id);
			}

			mToRemove.clear();

			if (mToReload != nullptr)
			{
				mId.Clear();
				mSearch.Clear();
				mUndoRedo->Clear();
				mState->Clear();
				mSelectionPosition = float4();

				delete mSceneGraph;
				mSceneGraph = mToReload;
				unsigned int id = mId.Next();
				mSceneGraph->mSceneID = id;
				mSearch.Add(id, "Root", mSceneGraph);

				mToReload = nullptr;
			}
		}

		/// <summary>
		///	Constructor
		/// </summary>
		Scene(Manager<Mesh>* meshManager, Manager<Texture>* textureManager);

		/// <summary>
		///	Destructor
		/// </summary>
		~Scene();

		State* GetState()
		{
			return mState;
		}

		float4 GetGizmoPosition()
		{
			return mSelectionPosition;
		}

		const mat4& GetGizmoMatrix()
		{
			return mMeanMatrix;
		}

		void SetSubstituteMatrix(const mat4& m)
		{
			mSubstituteMatrix = m;
		}

		void GetRay(Entity* camera, float4& origin, float4& direction, float x_ndc, float y_ndc)
		{
			// Make sure we're passed object with camera component
			if (camera->GameObject().Has<Engine::CameraComponent>())
			{
				// Get camera component
				Engine::Camera* cam = camera->GameObject().Get<Engine::CameraComponent>()->Get();

				// Get & store position
				origin = cam->GetPosition();

				// Calculate clip-space coordinates of ray
				float4 clip = float4(x_ndc, y_ndc, -1.0f, 1.0f);

				// Calculate view-space coordiantes of ray
				float4 view = inverse(cam->GetProjectionMatrix()) * clip;
				view.z = -1.0f;
				view.w = 0.0f;

				// Calculate world-space coordinates of ray and store
				float4 world = inverse(cam->GetViewMatrix()) * view;
				world = normalize(world);

				direction = world;
			}
		}

		bool CanUndo()
		{
			return mUndoRedo->CanUndo();
		}

		void PerformUndo()
		{
			Apply(mUndoRedo->Undo(), true);
		}

		bool CanRedo()
		{
			return mUndoRedo->CanRedo();
		}

		void PerformRedo()
		{
			Apply(mUndoRedo->Redo(), true);
		}

		void Apply(Command* cmd, bool redone = false);

		/// <summary>Apply command on scene, serialize-only - so don't impact objects, just store in undo/redo stack</summary>
		void ApplySerialize(Command* cmd)
		{
			mUndoRedo->Apply(cmd);
		}

		/// <summary>Update the whole scenegraph (loop through all entities in scenegraph)</summary>
		void Update()
		{
			mSimulation->StepSimulation(1.0f / 60.0f);

			std::stack<Entity*> tmp;
			if (mSceneGraph != NULL)
			{
				tmp.push(mSceneGraph);

				while (!tmp.empty())
				{
					Entity* e = tmp.top();
					tmp.pop();

					for (Entity* child : e->Children())
					{
						tmp.push(child);
					}

					e->Update();
				}
			}
		}

		void StartSimulation()
		{
			mSimulationState = PhysicsState::RUNNING;

			mSimulation->Init();

			std::stack<Entity*> tmp;
			if (mSceneGraph != NULL)
			{
				tmp.push(mSceneGraph);

				while (!tmp.empty())
				{
					Entity* e = tmp.top();
					tmp.pop();

					for (Entity* child : e->Children())
					{
						tmp.push(child);
					}

					if (e->GameObject().Has<RigidBodyComponent>() && e->GameObject().Has<CollisionComponent>())
					{
						Shape* shp = e->GameObject().Get<CollisionComponent>()->Get();

						if (shp->GetType() == Shape::Type::TRIANGLE_MESH && e->GameObject().Has<MeshComponent>())
						{
							Mesh* m = e->GameObject().Get<MeshComponent>()->GetMesh();
							TriangleMesh* tm = (TriangleMesh*)shp;
							tm->SetMesh((float*)m->GetVertexBuffer()->Data(), (unsigned int*)m->GetIndexBuffer()->Data(), m->GetIndexBuffer()->NumElements() / 3, m->GetVertexBuffer()->ElementSize());
						}
						
						e->GameObject().Get<RigidBodyComponent>()->Get()->SetTransform(e->GetWorldMatrix());
						mSimulation->AddRigidBody(shp, e->GameObject().Get<RigidBodyComponent>()->Get());
					}
				}
			}

			mSimulation->StartSimulation();
		}

		void PauseSimulation()
		{
			mSimulation->StopSimulation();
		}

		void StopSimulation()
		{
			mSimulationState = PhysicsState::STOPPED;

			mSimulation->StopSimulation();
			mSimulation->Shutdown();
		}
		
		/// <summary>Render the scene using a camera entity</summary>
		void Render(Entity* camera, std::vector<RenderNode>& nodes, mat4* matrices)
		{
			if (camera->GameObject().Has<CameraComponent>())
			{
				CameraComponent* c = camera->GameObject().Get<CameraComponent>();
				mat4 view = c->Get()->GetViewMatrix();
				mat4 proj = c->Get()->GetProjectionMatrix();
				Frustum* f = c->Get()->GetFrustum();
				
				std::stack<Entity*> tmp;
				std::stack<mat4> trans;
				
				if (mSceneGraph != NULL)
				{
					tmp.push(mSceneGraph);
					trans.push(mat4());

					while (!tmp.empty())
					{
						Entity* e = tmp.top();
						tmp.pop();

						mat4 world;
							
						if (e->GameObject().Has<RigidBodyComponent>() && e->GameObject().Has<CollisionComponent>() && mSimulation->IsRunning())
						{
							world = e->GameObject().Get<RigidBodyComponent>()->Get()->GetTransform();
						}
						else
						{
							world = trans.top() * e->Transformation().Get();

							if (mState->GetSelection().find(e->mSceneID) != mState->GetSelection().end())
							{
								world = trans.top() * (mSubstituteMatrix * e->Transformation().Get());
							}
						}

						trans.pop();

						for (Entity* child : e->Children())
						{
							tmp.push(child);
							trans.push(world);
						}

						nodes.push_back(RenderNode(world, &e->GameObject(), e->mSceneID));
						matrices[2 * e->mSceneID] = world;
						matrices[2 * e->mSceneID + 1] = transpose(inverse(world));
					}
				}
			}
		}

		/// <summary>Get entity by given ID</summary>
		/// <param name="id">ID</param>
		Entity* GetEntity(int id)
		{
			return mSearch.Get(id);
		}

		/// <summary>Get entity by given name</summary>
		/// <param name="name">Name</param>
		Entity* GetEntity(const std::string& name)
		{
			return mSearch.Get(name);
		}

		/// <summary>Get entity name</summary>
		/// <param name="id">ID</param>
		const std::string& GetName(int id)
		{
			return mSearch.GetName(id);
		}

		/// <summary>Get entity name</summary>
		/// <param name="entity">Pointer to entity</param>
		const std::string& GetName(Entity* entity)
		{
			return mSearch.GetName(entity->mSceneID);
		}

		/// <summary>Get entity ID</summary>
		/// <param name="name">Entity name</param>
		int GetID(const std::string& name)
		{
			return mSearch.GetID(name);
		}

		/// <summary>Get entity ID</summary>
		/// <param name="entity">Pointer to entity</param>
		int GetID(Entity* entity)
		{
			return mSearch.GetID(entity);
		}

		/// <summary>Get scenegraph root node</summary>
		Entity* GetScenegraph()
		{
			return mSceneGraph;
		}

		/// <summary>Rename entity</summary>
		/// <param name="id">ID</param>
		/// <param name="name">Name</param>
		void Rename(int id, const std::string& name)
		{
			mSearch.SetName(id, name);
		}

		/// <summary>Add New Entity</summary>
		/// <param name="entity">Current entity</param>
		/// <param name="parentId">Parent entity ID</param>
		int AddEntity(Entity* e, int parentId, bool isChild = false);

		/// <summary>Recursive remove of children of given Entity</summary>
		/// <param name="e">Entity whose children are to be removed</param>
		void RemoveChildren(Entity* e);

		/// <summary>Remove entity</summary>
		/// <param name="id">ID of entity to remove</param>
		void RemoveEntity(int id);
		
		ALIGNED_NEW_DELETE("Engine::Game::Scene")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif