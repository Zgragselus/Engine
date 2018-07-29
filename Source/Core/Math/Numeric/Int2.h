#ifndef __INT2__H__
#define __INT2__H__

namespace Engine
{
	struct int2
	{
		inline int2()
		{
			this->x = this->y = 0;
		}

		inline int2(int x, int y)
		{
			this->x = x;
			this->y = y;
		}

		inline int2(const int2& v)
		{
			this->x = v.x;
			this->y = v.y;
		}

		friend inline int2 operator/(const int2& a, int b)
		{
			return int2(a.x / b, a.y / b);
		}

		friend inline int2 operator-(const int2& a, const int2& b)
		{
			return int2(a.x - b.x, a.y - b.y);
		}

		inline int& operator[](int index)
		{
			return reinterpret_cast<int *>(this)[index];
		}

		inline const int& operator[](int index) const
		{
			return reinterpret_cast<const int *>(this)[index];
		}

		int x;
		int y;
	};
}

#endif