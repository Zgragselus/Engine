#ifndef _MATRIX4x4_H__
#define _MATRIX4x4_H__

#include "Quat.h"

namespace Engine
{
	struct __declspec(align(16)) mat4
	{
	public:
		// Could be used for compilers that doesn't use PSHUFD instruction (e.g. I bet MSVC, GCC uses it :P)
		//#define _mm_shufd(xmm, mask) _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(xmm), mask))

		////////////////////
		/** Constructors **/

		/* Identity constructor */
		inline mat4()
		{
			m1 = _mm_setr_ps(1.0f, 0.0f, 0.0f, 0.0f);
			m2 = _mm_setr_ps(0.0f, 1.0f, 0.0f, 0.0f);
			m3 = _mm_setr_ps(0.0f, 0.0f, 1.0f, 0.0f);
			m4 = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);
		}

		/* Scale constructor */
		explicit inline mat4(float f)
		{
			m1 = _mm_setr_ps(f, 0.0f, 0.0f, 0.0f);
			m2 = _mm_setr_ps(0.0f, f, 0.0f, 0.0f);
			m3 = _mm_setr_ps(0.0f, 0.0f, f, 0.0f);
			m4 = _mm_setr_ps(0.0f, 0.0f, 0.0f, f);
		}

		/* Vector constructor */
		inline mat4(const float4& _v1,
			const float4& _v2,
			const float4& _v3,
			const float4& _v4)
		{
			m1 = _v1.xmm;
			m2 = _v2.xmm;
			m3 = _v3.xmm;
			m4 = _v4.xmm;
		}

		/* SSE constructor */
		inline mat4(const __m128& _v1,
			const __m128& _v2,
			const __m128& _v3,
			const __m128& _v4)
		{
			m1 = _v1;
			m2 = _v2;
			m3 = _v3;
			m4 = _v4;
		}

		/* Scalar constructor */
		inline mat4(float  _f1, float  _f2, float  _f3, float  _f4,
			float  _f5, float  _f6, float  _f7, float  _f8,
			float  _f9, float _f10, float _f11, float _f12,
			float _f13, float _f14, float _f15, float _f16)
		{
			m1 = _mm_setr_ps(_f1, _f2, _f3, _f4);
			m2 = _mm_setr_ps(_f5, _f6, _f7, _f8);
			m3 = _mm_setr_ps(_f9, _f10, _f11, _f12);
			m4 = _mm_setr_ps(_f13, _f14, _f15, _f16);
		}

		/* Copy constructor */
		inline mat4(const mat4& _m)
		{
			m1 = _m.m1;
			m2 = _m.m2;
			m3 = _m.m3;
			m4 = _m.m4;
		}

		/** TODO! **/
		inline mat4(const quat& _q)
		{
			quat q = normalize(_q);

			m1 = _mm_setr_ps(1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z, 
				2.0f * q.x * q.y - 2.0f * q.z * q.w, 
				2.0f * q.x * q.z + 2.0f * q.y * q.w, 
				0.0f);
			m2 = _mm_setr_ps(2.0f * q.x * q.y + 2.0f * q.z * q.w, 
				1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z, 
				2.0f * q.y * q.z - 2.0f * q.x * q.w, 
				0.0f);
			m3 = _mm_setr_ps(2.0f * q.x * q.z - 2.0f * q.y * q.w, 
				2.0f * q.y * q.z + 2.0f * q.x * q.w, 
				1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y, 
				0.0f);
			m4 = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);
		}

		inline mat4(const float4& _p)
		{
			m1 = _mm_setr_ps(1.0f, 0.0f, 0.0f, _p.x);
			m2 = _mm_setr_ps(0.0f, 1.0f, 0.0f, _p.y);
			m3 = _mm_setr_ps(0.0f, 0.0f, 1.0f, _p.z);
			m4 = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);
		}

		inline mat4(float d1, float d2, float d3)
		{
			m1 = _mm_setr_ps(  d1, 0.0f, 0.0f, 0.0f);
			m2 = _mm_setr_ps(0.0f,   d2, 0.0f, 0.0f);
			m3 = _mm_setr_ps(0.0f, 0.0f,   d3, 0.0f);
			m4 = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);
		}

		/////////////////
		/** Accessing **/

		/* Write direct-access operator */
		inline float4& operator [](int index)
		{
			return reinterpret_cast<float4&>(m[index]);
		}

		/* Read direct-access operator */
		inline const float4& operator [](int index) const
		{
			return reinterpret_cast<const float4&>(m[index]);
		}

		/* Cast operator */
		inline operator float*()
		{
			return reinterpret_cast<float*>(this);
		}

		/* Const cast operator */
		inline operator const float*() const
		{
			return reinterpret_cast<const float*>(this);
		}

		/////////////////////////
		/** Unary Arithmetics **/

		/* Scalar addition */
		inline mat4& operator += (float f)
		{
			__m128 ff = _mm_set1_ps(f);
			m1 = _mm_add_ps(m1, ff);
			m2 = _mm_add_ps(m2, ff);
			m3 = _mm_add_ps(m3, ff);
			m4 = _mm_add_ps(m4, ff);
			return *this;
		}

		/* Matrix addition */
		inline mat4& operator += (const mat4& m)
		{
			m1 = _mm_add_ps(m1, m.m1);
			m2 = _mm_add_ps(m2, m.m2);
			m3 = _mm_add_ps(m3, m.m3);
			m4 = _mm_add_ps(m4, m.m4);
			return *this;
		}

		/* Scalar subtraction */
		inline mat4& operator -= (float f)
		{
			__m128 ff = _mm_set1_ps(f);
			m1 = _mm_sub_ps(m1, ff);
			m2 = _mm_sub_ps(m2, ff);
			m3 = _mm_sub_ps(m3, ff);
			m4 = _mm_sub_ps(m4, ff);
			return *this;
		}

		/* Matrix subtraction */
		inline mat4& operator -= (const mat4& m)
		{
			m1 = _mm_sub_ps(m1, m.m1);
			m2 = _mm_sub_ps(m2, m.m2);
			m3 = _mm_sub_ps(m3, m.m3);
			m4 = _mm_sub_ps(m4, m.m4);
			return *this;
		}

		/* Scalar multiplication */
		inline mat4& operator *= (float f)
		{
			__m128 ff = _mm_set1_ps(f);
			m1 = _mm_mul_ps(m1, ff);
			m2 = _mm_mul_ps(m2, ff);
			m3 = _mm_mul_ps(m3, ff);
			m4 = _mm_mul_ps(m4, ff);
			return *this;
		}

		/* Matrix multiplication */
		inline mat4& operator *= (const mat4& m)
		{
			m1 = _mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(m1, m1, 0x00), m.m1),
				_mm_mul_ps(_mm_shuffle_ps(m1, m1, 0x55), m.m2)),
				_mm_mul_ps(_mm_shuffle_ps(m1, m1, 0xaa), m.m3)),
				_mm_mul_ps(_mm_shuffle_ps(m1, m1, 0xff), m.m4));

			m2 = _mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(m2, m2, 0x00), m.m1),
				_mm_mul_ps(_mm_shuffle_ps(m2, m2, 0x55), m.m2)),
				_mm_mul_ps(_mm_shuffle_ps(m2, m2, 0xaa), m.m3)),
				_mm_mul_ps(_mm_shuffle_ps(m2, m2, 0xff), m.m4));

			m3 = _mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(m3, m3, 0x00), m.m1),
				_mm_mul_ps(_mm_shuffle_ps(m3, m3, 0x55), m.m2)),
				_mm_mul_ps(_mm_shuffle_ps(m3, m3, 0xaa), m.m3)),
				_mm_mul_ps(_mm_shuffle_ps(m3, m3, 0xff), m.m4));

			m4 = _mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(m4, m4, 0x00), m.m1),
				_mm_mul_ps(_mm_shuffle_ps(m4, m4, 0x55), m.m2)),
				_mm_mul_ps(_mm_shuffle_ps(m4, m4, 0xaa), m.m3)),
				_mm_mul_ps(_mm_shuffle_ps(m4, m4, 0xff), m.m4));

			return *this;
		}

		/* Scalar division */
		inline mat4& operator /= (float f)
		{
			__m128 ff = _mm_set1_ps(f);
			m1 = _mm_div_ps(m1, ff);
			m2 = _mm_div_ps(m2, ff);
			m3 = _mm_div_ps(m3, ff);
			m4 = _mm_div_ps(m4, ff);
			return *this;
		}

		/* Matrix division */
		inline mat4& operator /= (const mat4& m)
		{
			m1 = _mm_div_ps(m1, m.m1);
			m2 = _mm_div_ps(m2, m.m2);
			m3 = _mm_div_ps(m3, m.m3);
			m4 = _mm_div_ps(m4, m.m4);
			return *this;
		}

		//////////////////////////
		/** Binary Arithmetics **/

		/* Scalar addition */
		friend inline mat4 operator + (const mat4& m, float f)
		{
			__m128 ff = _mm_set1_ps(f);
			return mat4(_mm_add_ps(m.m1, ff),
				_mm_add_ps(m.m2, ff),
				_mm_add_ps(m.m3, ff),
				_mm_add_ps(m.m4, ff));
		}

		/* Matrix addition */
		friend inline mat4 operator + (const mat4& m0, const mat4& m1)
		{
			return mat4(_mm_add_ps(m0.m1, m1.m1),
				_mm_add_ps(m0.m2, m1.m2),
				_mm_add_ps(m0.m3, m1.m3),
				_mm_add_ps(m0.m4, m1.m4));
		}

		/* Scalar subtraction */
		friend inline mat4 operator - (const mat4& m, float f)
		{
			__m128 ff = _mm_set1_ps(f);
			return mat4(_mm_sub_ps(m.m1, ff),
				_mm_sub_ps(m.m2, ff),
				_mm_sub_ps(m.m3, ff),
				_mm_sub_ps(m.m4, ff));
		}

		/* Matrix subtraction */
		friend inline mat4 operator - (const mat4& m0, const mat4& m1)
		{
			return mat4(_mm_sub_ps(m0.m1, m1.m1),
				_mm_sub_ps(m0.m2, m1.m2),
				_mm_sub_ps(m0.m3, m1.m3),
				_mm_sub_ps(m0.m4, m1.m4));
		}

		/* Scalar multiplication */
		friend inline mat4 operator * (const mat4& m, float f)
		{
			__m128 ff = _mm_set1_ps(f);
			return mat4(_mm_mul_ps(m.m1, ff),
				_mm_mul_ps(m.m2, ff),
				_mm_mul_ps(m.m3, ff),
				_mm_mul_ps(m.m4, ff));
		}

		/* Matrix-Vector multiplication */
		friend inline float4 operator * (const mat4& m, const float4& v)
		{
			return float4(_mm_add_ps(_mm_add_ps(_mm_mul_ps(m.m1, _mm_shuffle_ps(v.xmm, v.xmm, 0x00)),
				_mm_mul_ps(m.m2, _mm_shuffle_ps(v.xmm, v.xmm, 0x55))),
				_mm_add_ps(_mm_mul_ps(m.m3, _mm_shuffle_ps(v.xmm, v.xmm, 0xaa)),
				_mm_mul_ps(m.m4, _mm_shuffle_ps(v.xmm, v.xmm, 0xff)))));
		}

		/* Vector-Matrix multiplication */
		friend inline float4 operator * (const float4& v, const mat4& m)
		{
			__m128 t1 = _mm_unpacklo_ps(m.m1, m.m2);
			__m128 t2 = _mm_unpacklo_ps(m.m3, m.m4);
			__m128 t3 = _mm_unpackhi_ps(m.m1, m.m2);
			__m128 t4 = _mm_unpackhi_ps(m.m3, m.m4);

			return float4(_mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_movelh_ps(t1, t2), _mm_shuffle_ps(v.xmm, v.xmm, 0x00)),
				_mm_mul_ps(_mm_movehl_ps(t2, t1), _mm_shuffle_ps(v.xmm, v.xmm, 0x55))),
				_mm_add_ps(_mm_mul_ps(_mm_movelh_ps(t3, t4), _mm_shuffle_ps(v.xmm, v.xmm, 0xaa)),
				_mm_mul_ps(_mm_movehl_ps(t4, t3), _mm_shuffle_ps(v.xmm, v.xmm, 0xff)))));
		}

		/* Matrix-Matrix multiplication */
		friend inline mat4 operator * (const mat4 &m0, const mat4 &m1)
		{
			return mat4(_mm_add_ps(_mm_add_ps(
				_mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(m0.m1, m0.m1, 0x00), m1.m1),
				_mm_mul_ps(_mm_shuffle_ps(m0.m1, m0.m1, 0x55), m1.m2)),
				_mm_mul_ps(_mm_shuffle_ps(m0.m1, m0.m1, 0xAA), m1.m3)),
				_mm_mul_ps(_mm_shuffle_ps(m0.m1, m0.m1, 0xFF), m1.m4)),
				_mm_add_ps(_mm_add_ps(
				_mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(m0.m2, m0.m2, 0x00), m1.m1),
				_mm_mul_ps(_mm_shuffle_ps(m0.m2, m0.m2, 0x55), m1.m2)),
				_mm_mul_ps(_mm_shuffle_ps(m0.m2, m0.m2, 0xAA), m1.m3)),
				_mm_mul_ps(_mm_shuffle_ps(m0.m2, m0.m2, 0xFF), m1.m4)),
				_mm_add_ps(_mm_add_ps(
				_mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(m0.m3, m0.m3, 0x00), m1.m1),
				_mm_mul_ps(_mm_shuffle_ps(m0.m3, m0.m3, 0x55), m1.m2)),
				_mm_mul_ps(_mm_shuffle_ps(m0.m3, m0.m3, 0xAA), m1.m3)),
				_mm_mul_ps(_mm_shuffle_ps(m0.m3, m0.m3, 0xFF), m1.m4)),
				_mm_add_ps(_mm_add_ps(
				_mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(m0.m4, m0.m4, 0x00), m1.m1),
				_mm_mul_ps(_mm_shuffle_ps(m0.m4, m0.m4, 0x55), m1.m2)),
				_mm_mul_ps(_mm_shuffle_ps(m0.m4, m0.m4, 0xAA), m1.m3)),
				_mm_mul_ps(_mm_shuffle_ps(m0.m4, m0.m4, 0xFF), m1.m4)));
		}

		/* Scalar division */
		friend inline mat4 operator / (const mat4& m, float f)
		{
			__m128 ff = _mm_set1_ps(f);
			return mat4(_mm_div_ps(m.m1, ff),
				_mm_div_ps(m.m2, ff),
				_mm_div_ps(m.m3, ff),
				_mm_div_ps(m.m4, ff));
		}

		/* Scalar division */
		friend inline mat4 operator / (float f, const mat4& m)
		{
			__m128 ff = _mm_set1_ps(f);
			return mat4(_mm_div_ps(m.m1, ff),
				_mm_div_ps(m.m2, ff),
				_mm_div_ps(m.m3, ff),
				_mm_div_ps(m.m4, ff));
		}

		/* Matrix division */
		friend inline mat4 operator / (const mat4& m0, const mat4& m1)
		{
			return mat4(_mm_div_ps(m0.m1, m1.m1),
				_mm_div_ps(m0.m2, m1.m2),
				_mm_div_ps(m0.m3, m1.m3),
				_mm_div_ps(m0.m4, m1.m4));
		}

		/* Matrix compontent multiplication */
		friend inline mat4 CompMult(const mat4& m0, const mat4& m1)
		{
			return mat4(_mm_mul_ps(m0.m1, m1.m1),
				_mm_mul_ps(m0.m2, m1.m2),
				_mm_mul_ps(m0.m3, m1.m3),
				_mm_mul_ps(m0.m4, m1.m4));
		}

		///////////////////////////////////
		/** Transposition and Inversion **/

		/* Transpose */
		friend inline mat4 transpose(const mat4& m)
		{
			__m128 t1 = _mm_unpacklo_ps(m.m1, m.m2);
			__m128 t2 = _mm_unpacklo_ps(m.m3, m.m4);
			__m128 t3 = _mm_unpackhi_ps(m.m1, m.m2);
			__m128 t4 = _mm_unpackhi_ps(m.m3, m.m4);

			return mat4(_mm_movelh_ps(t1, t2),
				_mm_movehl_ps(t2, t1),
				_mm_movelh_ps(t3, t4),
				_mm_movehl_ps(t4, t3));
		}

		/* Determinant */
		friend inline float determinant(const mat4& m)
		{
			__m128 r = _mm_shuffle_ps(m.m3, m.m3, 0x39);
			__m128 v1 = _mm_mul_ps(r, m.m4);
			__m128 v2 = _mm_mul_ps(r, _mm_shuffle_ps(m.m4, m.m4, 0x4e));
			__m128 v3 = _mm_mul_ps(r, _mm_shuffle_ps(m.m4, m.m4, 0x93));
			__m128 r1 = _mm_sub_ps(_mm_shuffle_ps(v2, v2, 0x39), _mm_shuffle_ps(v1, v1, 0x4e));
			__m128 r2 = _mm_sub_ps(_mm_shuffle_ps(v3, v3, 0x4e), v3);
			__m128 r3 = _mm_sub_ps(v2, _mm_shuffle_ps(v1, v1, 0x39));

			v1 = _mm_shuffle_ps(m.m2, m.m2, 0x93);
			v2 = _mm_shuffle_ps(m.m2, m.m2, 0x39);
			v3 = _mm_shuffle_ps(m.m2, m.m2, 0x4e);

			__m128 d = _mm_mul_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(v2, r1),
				_mm_mul_ps(v3, r2)),
				_mm_mul_ps(v1, r3)),
				m.m1);

			d = _mm_add_ps(d, _mm_shuffle_ps(d, d, 0x4e));
			d = _mm_sub_ss(d, _mm_shuffle_ps(d, d, 0x11));

			return _mm_cvtss_f32(d);
		}

		/* Inverse */
		friend inline mat4 inverse(const mat4& m)
		{
			__m128 f1 = _mm_sub_ps(_mm_mul_ps(_mm_shuffle_ps(m.m3, m.m2, 0xAA),
				_mm_shuffle_ps(_mm_shuffle_ps(m.m4, m.m3, 0xFF), _mm_shuffle_ps(m.m4, m.m3, 0xFF), 0x80)),
				_mm_mul_ps(_mm_shuffle_ps(_mm_shuffle_ps(m.m4, m.m3, 0xAA), _mm_shuffle_ps(m.m4, m.m3, 0xAA), 0x80),
				_mm_shuffle_ps(m.m3, m.m2, 0xFF)));

			__m128 f2 = _mm_sub_ps(_mm_mul_ps(_mm_shuffle_ps(m.m3, m.m2, 0x55),
				_mm_shuffle_ps(_mm_shuffle_ps(m.m4, m.m3, 0xFF), _mm_shuffle_ps(m.m4, m.m3, 0xFF), 0x80)),
				_mm_mul_ps(_mm_shuffle_ps(_mm_shuffle_ps(m.m4, m.m3, 0x55), _mm_shuffle_ps(m.m4, m.m3, 0x55), 0x80),
				_mm_shuffle_ps(m.m3, m.m2, 0xFF)));

			__m128 f3 = _mm_sub_ps(_mm_mul_ps(_mm_shuffle_ps(m.m3, m.m2, 0x55),
				_mm_shuffle_ps(_mm_shuffle_ps(m.m4, m.m3, 0xAA), _mm_shuffle_ps(m.m4, m.m3, 0xAA), 0x80)),
				_mm_mul_ps(_mm_shuffle_ps(_mm_shuffle_ps(m.m4, m.m3, 0x55), _mm_shuffle_ps(m.m4, m.m3, 0x55), 0x80),
				_mm_shuffle_ps(m.m3, m.m2, 0xAA)));

			__m128 f4 = _mm_sub_ps(_mm_mul_ps(_mm_shuffle_ps(m.m3, m.m2, 0x00),
				_mm_shuffle_ps(_mm_shuffle_ps(m.m4, m.m3, 0xFF), _mm_shuffle_ps(m.m4, m.m3, 0xFF), 0x80)),
				_mm_mul_ps(_mm_shuffle_ps(_mm_shuffle_ps(m.m4, m.m3, 0x00), _mm_shuffle_ps(m.m4, m.m3, 0x00), 0x80),
				_mm_shuffle_ps(m.m3, m.m2, 0xFF)));

			__m128 f5 = _mm_sub_ps(_mm_mul_ps(_mm_shuffle_ps(m.m3, m.m2, 0x00),
				_mm_shuffle_ps(_mm_shuffle_ps(m.m4, m.m3, 0xAA), _mm_shuffle_ps(m.m4, m.m3, 0xAA), 0x80)),
				_mm_mul_ps(_mm_shuffle_ps(_mm_shuffle_ps(m.m4, m.m3, 0x00), _mm_shuffle_ps(m.m4, m.m3, 0x00), 0x80),
				_mm_shuffle_ps(m.m3, m.m2, 0xAA)));

			__m128 f6 = _mm_sub_ps(_mm_mul_ps(_mm_shuffle_ps(m.m3, m.m2, 0x00),
				_mm_shuffle_ps(_mm_shuffle_ps(m.m4, m.m3, 0x55), _mm_shuffle_ps(m.m4, m.m3, 0x55), 0x80)),
				_mm_mul_ps(_mm_shuffle_ps(_mm_shuffle_ps(m.m4, m.m3, 0x00), _mm_shuffle_ps(m.m4, m.m3, 0x00), 0x80),
				_mm_shuffle_ps(m.m3, m.m2, 0x55)));

			__m128 v1 = _mm_shuffle_ps(_mm_shuffle_ps(m.m2, m.m1, 0x00), _mm_shuffle_ps(m.m2, m.m1, 0x00), 0xA8);
			__m128 v2 = _mm_shuffle_ps(_mm_shuffle_ps(m.m2, m.m1, 0x55), _mm_shuffle_ps(m.m2, m.m1, 0x55), 0xA8);
			__m128 v3 = _mm_shuffle_ps(_mm_shuffle_ps(m.m2, m.m1, 0xAA), _mm_shuffle_ps(m.m2, m.m1, 0xAA), 0xA8);
			__m128 v4 = _mm_shuffle_ps(_mm_shuffle_ps(m.m2, m.m1, 0xFF), _mm_shuffle_ps(m.m2, m.m1, 0xFF), 0xA8);
			__m128 s1 = _mm_set_ps(-0.0f, 0.0f, -0.0f, 0.0f);
			__m128 s2 = _mm_set_ps(0.0f, -0.0f, 0.0f, -0.0f);
			__m128 i1 = _mm_xor_ps(s1, _mm_add_ps(_mm_sub_ps(_mm_mul_ps(v2, f1),
				_mm_mul_ps(v3, f2)),
				_mm_mul_ps(v4, f3)));
			__m128 i2 = _mm_xor_ps(s2, _mm_add_ps(_mm_sub_ps(_mm_mul_ps(v1, f1),
				_mm_mul_ps(v3, f4)),
				_mm_mul_ps(v4, f5)));
			__m128 i3 = _mm_xor_ps(s1, _mm_add_ps(_mm_sub_ps(_mm_mul_ps(v1, f2),
				_mm_mul_ps(v2, f4)),
				_mm_mul_ps(v4, f6)));
			__m128 i4 = _mm_xor_ps(s2, _mm_add_ps(_mm_sub_ps(_mm_mul_ps(v1, f3),
				_mm_mul_ps(v2, f5)),
				_mm_mul_ps(v3, f6)));
			__m128 d = _mm_mul_ps(m.m1, _mm_movelh_ps(_mm_unpacklo_ps(i1, i2), _mm_unpacklo_ps(i3, i4)));
			d = _mm_add_ps(d, _mm_shuffle_ps(d, d, 0x4E));
			d = _mm_add_ps(d, _mm_shuffle_ps(d, d, 0x11));
			d = _mm_div_ps(_mm_set1_ps(1.0f), d);
			return mat4(float4(_mm_mul_ps(i1, d)),
				float4(_mm_mul_ps(i2, d)),
				float4(_mm_mul_ps(i3, d)),
				float4(_mm_mul_ps(i4, d)));
		}

		/* Quat to matrix */
		friend inline quat mat_to_quat(const mat4& mat)
		{
			float* m = (float*)&mat;
			quat q = quat();

			float tr = m[0] + m[5] + m[10];

			if (tr > 0) 
			{
				float S = sqrtf(tr + 1.0f) * 2.0f;
				q.w = 0.25f * S;
				q.x = (m[9] - m[6]) / S;
				q.y = (m[2] - m[8]) / S;
				q.z = (m[4] - m[1]) / S;
			}
			else if ((m[0] > m[5]) && (m[0] > m[10]))
			{
				float S = sqrtf(1.0f + m[0] - m[5] - m[10]) * 2.0f;
				q.w = (m[9] - m[6]) / S;
				q.x = 0.25f * S;
				q.y = (m[1] + m[4]) / S;
				q.z = (m[2] + m[8]) / S;
			}
			else if (m[5] > m[10]) 
			{
				float S = sqrtf(1.0f + m[5] - m[0] - m[10]) * 2.0f;
				q.w = (m[2] - m[8]) / S;
				q.x = (m[1] + m[4]) / S;
				q.y = 0.25f * S;
				q.z = (m[6] + m[9]) / S;
			}
			else 
			{
				float S = sqrtf(1.0f + m[10] - m[0] - m[5]) * 2.0f;
				q.w = (m[4] - m[1]) / S;
				q.x = (m[2] + m[8]) / S;
				q.y = (m[6] + m[9]) / S;
				q.z = 0.25f * S;
			}

			return q;
		}

		union
		{
			__m128 m[4];

			struct
			{
				__m128 m1;
				__m128 m2;
				__m128 m3;
				__m128 m4;
			};
		};

		ALIGNED_NEW_DELETE("Engine::Math::mat4")
	};

	inline mat4 rotation_mat(const mat4& m)
	{
		float* f = (float*)&m;
		return mat4(f[0], f[1], f[2], 0.0f,
			f[4], f[5], f[6], 0.0f,
			f[8], f[9], f[10], 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
	}

	inline mat4 perspective(float fov, float aspect, float n, float f)
	{
		float _asp = 1.0f / aspect;
		float _fov = 1.0f / tanf(fov * 0.5f);

		return mat4(_fov, 0.0f, 0.0f, 0.0f,
			0.0f, _fov * _asp, 0.0f, 0.0f,
			0.0f, 0.0f, -(f + n) / (f - n), -1.0f,
			0.0f, 0.0f, -(2.0f * f * n) / (f - n), 0.0f);
	}

	inline mat4 ortho(float l, float r, float b, float t, float n, float f)
	{
		return mat4(2.0f / (r - l), 0.0f, 0.0f, 0.0f,
			0.0f, 2.0f / (t - b), 0.0f, 0.0f,
			0.0f, 0.0f, -2.0f / (f - n), 0.0f,
			-(r + l) / (r - l), -(t + b) / (t - b), -(f + n) / (f - n), 1.0f);
	}

	inline mat4 OrthoLH(float width, float height, float zNear, float zFar)
	{
		return mat4(2.0f / width, 0.0f, 0.0f, 0.0f,
			0.0f, 2.0f / height, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f / (zFar - zNear), 0.0f,
			0.0f, 0.0f, zNear / (zNear - zFar), 1.0f);
	}

	inline mat4 lookat(const float4& _o, const float4& _t, const float4& _u)
	{
		const float4 f = normalize(_t - _o);
		const float4 r = normalize(cross(f, _u));
		const float4 u = normalize(cross(r, f));

		float4 pos;
		pos.x = r.x * -_o.x + r.y * -_o.y + r.z * -_o.z;
		pos.y = u.x * -_o.x + u.y * -_o.y + u.z * -_o.z;
		pos.z = -f.x * -_o.x + -f.y * -_o.y + -f.z * -_o.z;

		return mat4(r.x, u.x, -f.x, 0.0f,
			r.y, u.y, -f.y, 0.0f,
			r.z, u.z, -f.z, 0.0f,
			pos.x, pos.y, pos.z, 1.0f);
	}

	inline mat4 LookAtLH(const float4& _o, const float4& _t, const float4& _u)
	{
		float4 zAxis = normalize(_t - _o);
		float4 xAxis = normalize(cross(_u, zAxis));
		float4 yAxis = cross(zAxis, xAxis);

		return mat4(xAxis.x, yAxis.x, zAxis.x, 0.0f,
			xAxis.y, yAxis.y, zAxis.y, 0.0f,
			xAxis.z, yAxis.z, zAxis.z, 0.0f,
			-dot(xAxis, _o), -dot(yAxis, _o), -dot(zAxis, _o), 1.0f);
	}
}

#endif
