#include "RenderPassFullscreen.h"

using namespace SkyeCuillin;

size_t RenderPassFullscreen::mRefCounter = 0;
Engine::Mesh* RenderPassFullscreen::mMesh = nullptr;

RenderPassFullscreen::RenderPassFullscreen(Engine::D3DRenderer* renderer, unsigned int width, unsigned int height) : 
	RenderPass(renderer, width, height)
{
	if (mRefCounter == 0)
	{
		float triangleVertices[] =
		{
			-1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 1.0f
		};

		unsigned int triangleIndices[] =
		{
			0, 1, 2,
			3, 4, 5
		};

		mMesh = new Engine::Mesh(mRenderer, triangleVertices, sizeof(float) * 5, 6, triangleIndices, 6);
	}
}

RenderPassFullscreen::~RenderPassFullscreen()
{
	mRefCounter--;

	if (mRefCounter == 0)
	{
		delete mMesh;
		mMesh = nullptr;
	}
}