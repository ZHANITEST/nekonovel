// ===============================================================
//  nekoLogger - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoLogger_h__
#define __nekoLogger_h__

#include <stdio.h>
#include <string>

using namespace std;

namespace neko
{
	/// Log Level
	enum E_LOGLEVEL_TYPE
	{
		LOG_LEVEL_ERRO = 0,
		LOG_LEVEL_INFO,
		LOG_LEVEL_DINFO,
		LOG_LEVEL_WARN,
		LOG_LEVEL_COUNT
	};

	/// Log level strings
	static const char8 *gLogLevelStr[LOG_LEVEL_COUNT] =
	{
		"오류",
		"정보",
		"개발정보",
		"경고",
	};

	class nekoLogger
	{
	protected:

		/// File Name
		static string fileName;

		/// Line number
		static int32 lineNumber;

		/// File Pointer
		static FILE *pFile;

	public:

		/// Set log information.
		static bool SetInformation(char8 *pFileName, int32 line)
		{
			fileName = pFileName;
			lineNumber = line;
			return true;
		}

		/// Set Output
		static void SetOutput(FILE *pFilePointer)
		{
			if(pFile)
				fclose(pFile);
			pFile = pFilePointer;
		}

		/// Close the Output
		static void CloseOutput()
		{
			if(pFile)
				fclose(pFile);
		}

		/// Print log.
		static bool Log(E_LOGLEVEL_TYPE logLevel, const char8 *pMessage, ...);
	};

#ifdef _DEBUG
#	define DevLogInfo(msg) lesLogger::Log(LOG_LEVEL_DINFO, msg)
#else
#	define DevLogInfo(msg) 
#endif
}


#endif // __nekoLogger_h__