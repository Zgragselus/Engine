#ifndef __TEXTURE_ATLAS__H__
#define __TEXTURE_ATLAS__H__

#include <map>
#include <vector>

namespace Engine
{
	class TextureAtlas
	{
	private:
		enum class NodeType : unsigned int
		{
			LEAF = 0,
			INTERIOR
		};

		struct Node
		{
			NodeType mType;
			unsigned int mEmpty;

			unsigned int mPosition[2];
			unsigned int mSize[2];

			unsigned int mSplitAxis;
			unsigned int mSplit;

			Node* mLeft;
			Node* mRight;
		};

		Node* mRoot;

		std::map<int, Node*> mTextures;
		unsigned int mCounterID;
		std::vector<int> mFreeIDs;

		void RecursiveRelease(Node* node);

		Node* RecursiveRequest(Node* node, unsigned int width, unsigned int height);

	public:
		TextureAtlas(unsigned int width, unsigned int height);

		~TextureAtlas();

		int RequestTexture(unsigned int width, unsigned int height);

		void ReleaseTexture(int id);

		void GetTile(int id, unsigned int& positionX, unsigned int& positionY, unsigned int& width, unsigned int& height);

		void Clear(unsigned int width, unsigned int height);

		inline int GetWidth()
		{
			return mRoot->mSize[0];
		}

		inline int GetHeight()
		{
			return mRoot->mSize[1];
		}
	};
}

#endif
