#include "Entity.h"
#include "Game/Scene/Scene.h"

using namespace Engine;

std::string Entity::Serialize()
{
	std::stringstream ss;

	ss << "Entity" << std::endl;

	// Store the name
	ss << mName << std::endl;

	// Note. Each entity except Root will always have a parent!
	// Store the parent name
	if (mParent)
	{
		ss << mParent->mName << std::endl;
	}
	else
	{
		ss << std::endl;
	}

	// Store the transform
	ss << mTransform.Serialize();

	// Store components
	for (auto it = mObject.ComponentsItBegin(); it != mObject.ComponentsItEnd(); it++)
	{
		ss << it->second->Serialize();
	}

	ss << "(" << std::endl;

	// Store children (recursively)
	if (mChildren.size() > 0)
	{
		for (auto child : mChildren)
		{
			ss << child->Serialize();
		}
	}

	ss << ")" << std::endl;

	return ss.str();
}

void Entity::Deserialize(Scene* scene, const std::string& s, Entity* parent)
{
	std::vector<std::string> lines = String::Split(s, '\n');

	// Restore name
	mName = lines[1];

	if (scene->GetEntity(mName) == nullptr)
	{
		// Restore parents
		if (parent == nullptr)
		{
			if (lines.size() > 2)
			{
				Entity* parsedParent = scene->GetEntity(lines[2]);

				if (parsedParent != nullptr)
				{
					parsedParent->AddChild(this);
				}
				else
				{
					scene->GetScenegraph()->AddChild(this);
				}
			}
			else
			{
				scene->GetScenegraph()->AddChild(this);
			}
		}
		else
		{
			parent->AddChild(this);
		}
	}

	// Restore transforms
	std::vector<std::string> transformData;
	transformData.push_back(lines[3]);
	transformData.push_back(lines[4]);
	transformData.push_back(lines[5]);
	transformData.push_back(lines[6]);
	std::string transform = String::Join(transformData, '\n');
	mTransform.Deserialize(transform);

	// Restore components
	unsigned int lineID = 7;
	while (true)
	{
		if (lineID == lines.size())
		{
			break;
		}

		if (lines[lineID].size() < 1)
		{
			lineID++;
			continue;
		}

		if (lines[lineID][0] == '(')
		{
			break;
		}

		ComponentId compId = ComponentTypeId::Get(lines[lineID]);
		Component* c = ComponentFactory::CreateComponent(compId);

		std::vector<std::string> componentData;
		componentData.push_back(lines[lineID]);

		unsigned int componentEnd = lineID + 1;
		while (true)
		{
			if (componentEnd == lines.size())
			{
				break;
			}

			if (lines[componentEnd].size() < 1)
			{
				componentEnd++;
				continue;
			}

			if (lines[componentEnd][0] == '(')
			{
				break;
			}

			if (lines[componentEnd].find("Component") != std::string::npos)
			{
				break;
			}

			componentData.push_back(lines[componentEnd]);

			componentEnd++;
		}

		std::string component = String::Join(componentData, '\n');
		c->Deserialize(component);

		mObject.AddComponent(compId, c);

		lineID = componentEnd;
	}

	// If at this point we're not at the end yet, it means there are children for the node, parsing
	// those out is a bit tricky - we need to take whole list of entities within node, each into 
	// separate string buffer

	// Children tree depth search (only level 1 has to be taken into account, all other levels have
	// to be included in string buffer for respective entity
	int level = 0;

	// Should we instantiate entity?
	int instEntity = 0;

	// String buffer
	std::vector<std::string> entityData;

	while (true)
	{
		if (lineID == lines.size())
		{
			break;
		}

		if (lines[lineID].size() < 1)
		{
			lineID++;
			continue;
		}

		if (lines[lineID][0] == '(')
		{
			level++;
			if (level != 1)
			{
				entityData.push_back(lines[lineID]);
			}
			lineID++;
			continue;
		}

		if (level == 0)
		{
			break;
		}

		if (lines[lineID][0] == ')')
		{
			level--;
			if (level != 1)
			{
				entityData.push_back(lines[lineID]);
			}
			lineID++;
			continue;
		}

		if (level == 1)
		{
			if (lines[lineID].find("Entity") != std::string::npos)
			{
				if (instEntity == 1)
				{
					Entity* e = new Entity("_TempChild");
					std::string entityString = String::Join(entityData, '\n');
					e->Deserialize(scene, entityString, this);
					entityData.clear();

					unsigned int id = scene->GetIDGenerator()->Next();
					e->mSceneID = id;
					scene->GetSearchMap()->Add(id, e->GetName(), e);
				}

				// Offset line here by 1, in case name contained 'Entity' so we don't double-hit keyword
				instEntity = 1;
				entityData.push_back(lines[lineID]);
				lineID++;
			}

			// Push line
			entityData.push_back(lines[lineID]);
		}
		else
		{
			// Push line
			entityData.push_back(lines[lineID]);
		}

		lineID++;
	}

	if (instEntity == 1)
	{
		Entity* e = new Entity("_TempChild");
		std::string entityString = String::Join(entityData, '\n');
		e->Deserialize(scene, entityString, this);
		entityData.clear();

		unsigned int id = scene->GetIDGenerator()->Next();
		e->mSceneID = id;
		scene->GetSearchMap()->Add(id, e->GetName(), e);
	}
}