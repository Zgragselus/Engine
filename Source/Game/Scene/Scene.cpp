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

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section
#include "Scene.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Declaration section
using namespace Engine;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Definition section

/// <summary>
///	Constructor
/// </summary>
Scene::Scene(Manager<Mesh>* meshManager, Manager<Texture>* textureManager)
{
	mSimulation = new Simulation();
	mSimulationState = PhysicsState::STOPPED;

	mSceneGraph = new Entity("Root");
	unsigned int id = mId.Next();
	mSceneGraph->mSceneID = id;
	mSearch.Add(id, "Root", mSceneGraph);

	mUndoRedo = new UndoRedo(64);
	mState = new State();
	mSelectionPosition = float4();

	mToReload = nullptr;

	ComponentFactory::Initialize(meshManager, textureManager);
}

/// <summary>
///	Destructor
/// </summary>
Scene::~Scene()
{
	delete mSimulation;
	delete mSceneGraph;
	delete mState;
	delete mUndoRedo;
}

/// <summary>Add New Entity</summary>
/// <param name="entity">Current entity</param>
/// <param name="parentId">Parent entity ID</param>
/// <param name="name">Entity name</param>
int Scene::AddEntity(Entity* e, int parentId, bool isChild)
{
	if (!isChild)
	{
		Entity* parent = GetEntity(parentId);

		if (parent == NULL)
		{
			mSceneGraph->AddChild(e);
		}
		else
		{
			parent->AddChild(e);
		}
	}

	unsigned int id = mId.Next();
	e->mSceneID = id;
	mSearch.Add(id, e->GetName(), e);

	int tmp = 0;
	for (Entity* e : e->Children())
	{
		AddEntity(e, id, true);
		tmp++;
	}

	return id;
}

/// <summary>Recursive remove of children of given Entity</summary>
/// <param name="e">Entity whose children are to be removed</param>
void Scene::RemoveChildren(Entity* e)
{
	// For each children of given entity
	for (Entity* child : e->Children())
	{
		// Recursively remove all children
		RemoveChildren(child);

		// Get ID for current child
		unsigned int id = mSearch.GetID(child);
		if (id == 0)
		{
			return;
		}

		// Remove index from search structure
		mSearch.Remove(id);

		// Clear entity memory
		delete child;

		// Push free id to re-use
		mId.Remove(id);
	}
}

/// <summary>Remove entity</summary>
/// <param name="id">ID of entity to remove</param>
void Scene::RemoveEntity(int id)
{
	// Get entity for given ID
	Entity* e = mSearch.Get(id);

	// Remove all children for this entity
	RemoveChildren(e);

	// Remove index from search structure
	mSearch.Remove(id);

	// Clear entity memory
	delete e;

	// Push free id to re-use
	mId.Remove(id);
}

void Scene::Apply(Command* cmd, bool redone)
{
	int tmp;
	float x, y, z, w;
	float cx, cy, cz;
	mat4 transform;
	Entity* entity;

	if (cmd == NULL)
	{
		return;
	}

	Command::Type type = cmd->GetType();
	switch (type)
	{
	case Command::Select:
		for (tmp = 0; tmp < cmd->GetArgsCount(); tmp++)
		{
			mState->GetSelection().insert(*cmd->GetArg<int>(tmp));
		}
		RecalculateGizmoPosition();
		break;

	case Command::Deselect:
		for (tmp = 0; tmp < cmd->GetArgsCount(); tmp++)
		{
			mState->GetSelection().erase(*cmd->GetArg<int>(tmp));
		}
		RecalculateGizmoPosition();
		break;

	case Command::Translate:
		x = *cmd->GetArg<float>(0);
		y = *cmd->GetArg<float>(1);
		z = *cmd->GetArg<float>(2);
		transform = mat4(float4(x, y, z, 0.0f));
		for (auto it = mState->GetSelection().begin(); it != mState->GetSelection().end(); it++)
		{
			Entity* ent = mSearch.Get(*it);
			if (ent == nullptr)
			{
				continue;
			}

			ent->Transformation().Apply(transform);
		}
		RecalculateGizmoPosition();
		break;

	case Command::Rotate:
		x = *cmd->GetArg<float>(0);
		y = *cmd->GetArg<float>(1);
		z = *cmd->GetArg<float>(2);
		w = *cmd->GetArg<float>(3);
		cx = *cmd->GetArg<float>(4);
		cy = *cmd->GetArg<float>(5);
		cz = *cmd->GetArg<float>(6);
		transform = mat4(float4(cx, cy, cz, 1.0f)) * mat4(quat(x, y, z, w)) * mat4(float4(-cx, -cy, -cz, 1.0f));
		for (auto it = mState->GetSelection().begin(); it != mState->GetSelection().end(); it++)
		{
			Entity* ent = mSearch.Get(*it);
			if (ent == nullptr)
			{
				continue;
			}

			ent->Transformation().Apply(transform);
		}
		RecalculateGizmoPosition();
		break;

	case Command::Scale:
		x = *cmd->GetArg<float>(0);
		y = *cmd->GetArg<float>(1);
		z = *cmd->GetArg<float>(2);
		cx = *cmd->GetArg<float>(3);
		cy = *cmd->GetArg<float>(4);
		cz = *cmd->GetArg<float>(5);
		transform = mat4(float4(cx, cy, cz, 1.0f)) *
			mat4(*cmd->GetArg<float>(6), *cmd->GetArg<float>(9), *cmd->GetArg<float>(12), 0.0f,
				*cmd->GetArg<float>(7), *cmd->GetArg<float>(10), *cmd->GetArg<float>(13), 0.0f,
				*cmd->GetArg<float>(8), *cmd->GetArg<float>(11), *cmd->GetArg<float>(14), 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f) *
			mat4(x, 0.0f, 0.0f, 0.0f, 0.0f, y, 0.0f, 0.0f, 0.0f, 0.0f, z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f) *
			mat4(*cmd->GetArg<float>(6), *cmd->GetArg<float>(7), *cmd->GetArg<float>(8), 0.0f,
				*cmd->GetArg<float>(9), *cmd->GetArg<float>(10), *cmd->GetArg<float>(11), 0.0f,
				*cmd->GetArg<float>(12), *cmd->GetArg<float>(13), *cmd->GetArg<float>(14), 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f) *
			mat4(float4(-cx, -cy, -cz, 1.0f));
		for (auto it = mState->GetSelection().begin(); it != mState->GetSelection().end(); it++)
		{
			Entity* ent = mSearch.Get(*it);
			if (ent == nullptr)
			{
				continue;
			}

			ent->Transformation().Apply(transform);
		}
		RecalculateGizmoPosition();
		break;

	case Command::Create:
		mState->GetSelection().clear();

		for (unsigned int i = 0; i < cmd->GetArgsCount(); i++)
		{
			//printf("ENT %s\n", cmd->GetArg<std::string>(i)->c_str());

			

			entity = new Entity("_Temp");
			entity->Deserialize(this, *cmd->GetArg<std::string>(i));

			int id = AddEntity(entity, mSearch.GetID(entity->GetParent()));

			mState->GetSelection().insert(id);
		}
		break;

	case Command::Delete:
		for (auto it = mState->GetSelection().begin(); it != mState->GetSelection().end(); it++)
		{
			Entity* ent = mSearch.Get(*it);
			if (ent == nullptr)
			{
				continue;
			}

			//RemoveEntity(*it);
			mToRemove.push_back(*it);
		}

		mState->GetSelection().clear();
		break;

	case Command::Rename:
		for (auto it = mState->GetSelection().begin(); it != mState->GetSelection().end(); it++)
		{
			mSearch.SetName(*it, *cmd->GetArg<std::string>(1));
		}
		break;

	case Command::Reassign:
		{
			std::map<int, int> before;
			std::map<int, int> after;

			int tmp = 0;
			int entities = 0;
			while (*cmd->GetArg<int>(tmp) != -1)
			{
				before.insert(std::pair<int, int>(*cmd->GetArg<int>(tmp), *cmd->GetArg<int>(tmp + 1)));
				tmp += 2;
				entities++;
			}
			tmp++;
			for (int i = 0; i < entities; i++)
			{
				after.insert(std::pair<int, int>(*cmd->GetArg<int>(tmp), *cmd->GetArg<int>(tmp + 1)));
				tmp += 2;
			}

			for (auto it = before.begin(); it != before.end(); it++)
			{
				mat4 childMat = GetEntity(it->first)->GetWorldMatrix();
				mat4 parentMat = GetEntity(after[it->first])->GetWorldMatrix();
				mat4 transformation = inverse(parentMat) * childMat;

				GetEntity(after[it->first])->AddChild(GetEntity(it->first));

				GetEntity(it->first)->Transformation().Extract(transformation);
				GetEntity(it->first)->Transformation().Invalidate();
				GetEntity(it->first)->Transformation().Update();
			}
		}
		break;

	case Command::Serialize:
		{
			std::string prev = *cmd->GetArg<std::string>(0);

			for (auto it = mState->GetSelection().begin(); it != mState->GetSelection().end(); it++)
			{
				Entity* ent = mSearch.Get(*it);
				if (ent == nullptr)
				{
					continue;
				}

				std::stringstream ss(prev);
				std::string component;
				ss >> component;

				ComponentId componentId = ComponentTypeId::Get(component);

				if (ent->GameObject().HasComponent(componentId))
				{
					ent->GameObject().GetComponent(componentId)->Deserialize(prev);
				}
			}
		}
		break;

	case Command::AddComponent:
		{
			ComponentId compId = *cmd->GetArg<int>(0);
			Component* c = ComponentFactory::CreateComponent(compId);
			std::string tmp = *cmd->GetArg<std::string>(1);
			if (tmp.length() > 0)
			{
				c->Deserialize(tmp);
			}

			for (auto it = mState->GetSelection().begin(); it != mState->GetSelection().end(); it++)
			{
				Entity* ent = mSearch.Get(*it);
				if (ent == nullptr)
				{
					continue;
				}

				if (!ent->GameObject().HasComponent(compId))
				{
					ent->GameObject().AddComponent(compId, c);
				}
			}

			*cmd->GetArg<std::string>(1) = tmp;
		}
		break;

	case Command::RemoveComponent:
		{
			ComponentId compId = *cmd->GetArg<int>(0);

			std::string componentData;

			for (auto it = mState->GetSelection().begin(); it != mState->GetSelection().end(); it++)
			{
				Entity* ent = mSearch.Get(*it);
				if (ent == nullptr)
				{
					continue;
				}

				if (ent->GameObject().HasComponent(compId))
				{
					Component* c = ent->GameObject().GetComponent(compId);
					componentData = c->Serialize();
					ent->GameObject().RemoveComponent(compId);
				}
			}

			*cmd->GetArg<std::string>(1) = componentData;
		}
		break;
	}

	if (!redone)
	{
		mUndoRedo->Apply(cmd);
	}
}