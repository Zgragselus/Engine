#ifndef __RENDER_PASS_FULLSCREEN__H__
#define __RENDER_PASS_FULLSCREEN__H__

#include "RenderPass.h"

namespace SkyeCuillin
{
	class RenderPassFullscreen : public RenderPass
	{
	protected:
		static Engine::Mesh* mMesh;
		static size_t mRefCounter;

	public:
		RenderPassFullscreen(Engine::D3DRenderer* renderer, unsigned int width, unsigned int height);
		virtual ~RenderPassFullscreen();
		virtual void Resize(unsigned int width, unsigned int height) = 0;
	};
}

#endif