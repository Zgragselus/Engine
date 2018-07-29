///////////////////////////////////////////////////////////////////////////////////////////////////
//
// SceneMap.h
//
// Search structure to map between ID, name and data.
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __SCENE_MAP__H__
#define __SCENE_MAP__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include <string>
#include <map>
#include "Core/Memory/Memory.h"
#include "../Entity/Entity.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	///	Templated class for storing and searching in ID-Name-Record triples inside the game scene
	/// it maps between integer ID value, string (name) and record pointer.
	/// </summary>
	template <typename T>
	class SceneMap
	{
	private:
		/// <summary>ID-data association</summary>
		std::map<int, T*> mIdMap;

		/// <summary>Name-data association</summary>
		std::map<std::string, T*> mNameMap;

		/// <summary>ID-Name association</summary>
		std::map<int, std::string> mIdNameMap;

		/// <summary>Name-ID association</summary>
		std::map<std::string, int> mNameIdMap;

		/// <summary>Unnamed node</summary>
		std::string mUnnamed;

		/// <summary>
		/// Remove record from search structure
		/// </summary>
		/// <param name="idMap_it">Iterator for id-data pair</param>
		/// <param name="nameMap_it">Iterator for name-data pair</param>
		/// <param name="idName_it">Iterator for id-name pair</param>
		/// <param name="nameId_it">Iterator for name-id pair</param>
		inline void Remove(typename std::map<int, T*>::iterator idMap_it,
			typename std::map<std::string, T*>::iterator nameMap_it,
			typename std::map<int, std::string>::iterator idName_it,
			typename std::map<std::string, int>::iterator nameId_it)
		{
			mIdMap.erase(idMap_it);
			mNameMap.erase(nameMap_it);
			mIdNameMap.erase(idName_it);
			mNameIdMap.erase(nameId_it);
		}

	public:
		/// <summary>
		/// Default constructor
		/// </summary>
		SceneMap()
		{
			mUnnamed = "Unnamed";
		}

		/// <summary>
		/// Add new record. 
		/// </summary>
		/// <param name="id">Record ID</param>
		/// <param name="name">Record name</param>
		/// <param name="record">Pointer to data</param>
		inline void Add(int id, const std::string& name, T* record)
		{
			mIdMap.insert(std::pair<int, T*>(id, record));
			mNameMap.insert(std::pair<std::string, T*>(name, record));
			mIdNameMap.insert(std::pair<int, std::string>(id, name));
			mNameIdMap.insert(std::pair<std::string, int>(name, id));
		}

		/// <summary>
		/// Remove record based on id. Method has to find iterators to all 4 maps
		/// </summary>
		/// <param name="id">ID of the record to remove</param>
		inline void Remove(int id)
		{
			auto id_it = mIdMap.find(id);
			if (id_it == mIdMap.end())
			{
				return;
			}

			auto id_name_it = mIdNameMap.find(id);
			if (id_name_it == mIdNameMap.end())
			{
				return;
			}

			auto name_it = mNameMap.find(id_name_it->second);
			if (name_it == mNameMap.end())
			{
				return;
			}

			auto name_id_it = mNameIdMap.find(id_name_it->second);
			if (name_id_it == mNameIdMap.end())
			{
				return;
			}

			Remove(id_it, name_it, id_name_it, name_id_it);
		}

		/// <summary>
		/// Remove record based on name. Method has to find iterators to all 4 maps
		/// </summary>
		/// <param name="name">Name of the record to remove</param>
		inline void Remove(const std::string& name)
		{
			auto name_it = mNameMap.find(name);
			if (name_it == mNameMap.end())
			{
				return;
			}

			auto name_id_it = mNameIdMap.find(name);
			if (name_id_it == mNameIdMap.end())
			{
				return;
			}

			auto id_it = mIdMap.find(name_id_it->second);
			if (id_it == mIdMap.end())
			{
				return;
			}

			auto id_name_it = mIdNameMap.find(name_id_it->second);
			if (id_name_it == mIdNameMap.end())
			{
				return;
			}

			Remove(id_it, name_it, id_name_it, name_id_it);
		}

		/// <summary>
		/// Get record based on ID
		/// </summary>
		/// <param name="id">Specifies ID of the record</param>
		inline T* Get(int id)
		{
			auto id_it = mIdMap.find(id);
			if (id_it == mIdMap.end())
			{
				return NULL;
			}

			return id_it->second;
		}

		/// <summary>
		/// Get record based on name
		/// </summary>
		/// <param name="name">Specifies name of the record</param>
		inline T* Get(const std::string& name)
		{
			auto name_it = mNameMap.find(name);
			if (name_it == mNameMap.end())
			{
				return NULL;
			}

			return name_it->second;
		}

		/// <summary>
		/// Get ID by entity pointer
		/// </summary>
		/// <param name="e">Pointer to entity</param>
		inline int GetID(Entity* e)
		{
			unsigned int id = 0;
			for (auto it = mIdMap.begin();
				it != mIdMap.end();
				it++)
			{
				if (it->second == e)
				{
					id = it->first;
					break;
				}
			}
			return id;
		}

		/// <summary>
		/// Get ID by entity name
		/// </summary>
		/// <param name="name">Entity name</param>
		inline int GetID(const std::string& name)
		{
			auto id_it = mNameIdMap.find(name);
			if (id_it == mNameIdMap.end())
			{
				return -1;
			}
			return id_it->second;
		}

		/// <summary>
		/// Get name by ID
		/// </summary>
		inline const std::string& GetName(int id)
		{
			auto it = mIdNameMap.find(id);
			if (it != mIdNameMap.end())
			{
				return it->second;
			}

			return mUnnamed;
		}

		/// <summary>
		/// Set name for ID
		/// </summary>
		inline void SetName(int id, const std::string& name)
		{
			T* entity = Get(id);

			if (entity != nullptr)
			{
				entity->SetName(name);

				mIdNameMap[id] = name;

				for (auto it = mNameIdMap.begin(); it != mNameIdMap.end(); it++)
				{
					if (it->second == id)
					{
						mNameIdMap.erase(it);
						mNameIdMap.insert(std::pair<std::string, int>(name, id));
						break;
					}
				}

				for (auto it = mNameMap.begin(); it != mNameMap.end(); it++)
				{
					if (it->second == entity)
					{
						mNameMap.erase(it);
						mNameMap.insert(std::pair<std::string, T*>(name, entity));
						break;
					}
				}
			}			
		}

		ALIGNED_NEW_DELETE("Engine::SceneMap")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif