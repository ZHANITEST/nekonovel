// ===============================================================
//  nekoVariable - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoVariable_h__
#define __nekoVariable_h__

#include "nekoByteBuffer.h"

namespace neko
{
	enum E_VARIABLE_TYPE
	{
		EVT_STRING = 0,
		EVT_INTEGER,
		EVT_FLOAT,
		EVT_UNKNOWN,
		EVT_COUNT,
	};

	static const char8 *gVariableTypes[EVT_COUNT] = {"문자열", "정수", "실수", "알 수없음"};

	enum E_OPERATOR_TYPE
	{
		EOT_EQUAL = 0,
		EOT_NOTEQUAL,
		EOT_LEFTBIGGER,
		EOT_LEFTEQUALBIGGER,
		EOT_RIGHTBIGGER,
		EOT_RIGHTEQUALBIGGER,
		EOT_INCLUDE,
		EOT_INCLUDENOCASE,
		EOT_ADDEQUAL,
		EOT_SUBTRACTEQUAL,
		EOT_MULTIPLYEQUAL,
		EOT_DIVIDEEQUAL,
		EOT_DIVIDELEFTEQUAL,
		EOT_COUNT
	};

	static const char8 *gOperators[EOT_COUNT] = 
	{"=", "!=", ">", ">=", "<", "<=", "%", "%%", "+=", "-=", "*=", "/=", "%="};

	static E_OPERATOR_TYPE GetOperatorFrom(const char8 *str)
	{
		for(uint32 i = 0;i < EOT_COUNT;++i)
		{
			if(strcmp(str, gOperators[i]) == 0)	
				return (E_OPERATOR_TYPE)i;
		}

		return EOT_COUNT;
	}

	class nekoVariable
	{
	protected:

		E_VARIABLE_TYPE mType;

		string mString;

	public:
		
		/// Constructor
		nekoVariable();
		
		/// Destructor
		virtual ~nekoVariable();
		
		/// Set Value
		void SetValue(const char8 *value);

		inline void SetStringValue(const char8 *strValue)
		{
			mType = EVT_STRING;
			mString = strValue;
		}

		inline const char8 *GetValue() { return mString.c_str(); }
		E_VARIABLE_TYPE GetType() { return mType; }
		inline const char8 *GetTypeString() { return gVariableTypes[mType]; }

		/// Operate value
		result OperateValue(E_OPERATOR_TYPE opType, const char8 *value);

		static bool If(E_OPERATOR_TYPE opType, string rvar1, string rvar2);

		static E_VARIABLE_TYPE GetTypeFrom(const char8 *text);

		void Load(int32 version, nekoByteBuffer &buf);
	};
}


#endif // __nekoVariable_h__