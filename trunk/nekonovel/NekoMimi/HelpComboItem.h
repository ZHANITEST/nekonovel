#pragma once

using namespace System;

namespace NekoMimi
{
	public ref class HelpComboItem : public Object
	{
	public:
		String	^mName;
		int32	mValue;

		HelpComboItem(String ^name, int value)
			: mName(name)
			, mValue(value)
		{
		}

		virtual String ^ToString() override
		{
			return mName;
		}

	};
}