// ===============================================================
//  nekoPoint - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoPoint_h__
#define __nekoPoint_h__



namespace neko
{
	template <class T>
	class nekoPointBase
	{
	public:

		/// Point variable
		T x, y;

		/// Constructor
		nekoPointBase(T x = 0, T y = 0)
		{
			this->x = x;
			this->y = y;
		}

		/// Multiply Operator
		nekoPointBase<T> operator *(T scalar)
		{
			nekoPointBase<T> newPoint;

			newPoint.x = x * scalar;
			newPoint.y = y * scalar;

			return newPoint;
		}

		/// Multiply Operator
		nekoPointBase<T> operator *(nekoPointBase<T> &other)
		{
			nekoPointBase<T> newPoint;

			newPoint.x = x * other.x;
			newPoint.y = y * other.y;

			return newPoint;
		}

		/// Multiply-Substitute Operator
		nekoPointBase<T> &operator *=(T scalar)
		{
			x *= scalar;
			y *= scalar;

			return (*this);
		}

		/// Multiply-Substitute Operator
		nekoPointBase<T> &operator *=(nekoPointBase<T> &other)
		{
			x *= other.x;
			y *= other.y;

			return (*this);
		}

		/// Multiply-Substitute Operator
		nekoPointBase<T> &operator +=(T scalar)
		{
			x += scalar;
			y += scalar;

			return (*this);
		}

		/// Add Operator
		nekoPointBase<T> operator +(T scalar)
		{
			nekoPointBase<T> newPoint;

			newPoint.x = x + scalar;
			newPoint.y = y + scalar;

			return newPoint;
		}

		/// Add Operator
		nekoPointBase<T> operator +(nekoPointBase<T> &other)
		{
			nekoPointBase<T> newPoint;

			newPoint.x = x + other.x;
			newPoint.y = y + other.y;

			return newPoint;
		}

		/// Add-Substitute Operator
		nekoPointBase<T> &operator +=(nekoPointBase<T> &other)
		{
			x += other.x;
			y += other.y;

			return (*this);
		}

		/// Subtract Operator
		nekoPointBase<T> operator -(T scalar)
		{
			nekoPointBase<T> newPoint;

			newPoint.x = x - scalar;
			newPoint.y = y - scalar;

			return newPoint;
		}

		/// Subtract Operator
		nekoPointBase<T> operator -(nekoPointBase<T> &other)
		{
			nekoPointBase<T> newPoint;

			newPoint.x = x - other.x;
			newPoint.y = y - other.y;

			return newPoint;
		}

		/// Subtract-Substitute Operator
		nekoPointBase<T> &operator -=(nekoPointBase<T> &other)
		{
			x -= other.x;
			y -= other.y;

			return (*this);
		}

		/// Divide Operator
		nekoPointBase<T> operator /(T scalar)
		{
			nekoPointBase<T> newPoint;

			newPoint.x = x / scalar;
			newPoint.y = y / scalar;

			return newPoint;
		}

		/// Divide-Substitute Operator
		nekoPointBase<T> &operator /=(T scalar)
		{
			x /= scalar;
			y /= scalar;

			return (*this);
		}

		/// Divide-Substitute Operator
		nekoPointBase<T> &operator /=(nekoPointBase<T> &other)
		{
			x /= other.x;
			y /= other.y;

			return (*this);
		}

		/// Divide Operator
		nekoPointBase<T> operator /(nekoPointBase<T> &other)
		{
			nekoPointBase<T> newPoint;

			newPoint.x = x / other.x;
			newPoint.y = y / other.y;

			return newPoint;
		}

		/// Get Length
		float32 GetLength()
		{
			return sqrt((float32)x * x + y * y);
		}

		/// Normalize the vector
		void Normalize()
		{
			float64 length = (float64)GetLength();
			if(!length)
				return;
			length = 1.0f / length;

			x *= length;
			y *= length;
		}

		/// Is Same ?
		bool operator ==(nekoPointBase<T> &point)
		{
			if(IsFloatZero(x - point.x) && IsFloatZero(y - point.y))
				return true;
			return false;
		}

		/// Is Not Same ?
		bool operator !=(nekoPointBase<T> &point)
		{
			if(IsFloatZero(x - point.x) && IsFloatZero(y - point.y))
				return false;
			return true;
		}

		inline int32 GetArea() const
		{
			return x * y;
		}
	};

	// Define types.
	typedef nekoPointBase<int32> nekoPoint;
	typedef nekoPointBase<float32> nekoPointF;
}


#endif // __nekoPoint_h__