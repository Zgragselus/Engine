#ifndef __FRUSTUM__H__
#define __FRUSTUM__H__

#include "Plane.h"
#include "AABB.h"

namespace Engine
{
	class __declspec(align(16)) Frustum
	{
	private:
		float4 pt[8];
		Plane planes[6];

		enum
		{
			TOP = 0,
			BOTTOM,
			LEFT,
			RIGHT,
			NEARP,
			FARP
		};

	public:
		enum IntersectResult
		{
			OUTSIDE = 0,
			INTERSECT,
			INSIDE
		};

		void InitOrtho(const float4& position, const float4& forward, const float4& up, float left, float right, float top, float bottom, float front, float back)
		{
			float4 _right = normalize(cross(up, forward));
			float4 _up = cross(forward, _right);

			planes[TOP] = Plane::CreateFromPointNormal(position + _up * top, -_up);
			planes[BOTTOM] = Plane::CreateFromPointNormal(position + _up * bottom, _up);
			planes[LEFT] = Plane::CreateFromPointNormal(position + _right * left, _right);
			planes[RIGHT] = Plane::CreateFromPointNormal(position + _right * right, -_right);
			planes[NEARP] = Plane::CreateFromPointNormal(position + forward * front, forward);
			planes[FARP] = Plane::CreateFromPointNormal(position + forward * back, -forward);
		}

		void InitPerspective(const float4& position, const float4& forward, const float4& up, float fov, float ratio, float nearPlane, float farPlane)
		{
			float4 right = normalize(cross(forward, up));
			float4 _up = cross(right, forward);

			float tang = tanf(fov * 0.5f);
			//float tang = tanf(0.463647f);
			float nearWidth = nearPlane * tang;
			float nearHeight = nearWidth * ratio;
			float farWidth = farPlane * tang;
			float farHeight = farWidth * ratio;

			float4 nearCenter = position + forward * nearPlane;
			float4 farCenter = position + forward * farPlane;

			pt[0] = nearCenter + nearHeight * _up - nearWidth * right; // Near top left
			pt[1] = nearCenter + nearHeight * _up + nearWidth * right; // Near top right
			pt[2] = nearCenter - nearHeight * _up - nearWidth * right; // Near bot left
			pt[3] = nearCenter - nearHeight * _up + nearWidth * right; // Near bot right
			pt[4] = farCenter + farHeight * _up - farWidth * right; // Far top left
			pt[5] = farCenter + farHeight * _up + farWidth * right; // Far top right
			pt[6] = farCenter - farHeight * _up - farWidth * right; // Far bot left
			pt[7] = farCenter - farHeight * _up + farWidth * right; // Far bot right

			planes[TOP] = Plane::CreateFromPoints(position, pt[0], pt[1]);
			planes[BOTTOM] = Plane::CreateFromPoints(position, pt[3], pt[2]);
			planes[LEFT] = Plane::CreateFromPoints(position, pt[2], pt[0]);
			planes[RIGHT] = Plane::CreateFromPoints(position, pt[1], pt[3]);
			planes[NEARP] = Plane::CreateFromPoints(pt[2], pt[0], pt[3]);
			planes[FARP] = Plane::CreateFromPoints(pt[5], pt[4], pt[7]);
		}

		IntersectResult TestPoint(const float4& pt)
		{
			IntersectResult result = INSIDE;

			for (unsigned int i = 0; i < 6; i++)
			{
				if (planes[i].Distance(pt) < 0.0f)
				{
					result = OUTSIDE;
					break;
				}
			}

			return result;
		}

		IntersectResult TestAABB(const AABB& box)
		{
			IntersectResult result = INSIDE;

			for (unsigned int i = 0; i < 6; i++)
			{
				if (planes[i].Distance(box.GetVertexP(planes[i].eq)) < 0.0f)
				{
					return OUTSIDE;
				}
				else if (planes[i].Distance(box.GetVertexN(planes[i].eq)) < 0.0f)
				{
					result = INTERSECT;
				}
			}

			// Precise frustum culling
			/*if (result == INTERSECT)
			{
				int out = 0;
				out = 0; for (int i = 0; i < 8; i++) out += ((pt[i].x > box.mMax.x) ? 1 : 0); if (out == 8) return OUTSIDE;
				out = 0; for (int i = 0; i < 8; i++) out += ((pt[i].x < box.mMin.x) ? 1 : 0); if (out == 8) return OUTSIDE;
				out = 0; for (int i = 0; i < 8; i++) out += ((pt[i].y > box.mMax.y) ? 1 : 0); if (out == 8) return OUTSIDE;
				out = 0; for (int i = 0; i < 8; i++) out += ((pt[i].y < box.mMin.y) ? 1 : 0); if (out == 8) return OUTSIDE;
				out = 0; for (int i = 0; i < 8; i++) out += ((pt[i].z > box.mMax.z) ? 1 : 0); if (out == 8) return OUTSIDE;
				out = 0; for (int i = 0; i < 8; i++) out += ((pt[i].z < box.mMin.z) ? 1 : 0); if (out == 8) return OUTSIDE;
			}*/

			return result;
		}

		ALIGNED_NEW_DELETE("Engine::Math::Frustum")
	};
}

#endif