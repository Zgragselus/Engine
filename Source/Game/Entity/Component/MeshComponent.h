#ifndef __MESH_COMPONENT__
#define __MESH_COMPONENT__

#include "../Component.h"
#include "Graphics/Common/Mesh/Mesh.h"
#include "Graphics/Common/Manager/Manager.h"

namespace Engine
{
	class __declspec(align(16)) MeshComponent : public Component
	{
	private:
		static std::string mComponentName;
		Manager<Mesh>* mManager;
		Manager<Mesh>::Node* mMesh;

	public:
		MeshComponent(Mesh* mesh, Manager<Mesh>* manager) : Component()
		{
			mManager = manager;

			if (mesh)
			{
				mMesh = mManager->GetNode(mesh);
			}
		}

		MeshComponent(const MeshComponent& m)
		{
			mMesh = m.mMesh;
			mManager = m.mManager;
		}

		virtual ~MeshComponent()
		{
			if (mMesh)
			{
				mManager->Remove(mMesh->mItem);
				mMesh = nullptr;
			}
		}

		inline Mesh* GetMesh()
		{
			return mMesh->mItem;
		}

		static std::string& GetName()
		{
			return mComponentName;
		}

		virtual bool Editor(std::string& prev, std::string& next);

		virtual std::string Serialize()
		{
			std::stringstream ss;

			ss << "MeshComponent" << std::endl;
			ss << mMesh->mName << std::endl;

			return ss.str();
		}

		virtual void Deserialize(const std::string& s);

		ALIGNED_NEW_DELETE("Engine::MeshComponent")
	};
}

#endif