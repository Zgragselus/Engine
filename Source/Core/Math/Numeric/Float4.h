#ifndef __FLOAT4__H__
#define __FLOAT4__H__

#include <math.h>
#include "Core/Memory/Memory.h"

#define FLOAT4_USE_SSE
#define FLOAT4_USE_SSE2
#define FLOAT4_USE_SSE3
#define FLOAT4_USE_SSE4

#ifdef FLOAT4_USE_SSE
#include <xmmintrin.h>
#endif

#ifdef FLOAT4_USE_SSE2
#include <emmintrin.h>
#endif

#ifdef FLOAT4_USE_SSE3
#include <pmmintrin.h>
#endif

#ifdef FLOAT4_USE_SSE4
#include <smmintrin.h>
#endif

namespace Engine
{
	struct __declspec(align(16)) float4
	{
	#ifdef FLOAT4_USE_SSE
		union
		{
			struct
			{
				float x, y, z, w;
			};
			__m128 xmm;
		};
	#else
		union
		{
			struct
			{
				float x, y, z, w;
			};

			struct
			{
				unsigned int ux, uy, uz, uw;
			};
		}
	#endif


		///////////////////////////////////////////////////////////////////////////////////////////////////
		/** Constructors                                                                                **/

		/* Default constructor */
		inline float4()
		{
	#ifdef FLOAT4_USE_SSE		
			xmm = _mm_setzero_ps();
	#else
			x = y = z = w = 0.0f;
	#endif
		}

		/* Float constructor */
		explicit inline float4(float f)
		{
	#ifdef FLOAT4_USE_SSE		
			xmm = _mm_set1_ps(f);
	#else
			x = y = z = w = f;
	#endif		
		}

		/* Scalar constructor */
		inline float4(float _x, float _y, float _z, float _w)
		{
	#ifdef FLOAT4_USE_SSE		
			xmm = _mm_setr_ps(_x, _y, _z, _w);
	#else
			x = _x;
			y = _y;
			z = _z;
			w = _w;
	#endif		
		}

		/* Array constructor (unaligned!!!) */
		inline float4(const float* fv)
		{
	#ifdef FLOAT4_USE_SSE	
			xmm = _mm_loadu_ps(fv);
	#else
			x = fv[0];
			y = fv[1];
			z = fv[2];
			w = fv[3];
	#endif		
		}

		/* Copy constructor */
		inline float4(const float4 &v)
		{
	#ifdef FLOAT4_USE_SSE	
			xmm = v.xmm;
	#else
			x = v.x;
			y = v.y;
			z = v.z;
			w = v.w;
	#endif			
		}

	#ifdef FLOAT4_USE_SSE	
		/* SSE constructor */
		inline float4(const __m128 &_m)
		{
			xmm = _m;
		}
	#endif

		///////////////////////////////////////////////////////////////////////////////////////////////////
		/** Accessing                                                                                   **/

		/* Direct access write */
		inline float& operator[](int index)
		{
			return reinterpret_cast<float *>(this)[index];
		}

		/* Direct access read */
		inline const float& operator[](int index) const
		{
			return reinterpret_cast<const float *>(this)[index];
		}

		/* Cast */
		inline operator float* ()
		{
			return reinterpret_cast<float *>(this);
		}

		/* Const cast */
		inline operator const float* () const
		{
			return reinterpret_cast<const float *>(this);
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////
		/** Unary arithmetics                                                                           **/

		/* Scalar addition */
		friend inline float4& operator += (float4& v, float f)
		{
	#ifdef FLOAT4_USE_SSE	
			v.xmm = _mm_add_ps(v.xmm, _mm_set1_ps(f));
	#else
			v.x += f;
			v.y += f;
			v.z += f;
			v.w += f;
	#endif
			return v;
		}

		/* Vector addition */
		friend inline float4& operator += (float4& v0, const float4& v1)
		{
	#ifdef FLOAT4_USE_SSE	
			v0.xmm = _mm_add_ps(v0.xmm, v1.xmm);
	#else
			v0.x += v1.x;
			v0.y += v1.y;
			v0.z += v1.z;
			v0.w += v1.w;
	#endif
			return v0;
		}

		/* Scalar subtraction */
		friend inline float4& operator -= (float4& v, float f)
		{
	#ifdef FLOAT4_USE_SSE	
			v.xmm = _mm_sub_ps(v.xmm, _mm_set1_ps(f));
	#else
			v.x -= f;
			v.y -= f;
			v.z -= f;
			v.w -= f;
	#endif
			return v;
		}

		/* Sign negation */
		friend inline const float4 operator - (const float4& v)
		{
	#ifdef FLOAT4_USE_SSE	
			return _mm_xor_ps(v.xmm, _mm_set1_ps(-0.f));
	#else
			float4 ret;
			ret.x = -v.x;
			ret.y = -v.y;
			ret.z = -v.z;
			ret.w = -v.w;
			return ret;
	#endif
		}

		/* Vector subtraction */
		friend inline float4& operator -= (float4& v0, const float4& v1)
		{
	#ifdef FLOAT4_USE_SSE	
			v0.xmm = _mm_sub_ps(v0.xmm, v1.xmm);
	#else
			v0.x -= v1.x;
			v0.y -= v1.y;
			v0.z -= v1.z;
			v0.w -= v1.w;
	#endif
			return v0;
		}

		/* Scalar multiplication */
		friend inline float4& operator *= (float4& v, float f)
		{
	#ifdef FLOAT4_USE_SSE	
			v.xmm = _mm_mul_ps(v.xmm, _mm_set1_ps(f));
	#else
			v.x *= f;
			v.y *= f;
			v.z *= f;
			v.w *= f;
	#endif
			return v;
		}

		/* Vector multiplication */
		friend inline float4& operator *= (float4& v0, const float4& v1)
		{
	#ifdef FLOAT4_USE_SSE	
			v0.xmm = _mm_mul_ps(v0.xmm, v1.xmm);
	#else
			v0.x *= v1.x;
			v0.y *= v1.y;
			v0.z *= v1.z;
			v0.w *= v1.w;
	#endif
			return v0;
		}

		/* Scalar division */
		friend inline float4& operator /= (float4& v, float f)
		{
	#ifdef FLOAT4_USE_SSE	
			v.xmm = _mm_div_ps(v.xmm, _mm_set1_ps(f));
	#else
			float rf = 1.0f / f;
			v.x *= rf;
			v.y *= rf;
			v.z *= rf;
			v.w *= rf;
	#endif
			return v;
		}

		/* Vector division */
		friend inline float4& operator /= (float4& v0, const float4& v1)
		{
	#ifdef FLOAT4_USE_SSE	
			v0.xmm = _mm_div_ps(v0.xmm, v1.xmm);
	#else
			v0.x /= v1.x;
			v0.y /= v1.y;
			v0.z /= v1.z;
			v0.w /= v1.w;
	#endif
			return v0;
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////
		/** Binary arithmetics                                                                          **/

		/* Addition */
		friend inline const float4 operator + (float f, const float4& v)
		{
	#ifdef FLOAT4_USE_SSE	
			return _mm_add_ps(_mm_set1_ps(f), v.xmm);
	#else
			float4 ret;
			ret.x = f + v.x;
			ret.y = f + v.y;
			ret.z = f + v.z;
			ret.w = f + v.w;
			return ret;
	#endif
		}

		friend inline const float4 operator + (const float4& v, float f)
		{
	#ifdef FLOAT4_USE_SSE	
			return _mm_add_ps(v.xmm, _mm_set1_ps(f));
	#else
			float4 ret;
			ret.x = v.x + f;
			ret.y = v.y + f;
			ret.z = v.z + f;
			ret.w = v.w + f;
			return ret;
	#endif
		}

		friend inline const float4 operator + (const float4& v0, const float4& v1)
		{
	#ifdef FLOAT4_USE_SSE	
			return _mm_add_ps(v0.xmm, v1.xmm);
	#else
			float4 ret;
			ret.x = v0.x + v1.x;
			ret.y = v0.y + v1.y;
			ret.z = v0.z + v1.z;
			ret.w = v0.w + v1.w;
			return ret;
	#endif
		}

		/* Subtraction */
		friend inline const float4 operator - (float f, const float4& v)
		{
	#ifdef FLOAT4_USE_SSE	
			return _mm_sub_ps(_mm_set1_ps(f), v.xmm);
	#else
			float4 ret;
			ret.x = f - v.x;
			ret.y = f - v.y;
			ret.z = f - v.z;
			ret.w = f - v.w;
			return ret;
	#endif
		}

		friend inline const float4 operator - (const float4& v, float f)
		{
	#ifdef FLOAT4_USE_SSE	
			return _mm_sub_ps(v.xmm, _mm_set1_ps(f));
	#else
			float4 ret;
			ret.x = v.x - f;
			ret.y = v.y - f;
			ret.z = v.z - f;
			ret.w = v.w - f;
			return ret;
	#endif
		}

		friend inline const float4 operator - (const float4& v0, const float4& v1)
		{
	#ifdef FLOAT4_USE_SSE	
			return _mm_sub_ps(v0.xmm, v1.xmm);
	#else
			float4 ret;
			ret.x = v0.x - v1.x;
			ret.y = v0.y - v1.y;
			ret.z = v0.z - v1.z;
			ret.w = v0.w - v1.w;
			return ret;
	#endif
		}

		/* Multiplication */
		friend inline const float4 operator * (float f, const float4& v)
		{
	#ifdef FLOAT4_USE_SSE	
			return _mm_mul_ps(_mm_set1_ps(f), v.xmm);
	#else
			float4 ret;
			ret.x = f * v.x;
			ret.y = f * v.y;
			ret.z = f * v.z;
			ret.w = f * v.w;
			return ret;
	#endif
		}

		friend inline const float4 operator * (const float4& v, float f)
		{
	#ifdef FLOAT4_USE_SSE	
			return _mm_mul_ps(v.xmm, _mm_set1_ps(f));
	#else
			float4 ret;
			ret.x = v.x * f;
			ret.y = v.y * f;
			ret.z = v.z * f;
			ret.w = v.w * f;
			return ret;
	#endif
		}

		friend inline const float4 operator * (const float4& v0, const float4& v1)
		{
	#ifdef FLOAT4_USE_SSE	
			return _mm_mul_ps(v0.xmm, v1.xmm);
	#else
			float4 ret;
			ret.x = v0.x * v1.x;
			ret.y = v0.y * v1.y;
			ret.z = v0.z * v1.z;
			ret.w = v0.w * v1.w;
			return ret;
	#endif
		}

		/* Division */
		friend inline const float4 operator / (float f, const float4& v)
		{
	#ifdef FLOAT4_USE_SSE	
			return _mm_div_ps(_mm_set1_ps(f), v.xmm);
	#else
			float4 ret;
			ret.x = f / v.x;
			ret.y = f / v.y;
			ret.z = f / v.z;
			ret.w = f / v.w;
			return ret;
	#endif
		}

		friend inline const float4 operator / (const float4& v, float f)
		{
	#ifdef FLOAT4_USE_SSE	
			return _mm_div_ps(v.xmm, _mm_set1_ps(f));
	#else
			float rf = 1.0f / f;
			float4 ret;
			ret.x = v.x * rf;
			ret.y = v.y * rf;
			ret.z = v.z * rf;
			ret.w = v.w * rf;
			return ret;
	#endif
		}

		friend inline const float4 operator / (const float4& v0, const float4& v1)
		{
	#ifdef FLOAT4_USE_SSE	
			return _mm_div_ps(v0.xmm, v1.xmm);
	#else
			float4 ret;
			ret.x = v0.x / v1.x;
			ret.y = v0.y / v1.y;
			ret.z = v0.z / v1.z;
			ret.w = v0.w / v1.w;
			return ret;
	#endif
		}

		friend inline const float4 operator < (const float4& v0, const float4& v1)
		{
#ifdef FLOAT4_USE_SSE
			return _mm_cmplt_ps(v0.xmm, v1.xmm);
#else
			float4 ret;
			ret.x = (v0.x < v1.x) ? 0xffffffff : 0x0;
			ret.y = (v0.y < v1.y) ? 0xffffffff : 0x0;
			ret.z = (v0.z < v1.z) ? 0xffffffff : 0x0;
			ret.w = (v0.w < v1.w) ? 0xffffffff : 0x0;
			return ret;
#endif
		}

		friend inline const float4 operator > (const float4& v0, const float4& v1)
		{
#ifdef FLOAT4_USE_SSE
			return _mm_cmpgt_ps(v0.xmm, v1.xmm);
#else
			float4 ret;
			ret.x = (v0.x > v1.x) ? 0xffffffff : 0x0;
			ret.y = (v0.y > v1.y) ? 0xffffffff : 0x0;
			ret.z = (v0.z > v1.z) ? 0xffffffff : 0x0;
			ret.w = (v0.w > v1.w) ? 0xffffffff : 0x0;
			return ret;
#endif
		}

		friend inline const float4 operator & (const float4& v0, const float4& v1)
		{
#ifdef FLOAT4_USE_SSE
			return _mm_and_ps(v0.xmm, v1.xmm);
#else
			float4 ret;
			ret.ux = v0.ux & v1.ux;
			ret.uy = v0.uy & v1.uy;
			ret.uz = v0.uz & v1.uz;
			ret.uw = v0.uw & v1.uw;
			return ret;
#endif
		}

		friend inline const float4 andnot(const float4& v0, const float4& v1)
		{
#ifdef FLOAT4_USE_SSE
			return _mm_andnot_ps(v0.xmm, v1.xmm);
#else
			float4 ret;
			ret.ux = ~v0.ux & v1.ux;
			ret.uy = ~v0.uy & v1.uy;
			ret.uz = ~v0.uz & v1.uz;
			ret.uw = ~v0.uw & v1.uw;
			return ret;
#endif
		}

		friend inline const float4 mask_lt(const float4& v0, const float4& v1, const float4& value, const float4& compare)
		{
			float4 tmp = value < compare;
			return (tmp & v0) + andnot(tmp, v1);
		}

		friend inline const float4 mask_gt(const float4& v0, const float4& v1, const float4& value, const float4& compare)
		{
			float4 tmp = value > compare;
			return (tmp & v0) + andnot(tmp, v1);
		}

		ALIGNED_NEW_DELETE("Engine::Math::float4")
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	/** Additional arithmetics                                                                      **/

	/* Horizontal add */
	inline float4 hadd(const float4& a)
	{
#ifdef FLOAT4_USE_SSE3
		__m128 b = _mm_hadd_ps(a.xmm, a.xmm);
		return _mm_hadd_ps(b, b);
#elif defined FLOAT4_USE_SSE
		return float4(_mm_add_ss(a.xmm, _mm_add_ss(_mm_shuffle_ps(a.xmm, a.xmm, 1), _mm_add_ss(_mm_shuffle_ps(a.xmm, a.xmm, 2), _mm_shuffle_ps(a.xmm, a.xmm, 3)))));
#else
		float4 ret;
		ret.x = a.x + a.y + a.z + a.w;
		ret.w = ret.z = ret.y = ret.x;
		return ret;
#endif
	}

	/* Absolute value */
	inline const float4 abs(const float4& v)
	{
#ifdef FLOAT4_USE_SSE
		return _mm_andnot_ps(_mm_set1_ps(-0.f), v.xmm);
#else
		float4 ret;
		ret.x = fabsf(v.x);
		ret.y = fabsf(v.y);
		ret.z = fabsf(v.z);
		ret.w = fabsf(v.w);
		return ret;
#endif
	}

	/* Minimum */
	inline const float4 f4min(const float4& v0, const float4& v1)
	{
		return _mm_min_ps(v0.xmm, v1.xmm);
	}

	/* Maximum */
	inline const float4 f4max(const float4& v0, const float4& v1)
	{
		return _mm_max_ps(v0.xmm, v1.xmm);
	}

	inline const float4 f4floor(const float4& v)
	{
		return float4(floor(v.x), floor(v.y), floor(v.z), floor(v.w));
	}

	/* Clamp between values */
	inline const float4 clamp(const float4& v0, float f1, float f2)
	{
#ifdef FLOAT4_USE_SSE
		return _mm_max_ps(_mm_set1_ps(f1), _mm_min_ps(_mm_set1_ps(f2), v0.xmm));
#else
		float4 ret;
		ret.x = max(min(v0.x, f2), f1);
		ret.y = max(min(v0.y, f2), f1);
		ret.z = max(min(v0.z, f2), f1);
		ret.w = max(min(v0.w, f2), f1);
		return ret;
#endif
	}

	/* Clamp between vector values */
	inline const float4 clamp(const float4& v0, const float4& v1, const float4& v2)
	{
#ifdef FLOAT4_USE_SSE
		return _mm_max_ps(v1.xmm, _mm_min_ps(v2.xmm, v0.xmm));
#else
		float4 ret;
		ret.x = max(min(v0.x, v2.x), v1.x);
		ret.y = max(min(v0.y, v2.y), v1.y);
		ret.z = max(min(v0.z, v2.z), v1.z);
		ret.w = max(min(v0.w, v2.w), v1.w);
		return ret;
#endif
	}

	/* Distance between two vectors */
	inline float distance(const float4& v0, const float4& v1)
	{
#ifdef FLOAT4_USE_SSE4
		__m128 l = _mm_sub_ps(v0.xmm, v1.xmm);
		return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(l, l, 0xff)));
#elif defined FLOAT4_USE_SSE3
		__m128 l = _mm_sub_ps(v0.xmm, v1.xmm);
		l = _mm_mul_ps(l, l);
		l = _mm_hadd_ps(l, l);
		return _mm_cvtss_f32(_mm_sqrt_ss(_mm_hadd_ps(l, l)));
#elif defined FLOAT4_USE_SSE
		__m128 l = _mm_sub_ps(v0.xmm, v1.xmm);
		l = _mm_mul_ps(l, l);
		l = _mm_add_ps(l, _mm_shuffle_ps(l, l, 0x4E));
		return _mm_cvtss_f32(_mm_sqrt_ss(_mm_add_ss(l, _mm_shuffle_ps(l, l, 0x11))));
#else	
		float4 v = v0 - v1;
		return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
#endif
	}

	/* Dot product */
	inline float dot(const float4 &v0, const float4 &v1)
	{
#ifdef FLOAT4_USE_SSE4
		return _mm_cvtss_f32(_mm_dp_ps(v0.xmm, v1.xmm, 0xff));
#elif defined FLOAT4_USE_SSE3
		__m128 l = _mm_sub_ps(v0.xmm, v1.xmm);
		l = _mm_mul_ps(l, l);
		l = _mm_hadd_ps(l, l);
		return _mm_cvtss_f32(_mm_hadd_ps(l, l));
#elif defined FLOAT4_USE_SSE
		__m128 l = _mm_mul_ps(v0.xmm, v1.xmm);
		l = _mm_add_ps(l, _mm_shuffle_ps(l, l, 0x4E));
		return _mm_cvtss_f32(_mm_add_ss(l, _mm_shuffle_ps(l, l, 0x11)));
#else
		return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z + v0.w * v1.w;
#endif
	}

	/* Cross product */
	inline float4 cross(const float4& v0, const float4& v1)
	{
#ifdef FLOAT4_USE_SSE
		const __m128 l = _mm_mul_ps(_mm_shuffle_ps(v0.xmm, v0.xmm, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(v1.xmm, v1.xmm, _MM_SHUFFLE(3, 1, 0, 2)));
		const __m128 r = _mm_mul_ps(_mm_shuffle_ps(v0.xmm, v0.xmm, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(v1.xmm, v1.xmm, _MM_SHUFFLE(3, 0, 2, 1)));
		return _mm_sub_ps(l, r);
#else
		float4 ret;
		ret.x = v0.y * v1.z - v0.z * v1.y;
		ret.y = v0.z * v1.x - v0.x * v1.z;
		ret.z = v0.x * v1.y - v0.y * v1.x;
		ret.w = 0.0f;
		return ret;
#endif
	}

	/* Squared Length of vector */
	inline float lengthsq(const float4& v)
	{
#ifdef FLOAT4_USE_SSE4
		return _mm_cvtss_f32(_mm_dp_ps(v.xmm, v.xmm, 0xff));
#elif defined FLOAT4_USE_SSE3
		__m128 l = _mm_mul_ps(v.xmm, v.xmm);
		l = _mm_hadd_ps(l, l);
		return _mm_cvtss_f32(_mm_hadd_ps(l, l));
#elif defined FLOAT4_USE_SSE
		__m128 l = _mm_mul_ps(v.xmm, v.xmm);
		l = _mm_add_ps(l, _mm_shuffle_ps(l, l, 0x4E));
		return _mm_cvtss_f32(_mm_add_ss(l, _mm_shuffle_ps(l, l, 0x11)));
#else	
		return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
#endif
	}

	/* Length of vector */
	inline float length(const float4& v)
	{
#ifdef FLOAT4_USE_SSE4
		return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(v.xmm, v.xmm, 0xff)));
#elif defined FLOAT4_USE_SSE3
		__m128 l = _mm_mul_ps(v.xmm, v.xmm);
		l = _mm_hadd_ps(l, l);
		return _mm_cvtss_f32(_mm_sqrt_ss(_mm_hadd_ps(l, l)));
#elif defined FLOAT4_USE_SSE
		__m128 l = _mm_mul_ps(v.xmm, v.xmm);
		l = _mm_add_ps(l, _mm_shuffle_ps(l, l, 0x4E));
		return _mm_cvtss_f32(_mm_sqrt_ss(_mm_add_ss(l, _mm_shuffle_ps(l, l, 0x11))));
#else	
		return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
#endif
	}

	/* Normalization */
	inline const float4 normalize(const float4& v)
	{
#ifdef FLOAT4_USE_SSE4
		return _mm_div_ps(v.xmm, _mm_sqrt_ps(_mm_dp_ps(v.xmm, v.xmm, 0xff)));
#elif defined FLOAT4_USE_SSE3
		__m128 l = _mm_mul_ps(v.xmm, v.xmm);
		l = _mm_hadd_ps(l, l);
		return _mm_div_ps(v.xmm, _mm_sqrt_ps(_mm_hadd_ps(l, l)));
#elif defined FLOAT4_USE_SSE
		__m128 l = _mm_mul_ps(v.xmm, v.xmm);
		l = _mm_add_ps(l, _mm_shuffle_ps(l, l, 0x4E));
		return _mm_mul_ps(v.xmm, _mm_rsqrt_ps(_mm_add_ps(l, _mm_shuffle_ps(l, l, 0x11))));
#else	
		float l = 1.0f / sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
		float4 ret;
		ret.x = v.x * l;
		ret.y = v.y * l;
		ret.z = v.z * l;
		ret.w = v.w * l;
		return ret;
#endif
	}

	/* Linear interpolation */
	inline float4 mix(const float4& a, const float4& b, float l)
	{
#ifdef FLOAT4_USE_SSE
		return _mm_add_ps(_mm_mul_ps(_mm_sub_ps(b.xmm, a.xmm), _mm_set1_ps(l)), a.xmm);
#else
		float4 ret;
		ret.x = a.x + (b.x - a.x) * l;
		ret.x = a.y + (b.y - a.y) * l;
		ret.x = a.z + (b.z - a.z) * l;
		ret.x = a.w + (b.w - a.w) * l;
		return ret;
#endif
	}
}

#endif
