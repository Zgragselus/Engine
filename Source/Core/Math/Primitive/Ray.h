#ifndef __RAY__H__
#define __RAY__H__

#include "AABB.h"
#include "Plane.h"
#include "../Numeric/Common.h"

namespace Engine
{
	class __declspec(align(16)) Ray
	{
	public:
		float4 origin;
		float4 direction;

		Ray(const float4& origin, const float4& dir) :
			origin(origin)
		{
			direction = normalize(dir);
		}

		bool TestPlane(const Plane& plane, float& distance)
		{
			float4 n = plane.eq * float4(1.0f, 1.0f, 1.0f, 0.0f);
			float denom = dot(n, direction);
			if (denom > 1e-6 || denom < -1e-6)
			{
				float4 v = (n * -plane.eq.w) - origin;
				distance = dot(v, n) / denom;
				return (distance > 0.0f);
			}

			return false;
		}

		bool TestAABB(const AABB& box, float& enter, float& exit)
		{
			float4 tmin = (box.mMin - origin) / direction;
			float4 tmax = (box.mMax - origin) / direction;
			float4 tnear = f4min(tmin, tmax);
			float4 tfar = f4max(tmin, tmax);
			enter = Math::Max(Math::Max(tnear.x, 0.0f), Math::Max(tnear.y, tnear.z));
			exit = Math::Min(tfar.x, Math::Min(tfar.y, tfar.z));
			return (exit > 0.0f) && (enter < exit);
		}

		bool TestTriangle(const float4& a, const float4& b, const float4& c, float& distance, float& u, float& v)
		{
			float4 e1 = b - a;
			float4 e2 = c - a;
			float4 pvec = cross(direction, e2);
			float det = dot(e1, pvec);
			if (det > -1e-6 && det < 1e-6)
			{
				return false;
			}
			float inv_det = 1.0f / det;

			float4 tvec = origin - a;
			u = dot(tvec, pvec) * inv_det;
			if (u < 0.0f || u > 1.0f)
			{
				return false;
			}

			float4 qvec = cross(tvec, e1);
			v = dot(direction, qvec) * inv_det;
			if (v < 0.0f || u + v > 1.0f)
			{
				return false;
			}

			distance = dot(e2, qvec) * inv_det;
			return (distance > 0.0f);
		}

		bool TestLine(const float4& lineStart, const float4& lineEnd, float intersectDistance, float& distance, float& s, float& t)
		{
			float4 u = direction;
			float4 v = lineEnd - lineStart;
			float4 w = origin - lineStart;
			float a = dot(u, u);
			float b = dot(u, v);
			float c = dot(v, v);
			float d = dot(u, w);
			float e = dot(v, w);
			float D = a * c - b * b;
			float sc, sN, sD = D;
			float tc, tN, tD = D;

			if (D < 0.0005f)
			{
				sN = 0.0f;
				sD = 1.0f;
				tN = e;
				tD = c;
			}
			else
			{
				sN = (b * e - c * d);
				tN = (a * e - b * d);
				if (sN < 0.0f)
				{
					sN = 0.0f;
					tN = e;
					tD = c;
				}
			}

			if (tN < 0.0f)
			{
				tN = 0.0f;
				if (-d < 0.0f)
				{
					sN = 0.0f;
				}
				else
				{
					sN = -d;
					sD = a;
				}
			}
			else if (tN > tD)
			{
				tN = tD;
				if ((-d + b) < 0.0f)
				{
					sN = 0.0f;
				}
				else
				{
					sN = (-d + b);
					sD = a;
				}
			}

			sc = (fabsf(sN) < 0.0005f ? 0.0f : sN / sD);
			tc = (fabsf(tN) < 0.0005f ? 0.0f : tN / tD);

			float4 dP = w + (sc * u) - (tc * v);
			distance = length(dP);
			s = sc;
			t = tc;

			return (distance / length(w + sc * u - origin) <= intersectDistance);
		}
	};
}

#endif 