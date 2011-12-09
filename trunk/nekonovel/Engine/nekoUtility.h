// ===============================================================
//  nekoUtility - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoUtility_h__
#define __nekoUtility_h__



namespace neko
{
	class nekoUtility
	{
	protected:


	public:

		/// Get File name
		static const char8 *GetFileName(const char8 *p);

		/// Read string from file pointer
		static const string &ReadString(FILE *fp, uint32 maxSize = -1);

		/// Write string to file pointer
		static void WriteString(FILE *fp, const char *str);

		static const char8 *FormString(const char8 *fmt, ...);

		/// Is Hangul Character ?
		static inline bool IsMultibyteCode(char8 ch)
		{
			return ((ch & 0x80) > 0);
		}

		/// RGBA
		static inline uint32 Color(int32 r, int32 g, int32 b, int32 a = 255)
		{
			return (((a & 0xff) << 24) | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff));
		}

		/// Is Number ?
		static bool IsNumber(const char8 *txt);

		static uint32 TextToColor(const char8 *txt);

		static void TrimRight(string &strInOut);
		
	};
}


#endif // __nekoUtility_h__