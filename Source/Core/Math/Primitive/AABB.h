#ifndef __AABB__H__
#define __AABB__H__

#include "../Numeric/Float4.h"
#include "../Numeric/Mat4.h"

namespace Engine
{
	class AABB
	{
	public:
		float4 mMin;
		float4 mMax;

		AABB()
		{
			mMin = float4(INFINITY, INFINITY, INFINITY, 1.0f);
			mMax = float4(-INFINITY, -INFINITY, -INFINITY, 1.0f);
		}

		AABB(const AABB& box)
		{
			mMin = box.mMin;
			mMax = box.mMax;
		}

		AABB(const float4& p)
		{
			mMin = mMax = p;
		}

		AABB(const float4& a, const float4& b)
		{
			mMin = f4min(a, b);
			mMax = f4max(a, b);
		}

		void Expand(const float4& p)
		{
			mMin = f4min(mMin, p);
			mMax = f4max(mMax, p);
		}

		void Union(const AABB& b)
		{
			mMin = f4min(b.mMin, mMin);
			mMax = f4max(b.mMax, mMax);
		}

		AABB TransformAABB(const mat4& matrix) const
		{
			float4 verts[8] =
			{
				mMin,
				float4(mMin.x, mMin.y, mMax.z, 1.0f),
				float4(mMin.x, mMax.y, mMin.z, 1.0f),
				float4(mMax.x, mMin.y, mMin.z, 1.0f),
				float4(mMax.x, mMax.y, mMin.z, 1.0f),
				float4(mMax.x, mMin.y, mMax.z, 1.0f),
				float4(mMin.x, mMax.y, mMax.z, 1.0f),
				mMax
			};

			AABB result;

			for (int i = 0; i < 8; i++)
			{
				result.Union(verts[i] * matrix);
			}
			
			return result;
		}

		float4 GetVertexP(const float4& normal) const 
		{
			//float4 p = mask_gt(mMin, mMax, normal, float4(0.0f, 0.0f, 0.0f, 0.0f));
			float4 p = mMin;
			if (normal.x >= 0.0f)
				p.x = mMax.x;
			if (normal.y >= 0.0f)
				p.y = mMax.y;
			if (normal.z >= 0.0f)
				p.z = mMax.z;
			p.w = 1.0f;
			return p;
		}

		float4 GetVertexN(const float4& normal) const
		{
			//float4 n = mask_gt(mMin, mMax, normal, float4(0.0f, 0.0f, 0.0f, 0.0f));
			float4 n = mMax;
			if (normal.x >= 0.0f)
				n.x = mMin.x;
			if (normal.y >= 0.0f)
				n.y = mMin.y;
			if (normal.z >= 0.0f)
				n.z = mMin.z;
			n.w = 1.0f;
			return n;
		}

		int MaximumExtent() const
		{
			float4 d = mMax - mMin;
			if (d.x > d.y)
			{
				if (d.x > d.z)
				{
					return 0;
				}
				else
				{
					return 2;
				}
			}
			else
			{
				if (d.y > d.z)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}

		float4 Offset(const float4& p) const
		{
			return ((p - mMin) / (mMax - mMin));
		}

		float SurfaceArea() const
		{
			float4 d = mMax - mMin;
			return 2.0f * (d.x * d.y + d.x * d.z + d.y * d.z);
		}

		ALIGNED_NEW_DELETE("Engine::Math::AABB")
	};
}

#endif