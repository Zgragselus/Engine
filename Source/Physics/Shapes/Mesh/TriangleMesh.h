#ifndef __TRIANGLE_MESH__H__
#define __TRIANGLE_MESH__H__

#include "Physics/Shapes/Shape.h"
#include "Graphics/Common/Mesh/Mesh.h"

namespace Engine
{
	class Simulation;

	class __declspec(align(16)) TriangleMesh : public Shape
	{
	protected:
		float* mVertices;
		unsigned int* mIndices;
		size_t mNumTriangles;
		size_t mVertexSize;

	public:
		inline TriangleMesh() : Shape()
		{
			mType = Type::TRIANGLE_MESH;
		}

		void SetMesh(float* vertices, unsigned int* indices, size_t numTriangles, size_t vertexSize)
		{
			mVertices = vertices;
			mIndices = indices;
			mNumTriangles = numTriangles;
			mVertexSize = vertexSize;
		}

		virtual std::string Serialize()
		{
			std::stringstream ss;

			ss << mNumTriangles << " " << mVertexSize << std::endl;

			return (Shape::Serialize() + ss.str());
		}

		virtual void Deserialize(const std::string& s)
		{
			std::vector<std::string> lines = String::Split(s, '\n');

			Shape::Deserialize(lines[0]);
		}

		friend class CollisionComponent;

		ALIGNED_NEW_DELETE("Engine::TriangleMesh")

		friend class Simulation;
	};
}

#endif