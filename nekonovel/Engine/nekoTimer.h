// ===============================================================
//  nekoTimer - 2009.7.9
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoTimer_h__
#define __nekoTimer_h__



namespace neko
{
	class nekoTimer
	{
	public:

		/// Constructor
		nekoTimer(float32 time = -1)
			: mTime(time)
		{
			this->mCurrentTime = 0;
		}

		/// Destructor
		~nekoTimer()
		{

		}

		/// Set time
		inline void SetTime(float32 time)
		{
			this->mTime = time;
		}

		/// Get timer time
		inline float32 GetTimerTime() const
		{
			return mTime;
		}

		/// Get reverse time
		inline float32 GetReverseTime() const
		{
			return (mTime - mCurrentTime);
		}

		/// Update
		inline void Update(float32 frameTime)
		{
			mCurrentTime += frameTime;
		}

		/// Get Current Time
		inline float32 GetCurrentTime() const
		{
			return mCurrentTime;
		}

		/// Is Completed ?
		inline bool IsCompleted()
		{
			if(mTime >= 0 &&
				mCurrentTime >= mTime)
			{
				mCurrentTime = 0;
				return true;
			}

			return false;
		}

		/// Clear time
		inline void Clear()
		{
			mCurrentTime = 0;
		}

	protected:

		/// Time to end.
		float32 mTime;

		/// Current time
		float32 mCurrentTime;
	};
}


#endif // __nekoTimer_h__