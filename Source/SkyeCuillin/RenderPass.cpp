#include "RenderPass.h"

using namespace SkyeCuillin;

RenderPass::RenderPass(Engine::D3DRenderer* renderer, unsigned int width, unsigned int height)
{
	mRenderer = renderer;
	mWidth = width;
	mHeight = height;
}

RenderPass::~RenderPass()
{

}