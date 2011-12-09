#include "nekoPrecompiledHeader.h"
#include "nekoInteligentParser.h"

namespace neko
{
	/// Constructor
	nekoInteligentParser::nekoInteligentParser()
		: mCurrentTarget(NULL)
	{

	}

	/// Destructor
	nekoInteligentParser::~nekoInteligentParser()
	{

	}

	/// 몇 라인이 삭제되었다.
	void nekoInteligentParser::OnLineRemoved(int32 startLine, int32 endLine)
	{

	}

	/// 해당 라인이 변경되었다.
	void nekoInteligentParser::OnLineEdited(int32 lineNumber, const wchar_t *message)
	{

	}

	/// 새 라인이 입력되었다.
	void nekoInteligentParser::OnNewLineAdded(int32 lineNumber, const wchar_t *message)
	{

	}
}