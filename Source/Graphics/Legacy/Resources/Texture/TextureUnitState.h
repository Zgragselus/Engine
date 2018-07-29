///////////////////////////////////////////////////////////////////////////////////////////////////
//
// TextureUnitState.h
//
// 
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __TEXTURE_UNIT_STATE__H__
#define __TEXTURE_UNIT_STATE__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/Memory/Memory.h"
#include "Graphics/Resources/Texture/Texture.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

#define TEXTURE_UNIT_COUNT 16

namespace Engine
{
	/// <summary>
	/// A single item to be rendered by graphics pipeline
	/// </summary>
	class TextureUnitState
	{
	public:
		/// <summary>Pointer to texture per each texture unit</summary>
		Texture* mTexel[TEXTURE_UNIT_COUNT];

		/// <summary>Default constructor</summary>
		TextureUnitState()
		{
			for (unsigned int i = 0; i < TEXTURE_UNIT_COUNT; i++)
			{
				mTexel[i] = NULL;
			}
		}

		/// <summary>Constructor for all texture units at once</summary>
		TextureUnitState(Texture* texel0,
			Texture* texel1 = NULL,
			Texture* texel2 = NULL,
			Texture* texel3 = NULL,
			Texture* texel4 = NULL,
			Texture* texel5 = NULL,
			Texture* texel6 = NULL,
			Texture* texel7 = NULL,
			Texture* texel8 = NULL,
			Texture* texel9 = NULL,
			Texture* texel10 = NULL,
			Texture* texel11 = NULL,
			Texture* texel12 = NULL,
			Texture* texel13 = NULL,
			Texture* texel14 = NULL,
			Texture* texel15 = NULL)
		{
			mTexel[0] = texel0;
			mTexel[1] = texel1;
			mTexel[2] = texel2;
			mTexel[3] = texel3;
			mTexel[4] = texel4;
			mTexel[5] = texel5;
			mTexel[6] = texel6;
			mTexel[7] = texel7;
			mTexel[8] = texel8;
			mTexel[9] = texel9;
			mTexel[10] = texel10;
			mTexel[11] = texel11;
			mTexel[12] = texel12;
			mTexel[13] = texel13;
			mTexel[14] = texel14;
			mTexel[15] = texel15;
		}

		/// <summary>Enable the current texture unit state</summary>
		void Enable()
		{
			for (unsigned int i = 0; i < TEXTURE_UNIT_COUNT; i++)
			{
				if (mTexel[i] != NULL)
				{
					mTexel[i]->BindTexture(i);
				}
			}
		}

		ALIGNED_NEW_DELETE("Engine::Graphics::TextureUnitState")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif
