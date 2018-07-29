#ifndef __RENDER_PASS__H__
#define __RENDER_PASS__H__

#include "Graphics/Graphics.h" 
#include "Game/Game.h"
#include "Core/Math/Numeric/Float4.h"

namespace SkyeCuillin
{
	class RenderPass
	{
	protected:
		Engine::D3DRenderer* mRenderer;

		unsigned int mWidth;
		unsigned int mHeight;

	public:
		RenderPass(Engine::D3DRenderer* renderer, unsigned int width, unsigned int height);
		virtual ~RenderPass();
		virtual void Resize(unsigned int width, unsigned int height) = 0;
	};
}

#endif