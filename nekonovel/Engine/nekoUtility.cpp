#include "nekoPrecompiledHeader.h"
#include "nekoUtility.h"

namespace neko
{
	/// Get File name
	const char8 *nekoUtility::GetFileName(const char8 *p)
	{
		p += strlen(p);
		while(*p != '\\' && *p != '/')
			--p;

		return p + 1;
	}

	/// Read string from file pointer
	const string &nekoUtility::ReadString(FILE *fp, uint32 maxSize)
	{
		static string result;
		uint32 length;
		char8 *tmp;

		result.clear();

		// Read length.
		fread(&length, 1, sizeof(uint32), fp);
		if(!length)
			return result;

		if(length > maxSize)
			return result;

		// Allocate string
		tmp = nekoNew char8[length + 1];
		tmp[length] = '\0';
		fread(tmp, 1, length, fp);
		result = tmp;
		delete []tmp;

		return result;
	}

	/// Write string to file pointer
	void nekoUtility::WriteString(FILE *fp, const char *str)
	{
		int32 length = strlen(str);
		fwrite(&length, 1, sizeof(uint32), fp);
		fwrite(str, 1, length, fp);
	}

	/// Form formatted String
	const char8 *nekoUtility::FormString(const char8 *fmt, ...)
	{
		static char8 tempBuf[1024];

		/// Construct a log message.
		va_list arg;
		va_start(arg, fmt);
		vsprintf(tempBuf, fmt, arg);
		va_end(arg);

		return tempBuf;
	}

	/// Is Number ?
	bool nekoUtility::IsNumber(const char8 *txt)
	{
		if(*txt == '+' || *txt == '-')
			++txt;

		while(*txt != '\0')
		{
			if(!(*txt >= '0' && *txt <= '9'))
				return false;
		}

		return true;
	}

	uint32 nekoUtility::TextToColor(const char8 *txt)
	{
		if(!txt)
			return 0xff000000;
		uint32 color = 0xff000000;
		
		color |= ((atoi(txt) & 0xff) << 16);

		while(*txt != '\0' && *txt != ',') ++txt;
		if(*txt == ',') ++txt;
		color |= ((atoi(txt) & 0xff) << 8);

		while(*txt != '\0' && *txt != ',') ++txt;
		if(*txt == ',') ++txt;
		color |= (atoi(txt) & 0xff);

		return color;
	}

	void nekoUtility::TrimRight(string &strInOut)
	{
		if(strInOut.empty())
			return;

		int32 spaceCount = 0;

		const char8 *p = strInOut.c_str() + strInOut.length() - 1;
		while(*p-- == ' ') { ++spaceCount; }

		if(!spaceCount)
			return;

		strInOut = strInOut.substr(0, strInOut.length() - spaceCount);
	}
}