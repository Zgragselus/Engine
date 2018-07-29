#ifndef _QUATERNION_H__
#define _QUATERNION_H__

#include "Float4.h"

namespace Engine
{
	struct __declspec(align(16)) quat
	{
	public:
		////////////////////
		/** Constructors **/

		/* Default constructor */
		inline quat()
		{
			xmm = _mm_setzero_ps();
		}

		/* Float constructor */
		explicit inline quat(float f)
		{
			xmm = _mm_set1_ps(f);
		}

		/* Scalar constructor */
		inline quat(float _x, float _y, float _z, float _w)
		{
			xmm = _mm_setr_ps(_x, _y, _z, _w);
		}

		/* Scalar constructor */
		inline quat(float _x, float _y, float _z)
		{
			float t = 1.0f - (_x * _x) - (_y * _y) - (_z * _z);

			if (t < 0.0f)
			{
				t = 0.0f;
			}
			else
			{
				t = -sqrt(t);
			}

			xmm = _mm_setr_ps(_x, _y, _z, t);
		}

		/* Array constructor (unaligned!!!) */
		inline quat(const float* fv)
		{
			xmm = _mm_loadu_ps(fv);
		}

		/* Copy constructor */
		inline quat(const quat &v)
		{
			xmm = v.xmm;
		}

		/* SSE constructor */
		inline quat(const __m128 &_m)
		{
			xmm = _m;
		}

		/* Constructor from 2 vectors */
		inline quat(const float4& u, const float4& v)
		{
			float norm_u_norm_v = sqrt(dot(u, u) * dot(v, v));
			float real_part = norm_u_norm_v + dot(u, v);
			float4 axis;
			if (real_part < 1.e-6f * norm_u_norm_v)
			{
				real_part = 0.0f;
				axis = (fabsf(u.x) > fabsf(u.z)) ? float4(-u.y, u.x, 0.0f, 0.0f) :
					float4(0.0f, -u.z, u.y, 0.0f);
			}
			else
			{
				axis = cross(u, v);
			}
			x = axis.x;
			y = axis.y;
			z = axis.z;
			w = real_part;
			float mag2 = w * w + x * x + y * y + z * z;
			if (fabs(mag2) > 0.0001f && fabs(mag2 - 1.0f) > 0.0001f)
			{
				float mag = 1.0f / sqrt(mag2);
				w *= mag;
				x *= mag;
				y *= mag;
				z *= mag;
			}
		}

		///////////////
		/** Methods **/

		/* Operator - */
		inline friend const quat operator -(const quat& v)
		{
			return _mm_xor_ps(v.xmm, _mm_set1_ps(-0.f));
		}

		/* Vector addition */
		inline quat operator + (const quat& v) const
		{
			return quat(_mm_add_ps(xmm, v.xmm));
		}

		/* Scalar multiplication */
		inline quat operator * (float f) const
		{
			return quat(_mm_mul_ps(xmm, _mm_set1_ps(f)));
		}

		/* Vector multiplication */
		inline quat operator * (const quat& q) const
		{
			quat r;
			r.x = (x * q.w) + (w * q.x) + (y * q.z) - (z * q.y);
			r.y = (y * q.w) + (w * q.y) + (z * q.x) - (x * q.z);
			r.z = (z * q.w) + (w * q.z) + (x * q.y) - (y * q.x);
			r.w = (w * q.w) - (x * q.x) - (y * q.y) - (z * q.z);
			/*r.x = w * rq.x + x * rq.w + y * rq.z - z * rq.y;
			r.y = w * rq.y + y * rq.w + z * rq.x - x * rq.z;
			r.z = w * rq.z + z * rq.w + x * rq.y - y * rq.x;
			r.w = w * rq.w - x * rq.x - y * rq.y - z * rq.z;*/
			return r;
		}

		/* Scalar division */
		inline quat operator / (float f) const
		{
			return quat(_mm_div_ps(xmm, _mm_set1_ps(f)));
		}

		/* Inverse */
		friend inline quat inverse(const quat &v)
		{
			return quat(-v.x, -v.y, -v.z, v.w);
		}

		inline quat inverse() const
		{
			quat q(-x, -y, -z, w);
			return q;
		}

		/* Normalization */
		friend inline const quat normalize(const quat& v)
		{
			/*__m128 l = _mm_mul_ps(v.xmm, v.xmm);
			l = _mm_add_ps(l, _mm_shuffle_ps(l, l, 0x4E));
			return _mm_mul_ps(v.xmm, _mm_rsqrt_ps(_mm_add_ps(l, _mm_shuffle_ps(l, l, 0x11))));	*/

			quat r = v;
			float mag2 = v.w * v.w + v.x * v.x + v.y * v.y + v.z * v.z;
			if (fabs(mag2) > 0.0001f && fabs(mag2 - 1.0f) > 0.0001f)
			{
				float mag = sqrt(mag2);
				r.w /= mag;
				r.x /= mag;
				r.y /= mag;
				r.z /= mag;
			}
			return r;
		}

		/* Dot product */
		friend inline float dot(const quat &v0, const quat &v1)
		{
			__m128 l = _mm_mul_ps(v0.xmm, v1.xmm);
			l = _mm_add_ps(l, _mm_shuffle_ps(l, l, 0x4E));
			return _mm_cvtss_f32(_mm_add_ss(l, _mm_shuffle_ps(l, l, 0x11)));
		}

		/* Linear interpolation */
		friend inline const quat lerp(const quat& v0, const quat& v1, float f)
		{
			__m128 ff = _mm_set1_ps(f);
			return _mm_add_ps(_mm_mul_ps(v0.xmm, _mm_sub_ps(_mm_set1_ps(1.f), ff)),
				_mm_mul_ps(v1.xmm, ff));
		}

		/* Spherical interpolation */
		friend inline const quat slerp(const quat& a, const quat &b, float t)
		{
			float w1, w2;
			bool flip = false;

			float cosTheta = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
			if (cosTheta < 0)
			{
				flip = true;
				cosTheta = a.x * (-b.x) + a.y * (-b.y) + a.z * (-b.z) + a.w * (-b.w);
			}

			float theta = (float)acos(cosTheta);
			float sinTheta = (float)sin(theta);

			if (sinTheta > 0.001f)
			{
				w1 = float(sin((1.0f - t)*theta) / sinTheta);
				w2 = float(sin(t*theta) / sinTheta);
			}
			else
			{
				w1 = 1.0f - t;
				w2 = t;
			}

			if (flip)
				return quat(a.x*w1 - b.x*w2, a.y*w1 - b.y*w2, a.z*w1 - b.z*w2, a.w*w1 - b.w*w2);
			else
				return quat(a.x*w1 + b.x*w2, a.y*w1 + b.y*w2, a.z*w1 + b.z*w2, a.w*w1 + b.w*w2);
		}

		/* Rotate vector */
		friend inline const float4 rotate(const quat& q, const float4& v)
		{
			const quat _v = quat(v.x, v.y, v.z, 0.0f);
			return float4((q * _v * q.inverse()).xmm);
		}

		inline quat(const float4& v)
		{
			float4 dir = float4(v.x, v.y, v.z, v.w);
			float4 up = float4(0, 1, 0, 0);
			float4 right = cross(up, dir);
			up = cross(dir, right);

			w = sqrtf(1.0f + right.x + up.y + dir.z) * 0.5f;
			float scale = 1.0f / (4.0f * w);
			x = (dir.y - up.z) * scale;
			y = (right.z - dir.x) * scale;
			z = (up.x - right.y) * scale;
		}

		union
		{
			struct { float x, y, z, w; };		//< Accessing elements
			__m128 xmm;					//< As SSE register
		};

		ALIGNED_NEW_DELETE("Engine::Math::quat")
	};

	inline quat from_axisangle(const float4& axis, float angle)
	{
		float _ang = sin(angle * 0.5f);
		float4 _axis = normalize(axis);

		return quat(_axis.x * _ang, _axis.y * _ang, _axis.z * _ang, cos(angle * 0.5f));
	}

	inline quat from_euler(float roll, float pitch, float yaw)
	{
		float p = pitch * 3.141593f * 0.5f / 180.0f;
		float y = yaw * 3.141593f * 0.5f / 180.0f;
		float r = roll * 3.141593f * 0.5f / 180.0f;

		float sinp = sin(p);
		float siny = sin(y);
		float sinr = sin(r);
		float cosp = cos(p);
		float cosy = cos(y);
		float cosr = cos(r);

		quat q(sinr * cosp * cosy - cosr * sinp * siny,
			cosr * sinp * cosy + sinr * cosp * siny,
			cosr * cosp * siny - sinr * sinp * cosy,
			cosr * cosp * cosy + sinr * sinp * siny);

		return normalize(q);
	}

	inline void to_euler(const quat& q, float& roll, float& pitch, float& yaw)
	{
		// Roll (x-axis)
		float sinr = 2.0f * (q.w * q.x + q.y * q.z);
		float cosr = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
		roll = atan2(sinr, cosr) * 180.0f / 3.141593f;

		// Pitch (y-axis)
		float sinp = 2.0f * (q.w * q.y - q.z * q.x);
		if (fabsf(sinp) >= 1.0f)
		{
			pitch = sinp > 0.0f ? 90.0f : -90.0f;
		}
		else
		{
			pitch = asin(sinp) * 180.0f / 3.141593f;
		}

		// Yaw (z-axis)
		float siny = 2.0f * (q.w * q.z + q.x * q.y);
		float cosy = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
		yaw = atan2(siny, cosy) * 180.0f / 3.141593f;
	}
}

#endif
