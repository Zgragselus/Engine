///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Manager.h
//
// Following file contains templated manager class
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __MANAGER__H__
#define __MANAGER__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/Memory/Memory.h"
#include <map>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	/// Generic Manager class for managing unique resources
	/// </summary>
	template <typename T>
	class Manager
	{
	private:
		/// <summary>
		/// Node class, representing data node inside manager class
		/// </summary>
		struct Node
		{
			int mReferences;		// How many times has resource been referenced
			std::string mName;		// Resource name
			T* mItem;				// Pointer to resource data

			/// <summary>
			/// Constructor from resource name
			/// </summary>
			/// <param name="name">Name of the new resource</param>
			Node(const std::string& name)
			{
				mName = name;
				mReferences = 1;
			}

			/// <summary>
			/// Default destructor
			/// </summary>
			~Node()
			{
				if (mItem != NULL)
				{
					delete mItem;
				}
			}

			template <typename S>
			void Init()
			{
				mItem = new S(mName);
			}
		};
		
		std::map<std::string, Node*> mData;		// Data container for manager class

	public:
		/// <summary>
		/// Destructor, removes all data
		/// </summary>
		~Manager()
		{
			RemoveAll();
		}

		/// <summary>
		/// Creates new resource with given name, returns pointer to it
		/// </summary>
		/// <param name="name">Name of the resource</param>
		template <typename S>
		T* Load(const std::string& name)
		{
			auto it = mData.find(name);
			if (it == mData.end())
			{
				// Load item
				Node* item = new Node(name);
				item->Init<S>();
				mData.insert(std::pair<std::string, Node*>(name, item));
				return item->mItem;
			}
			else
			{
				it->second->mReferences++;
				return it->second->mItem;
			}
		}

		/// <summary>
		/// Gets pointer to number of references on given name.
		/// </summary>
		/// <param name="name">Name of the resource</param>
		int* GetReferences(const std::string& name)
		{
			auto it = mData.find(name);
			if (it == mData.end())
			{
				return NULL;
			}
			else
			{
				return &(it->second->mReferences);
			}
		}

		/// <summary>
		/// Gets pointer to number of references for given resource.
		/// </summary>
		/// <param name="item">Pointer to resource</param>
		int* GetReferences(T* item)
		{
			for (auto it = mData.begin(); it != mData.end(); it++)
			{
				if (it->second->mItem == item)
				{
					return &(it->second->mReferences);
				}
			}

			return NULL;
		}

		/// <summary>
		/// Removes resource with given name.
		/// </summary>
		/// <param name="name">Name of the resource</param>
		void Remove(const std::string& name)
		{
			auto it = mData.find(name);

			if (it != mData.end())
			{
				it->second->mReferences--;

				if (it->second->mReferences <= 0)
				{
					delete it->second;
					mData.erase(it);
				}
			}
		}

		/// <summary>
		/// Removes resource.
		/// </summary>
		/// <param name="item">Resource to remove</param>
		void Remove(T* item)
		{
			for (auto it = mData.begin(); it != mData.end(); )
			{
				if (it->second->mItem == item)
				{
					it->second->mReferences--;

					if (it->second->mReferences <= 0)
					{
						delete it->second;
						auto tmp = it;
						it++;
						mData.erase(tmp);
						continue;
					}
				}

				it++;
			}
		}

		/// <summary>
		/// Removes all resources.
		/// </summary>
		void RemoveAll()
		{
			auto it = mData.begin();
			typename std::map<std::string, Node*>::iterator prevIt;

			while (it != mData.end())
			{
				prevIt = it;
				it++;

				delete prevIt->second;
				mData.erase(prevIt);
			}
		}
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif