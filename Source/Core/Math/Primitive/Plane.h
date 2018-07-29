#ifndef __PLANE__H__
#define __PLANE__H__

#include "Core/Math/Numeric/Float4.h"

namespace Engine
{
	class __declspec(align(16)) Plane
	{
	public:
		float4 eq;

		Plane()
		{
			eq = float4(0.0f, 0.0f, 0.0f, 0.0f);
		}

		Plane(const Plane& p)
		{
			eq = p.eq;
		}

		Plane(float a, float b, float c, float d)
		{
			eq = float4(a, b, c, d);
		}

		Plane(const float4& normal, float offset)
		{
			eq = normal;
			eq.w = offset;
		}

		static Plane CreateFromPointNormal(const float4& point, const float4& normal)
		{
			Plane p;
			p.eq = normal;
			p.eq.w = -(normal.x * point.x + normal.y * point.y + normal.z * point.z);
			return p;
		}

		static Plane CreateFromPointVectors(const float4& point, const float4& v1, const float4& v2)
		{
			float4 normal = cross(v1, v2);
			return CreateFromPointNormal(point, normal);
		}

		static Plane CreateFromPoints(const float4& pt1, const float4& pt2, const float4& pt3)
		{
			float4 normal = normalize(cross(pt2 - pt1, pt3 - pt1));
			return CreateFromPointNormal(pt1, normal);
		}

		float Distance(const float4& pt)
		{
			return dot(pt, eq);
		}

		ALIGNED_NEW_DELETE("Engine::Math::Plane")
	};
}

#endif