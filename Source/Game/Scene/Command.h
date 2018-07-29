#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "Game/Entity/Entity.h"
#include "Core/Math/Numeric/Float4.h"
#include "Core/Math/Numeric/Mat4.h"
#include "Core/Util/String.h"

namespace Engine
{
	class Command
	{
	public:
		enum Type
		{
			Select,
			Deselect,
			Translate,
			Rotate,
			Scale,
			Create,
			Delete,
			Rename,
			Reassign,
			Serialize,
			AddComponent,
			RemoveComponent
		};

	private:
		enum tValue
		{
			VALUE_INT,
			VALUE_FLOAT,
			VALUE_STRING,
			VALUE_FLOAT4,
			VALUE_MATRIX,
			VALUE_ENTITY
		};

		struct sValue
		{
			tValue type;
			void* data;

			sValue(int value)
			{
				type = VALUE_INT;
				data = new int(value);
			}

			sValue(float value)
			{
				type = VALUE_FLOAT;
				data = new float(value);
			}

			sValue(const std::string& value)
			{
				type = VALUE_STRING;
				data = new std::string(value);
			}

			sValue(const float4& value)
			{
				type = VALUE_FLOAT4;
				data = new float4(value);
			}

			sValue(const mat4& value)
			{
				type = VALUE_MATRIX;
				data = new mat4(value);
			}

			sValue(Entity* value)
			{
				type = VALUE_ENTITY;
				data = value;
			}

			~sValue()
			{	
				switch (type)
				{
				case VALUE_INT:
					delete (int*)data;
					break;

				case VALUE_FLOAT:
					delete (float*)data;
					break;

				case VALUE_STRING:
					delete (std::string*)data;
					break;

				case VALUE_FLOAT4:
					delete (float4*)data;
					break;

				case VALUE_MATRIX:
					delete (mat4*)data;
					break;

				case VALUE_ENTITY:
					data = NULL;
					break;
				}
			}
		};

		Type mType;
		std::vector<sValue*> mArgs;

	public:
		Command(Type type)
		{
			mType = type;
		}

		Command(const Command& c)
		{
			mType = c.mType;
			for (sValue* v : c.mArgs)
			{
				switch (v->type)
				{
				case VALUE_INT:
					mArgs.push_back(new sValue(*(int*)v->data));
					break;

				case VALUE_FLOAT:
					mArgs.push_back(new sValue(*(float*)v->data));
					break;

				case VALUE_STRING:
					mArgs.push_back(new sValue(*(std::string*)v->data));
					break;

				case VALUE_FLOAT4:
					mArgs.push_back(new sValue(*(float4*)v->data));
					break;

				case VALUE_MATRIX:
					mArgs.push_back(new sValue(*(mat4*)v->data));
					break;

				case VALUE_ENTITY:
					mArgs.push_back(new sValue((Entity*)v->data));
					break;
				}
			}
		}

		~Command()
		{
			for (sValue* v : mArgs)
			{
				delete v;
			}
			mArgs.clear();
		}

		void Invert()
		{
			switch (mType)
			{
			case Select:
				mType = Deselect;
				break;

			case Deselect:
				mType = Select;
				break;

			case Translate:
				((float*)mArgs[0]->data)[0] = -*GetArg<float>(0);
				((float*)mArgs[1]->data)[0] = -*GetArg<float>(1);
				((float*)mArgs[2]->data)[0] = -*GetArg<float>(2);
				break;

			case Rotate:
				((float*)mArgs[0]->data)[0] = -*GetArg<float>(0);
				((float*)mArgs[1]->data)[0] = -*GetArg<float>(1);
				((float*)mArgs[2]->data)[0] = -*GetArg<float>(2);
				((float*)mArgs[3]->data)[0] = *GetArg<float>(3);
				((float*)mArgs[4]->data)[0] = *GetArg<float>(4);
				((float*)mArgs[5]->data)[0] = *GetArg<float>(5);
				((float*)mArgs[6]->data)[0] = *GetArg<float>(6);
				break;

			case Scale:
				((float*)mArgs[0]->data)[0] = 1.0f / *GetArg<float>(0);
				((float*)mArgs[1]->data)[0] = 1.0f / *GetArg<float>(1);
				((float*)mArgs[2]->data)[0] = 1.0f / *GetArg<float>(2);
				break;

			case Create:
				mType = Delete;
				break;

			case Delete:
				mType = Create;
				break;

			case Rename:
				{
					std::string tmp = *GetArg<std::string>(0);
					*((std::string*)mArgs[0]->data) = *GetArg<std::string>(1);
					*((std::string*)mArgs[1]->data) = tmp;
				}
				break;

			case Reassign:
				{
					// Swaps command from:
					//     [0, 5, 2, 5, 3, 4, -1, 0, 7, 2, 7, 3, 7]
					// to:
					//     [0, 7, 2, 7, 3, 7, -1, 0, 5, 2, 5, 3, 4]
				
					// Store entity + parent ID before reassignment
					std::vector<int> prev;

					int tmp = 0;
					int entities = 0;
					while (*GetArg<int>(tmp) != -1)
					{
						prev.push_back(*GetArg<int>(tmp));
						prev.push_back(*GetArg<int>(tmp + 1));
						tmp += 2;
						entities++;
					}

					// Push tmp to begin at after pair set (not on the separator)
					tmp++;

					// Copy after state instead of before, and set befor instead of after (from 'prev' array)
					for (int i = 0; i < entities; i++)
					{
						*((int*)mArgs[i * 2 + 0]->data) = *GetArg<int>(tmp);
						*((int*)mArgs[i * 2 + 1]->data) = *GetArg<int>(tmp + 1);
						*((int*)mArgs[tmp]->data) = prev[i * 2 + 0];
						*((int*)mArgs[tmp + 1]->data) = prev[i * 2 + 1];
						tmp += 2;
					}
				}
				break;

			case Serialize:
				{
					std::string prev = *GetArg<std::string>(0);
					std::string next = *GetArg<std::string>(1);
					*((std::string*)mArgs[0]->data) = next;
					*((std::string*)mArgs[1]->data) = prev;
				}
				break;

			case AddComponent:
				mType = RemoveComponent;
				((int*)mArgs[0]->data)[0] = *GetArg<int>(0);
				*((std::string*)mArgs[1]->data) = *GetArg<std::string>(1);
				break;

			case RemoveComponent:
				mType = AddComponent;
				((int*)mArgs[0]->data)[0] = *GetArg<int>(0);
				*((std::string*)mArgs[1]->data) = *GetArg<std::string>(1);
				break;
			}
		}

		template <typename T>
		void AddArg(T value)
		{
			mArgs.push_back(new sValue(value));
		}

		template <typename T>
		T* GetArg(int idx) const
		{
			return (T*)((mArgs[idx])->data);
		}

		const Type& GetType() const
		{
			return mType;
		}

		size_t GetArgsCount() const
		{
			return mArgs.size();
		}

		ALIGNED_NEW_DELETE("Command")
	};
}

#endif