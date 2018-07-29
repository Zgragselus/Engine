#include "TextureAtlas.h"

using namespace Engine;

void TextureAtlas::RecursiveRelease(TextureAtlas::Node* node)
{
	if (node->mLeft)
	{
		RecursiveRelease(node->mLeft);
	}

	if (node->mRight)
	{
		RecursiveRelease(node->mRight);
	}

	delete node;
}

TextureAtlas::Node* TextureAtlas::RecursiveRequest(TextureAtlas::Node* node, unsigned int width, unsigned int height)
{
	if (node->mEmpty == 0)
	{
		return nullptr;
	}

	if (width > node->mSize[0] || height > node->mSize[1])
	{
		return nullptr;
	}

	if (node->mType == NodeType::INTERIOR)
	{
		switch (node->mSplitAxis)
		{
		case 0:
			if (node->mSplit - node->mPosition[0] > node->mSize[0] - (node->mSplit - node->mPosition[0]))
			{
				Node* tmp = RecursiveRequest(node->mRight, width, height);
				if (tmp == nullptr)
				{
					tmp = RecursiveRequest(node->mLeft, width, height);
				}
				return tmp;
			}
			else
			{
				Node* tmp = RecursiveRequest(node->mLeft, width, height);
				if (tmp == nullptr)
				{
					tmp = RecursiveRequest(node->mRight, width, height);
				}
				return tmp;
			}
			break;

		case 1:
			if (node->mSplit - node->mPosition[1] > node->mSize[1] - (node->mSplit - node->mPosition[1]))
			{
				Node* tmp = RecursiveRequest(node->mRight, width, height);
				if (tmp == nullptr)
				{
					tmp = RecursiveRequest(node->mLeft, width, height);
				}
				return tmp;
			}
			else
			{
				Node* tmp = RecursiveRequest(node->mLeft, width, height);
				if (tmp == nullptr)
				{
					tmp = RecursiveRequest(node->mRight, width, height);
				}
				return tmp;
			}
			break;

		default:
			return nullptr;
			break;
		}
	}
	else
	{
		if (width == node->mSize[0] && height == node->mSize[1])
		{
			return node;
		}
		else
		{
			if (node->mSize[0] > node->mSize[1])
			{
				node->mType = NodeType::INTERIOR;
				node->mSplitAxis = 0;
				node->mSplit = node->mPosition[0] + width;

				node->mLeft = new Node();
				node->mLeft->mType = NodeType::LEAF;
				node->mLeft->mPosition[0] = node->mPosition[0];
				node->mLeft->mPosition[1] = node->mPosition[1];
				node->mLeft->mSize[0] = width;
				node->mLeft->mSize[1] = node->mSize[1];
				node->mLeft->mLeft = nullptr;
				node->mLeft->mRight = nullptr;
				node->mLeft->mEmpty = 1;

				node->mRight = new Node();
				node->mRight->mType = NodeType::LEAF;
				node->mRight->mPosition[0] = node->mPosition[0] + width;
				node->mRight->mPosition[1] = node->mPosition[1];
				node->mRight->mSize[0] = node->mSize[0] - width;
				node->mRight->mSize[1] = node->mSize[1];
				node->mRight->mLeft = nullptr;
				node->mRight->mRight = nullptr;
				node->mRight->mEmpty = 1;

				return RecursiveRequest(node->mLeft, width, height);
			}
			else
			{
				node->mType = NodeType::INTERIOR;
				node->mSplitAxis = 1;
				node->mSplit = node->mPosition[1] + height;

				node->mLeft = new Node();
				node->mLeft->mType = NodeType::LEAF;
				node->mLeft->mPosition[0] = node->mPosition[0];
				node->mLeft->mPosition[1] = node->mPosition[1];
				node->mLeft->mSize[0] = node->mSize[0];
				node->mLeft->mSize[1] = height;
				node->mLeft->mLeft = nullptr;
				node->mLeft->mRight = nullptr;
				node->mLeft->mEmpty = 1;

				node->mRight = new Node();
				node->mRight->mType = NodeType::LEAF;
				node->mRight->mPosition[0] = node->mPosition[0];
				node->mRight->mPosition[1] = node->mPosition[1] + height;
				node->mRight->mSize[0] = node->mSize[0];
				node->mRight->mSize[1] = node->mSize[1] - height;
				node->mRight->mLeft = nullptr;
				node->mRight->mRight = nullptr;
				node->mRight->mEmpty = 1;

				return RecursiveRequest(node->mLeft, width, height);
			}
		}
	}
}

TextureAtlas::TextureAtlas(unsigned int width, unsigned int height)
{
	mRoot = new Node();
	mRoot->mType = NodeType::LEAF;
	mRoot->mPosition[0] = 0;
	mRoot->mPosition[1] = 0;
	mRoot->mSize[0] = width;
	mRoot->mSize[1] = height;
	mRoot->mLeft = nullptr;
	mRoot->mRight = nullptr;
	mRoot->mEmpty = 1;

	mCounterID = 0;
}

TextureAtlas::~TextureAtlas()
{
	RecursiveRelease(mRoot);
}

int TextureAtlas::RequestTexture(unsigned int width, unsigned int height)
{
	int id = -1;
	Node* node = RecursiveRequest(mRoot, width, height);
	if (node == nullptr)
	{
		return id;
	}

	node->mEmpty = 0;

	if (mFreeIDs.size() > 0)
	{
		id = mFreeIDs[mFreeIDs.size() - 1];
		mFreeIDs.pop_back();
	}
	else
	{
		id = mCounterID++;
	}

	mTextures[id] = node;

	return id;
}

void TextureAtlas::ReleaseTexture(int id)
{
	Node* node = mTextures[id];
	node->mEmpty = 1;
	mFreeIDs.push_back(id);
	mTextures.erase(id);
}

void TextureAtlas::GetTile(int id, unsigned int& positionX, unsigned int& positionY, unsigned int& width, unsigned int& height)
{
	Node* node = mTextures[id];

	positionX = node->mPosition[0];
	positionY = node->mPosition[1];
	width = node->mSize[0];
	height = node->mSize[1];
}

void TextureAtlas::Clear(unsigned int width, unsigned int height)
{
	RecursiveRelease(mRoot);

	mRoot = new Node();
	mRoot->mType = NodeType::LEAF;
	mRoot->mPosition[0] = 0;
	mRoot->mPosition[1] = 0;
	mRoot->mSize[0] = width;
	mRoot->mSize[1] = height;
	mRoot->mLeft = nullptr;
	mRoot->mRight = nullptr;
	mRoot->mEmpty = 1;

	mCounterID = 0;
}