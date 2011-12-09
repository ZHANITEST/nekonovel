#pragma once

namespace NekoMimi
{
	public ref class ScriptFile
	{
	public:

		String ^sourceFile;
		int32 selectionStart;
		int32 selectionLength;
		int32 vScroll;
		int32 hScroll;
		bool changed;
		nekoCommands *commands;

		ScriptFile()
			: selectionStart(0)
			, selectionLength(0)
			, vScroll(0)
			, hScroll(0)
			, changed(false)
		{

		}

	};
}