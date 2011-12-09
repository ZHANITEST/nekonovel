#include "nekoPrecompiledHeader.h"
#include "nekoLogger.h"
#include "NekoNovel.h"
#include <time.h>

namespace neko
{
	/// Message buffer
	string nekoLogger::fileName;
	int32 nekoLogger::lineNumber;
	FILE *nekoLogger::pFile = 0;

	/// Print log.
	bool nekoLogger::Log(E_LOGLEVEL_TYPE logLevel, const char8 *pMessage, ...)
	{
		char8 messageBuf[1024];
		char8 *p = messageBuf;

#ifndef _DEBUG
		if(logLevel == LOG_LEVEL_DINFO)
			return true;
#endif

#ifndef _DEBUG
		// 릴리즈 상태일 때에는
		// 정보 로그를 표시하지 않는다.
		//if(logLevel == LOG_LEVEL_INFO)
		//	return true;
#endif

		/// Get current time
		struct tm *t;
		time_t timer;
		timer = time(0);
		t = localtime(&timer);

		/// Print on a screen.
		sprintf(p, "[%s][%02d:%02d:%02d]", gLogLevelStr[logLevel], t->tm_hour, t->tm_min, t->tm_sec);
		p += strlen(p);

		// Put log string.
		va_list vlist;
		va_start(vlist, pMessage);
		vsprintf(p, pMessage, vlist);
		va_end(vlist);


		// Log output.
		if(pFile)
		{
			fprintf(pFile, "%s\n", messageBuf);
			fflush(pFile);
		}

		// If level is error(fatal).
		if(logLevel == LOG_LEVEL_ERRO)
		{
			//nekoErrorException exp(string(messageBuf), fileName, lineNumber);
			//throw exp;

			// Change the line number to string.
			char lineBuf[10];
			sprintf(lineBuf, "%d", lineNumber);

			// If error has broken out.
			string msg =  "게임을 이용하는데 불편을 드려 죄송합니다.\n"
				"다음의 오류가 프로그램을 실행하는 도중에 발생하였습니다.\n\n";
			msg += "--- Fatal Error Exception ------------\n";
			msg += "File - " + fileName + " (" + lineBuf + ")\n";
			msg += "Message - "; msg += messageBuf;

			MessageBox(0, msg.c_str(), NEKO_WINDOWNAME, MB_ICONEXCLAMATION);
		}

		nekoEvent e(EVENT_LOG);
		e.SetText(0, messageBuf);
		e.value[1] = (int32)logLevel;
		GetNekoNovel()->OnEvent(e);

		return true;
	}
}