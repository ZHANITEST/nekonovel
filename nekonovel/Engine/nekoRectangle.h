// ===============================================================
//  nekoRectangle - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoRectangle_h__
#define __nekoRectangle_h__

#include "nekoPoint.h"

namespace neko
{
	class nekoRectangle
	{
	public:

		union
		{
			struct
			{
				nekoPoint min;
				nekoPoint max;
			};

			struct 
			{
				int32 left, top;
				int32 right, bottom;
			};
		};

		/// Constructor
		nekoRectangle(int32 left = -1, int32 top = -1, int32 right = 1, int32 bottom = 1)
		{
			this->left = left;
			this->top = top;
			this->right = right;
			this->bottom = bottom;
		}

		/// Get area 
		inline const int32 GetArea() const
		{
			return GetWidth() * GetHeight();
		}

		/// Get Width
		inline const int32 GetWidth() const
		{
			return (right - left);
		}

		/// Get Height
		inline const int32 GetHeight() const
		{
			return (bottom - top);
		}

		/// Get Center point
		inline nekoPoint GetCenter() const
		{
			return nekoPoint((right + left) / 2, (bottom + top) / 2);
		}

		/// Get Size
		inline nekoPoint GetSize() const
		{
			return nekoPoint((right - left), (bottom - top));
		}

		/// Is Intersect?
		bool IsIntersect(const nekoRectangle &other) const
		{
			if( min.x <= other.max.x &&
				max.x >= other.min.x &&
				min.y <= other.max.y &&
				max.y >= other.min.y )
				return true;
			return false;
		}

		/// Is Point in ?
		inline bool IsPointIn(int32 x, int32 y) const
		{
			if(min.x <= x && min.y <= y && max.x >= x && max.y >= y)
				return true;
			return false;
		}

		inline void SetEmpty()
		{
			left = right = top = bottom = 0;
		}

		/// Set size
		inline void SetSize(int32 width, int32 height)
		{
			max.x = min.x + width;
			max.y = min.y + height;
		}
		inline void SetWidth(int32 width) { max.x = min.x + width; }
		inline void SetHeight(int32 height) { max.y = min.y + height; }

		/// Move To
		void MoveTo(int32 x, int32 y)
		{
			int32 width = GetWidth();
			int32 height = GetHeight();
			min.x = x;
			min.y = y;
			max.x = x + width;
			max.y = y + height;
		}

		/// Move Relative.
		void MoveRelative(int32 amountX, int32 amountY)
		{
			min.x += amountX; max.x += amountX;
			min.y += amountY; max.y += amountY;
		}

		/// Scale
		void Scale(float32 x, float32 y)
		{
			nekoPoint center = GetCenter();
			int32 width = GetWidth() * x;
			int32 height = GetHeight() * y;

			min.x = center.x - width / 2;
			max.x = center.x + width / 2;
			min.y = center.y - height / 2;
			max.y = center.y + height / 2;
		}

		/// Move center point to
		void MoveCenterTo(int32 x, int32 y)
		{
			int32 width = GetWidth();
			int32 height = GetHeight();

			min.x = x - width / 2;
			min.y = y - height / 2;
			max.x = x + width / 2;
			max.y = y + height / 2;
		}

		/// Include a box
		inline nekoRectangle &IncludeBox(const nekoRectangle &other)
		{
			min.x = min(other.min.x, min.x);
			min.y = min(other.min.y, min.y);
			max.x = max(other.max.x, max.x);
			max.y = max(other.max.y, max.y);

			return (*this);
		}
	};
}


#endif // __nekoRectangle_h__