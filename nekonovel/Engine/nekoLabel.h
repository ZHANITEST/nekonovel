// ===============================================================
//  nekoLabel - 2009.8.10
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoLabel_h__
#define __nekoLabel_h__

#include "nekoIBlock.h"
#include "nekoIBlockInstance.h"
#include "nekoHWFont.h"

namespace neko
{
	enum E_LABEL_ALIGN_TYPE
	{
		ELAT_LEFT = 0,
		ELAT_CENTER,
		ELAT_RIGHT,
	};

	class nekoLabelInstance : public nekoIBlockInstance
	{
	protected:

		string mCaption; // 변수 이름이 포함되어있는..
		uint32 mColor;
		nekoAutoPtr<nekoHWFont> mFont;
		bool mSpecialPurpose;
		E_LABEL_ALIGN_TYPE mAlign;

	public:

		nekoLabelInstance(nekoIBlock *block);
		virtual ~nekoLabelInstance();

		void SetCaption(const string &caption, uint32 color);
		void SetFont(const string &fontName, int32 size, int32 bold);

		void Update(float32 frameTime);
		void Draw();

		/// 데이터 덤프하기
		result Dump(int32 version, nekoByteBuffer &outBuf);
		result Load(int32 version, nekoByteBuffer &inBuf);

		bool IsIncludedInSnapShot() { return false; }
		bool IsZeroAlphaDeleting() const { return true; }
	};

	class nekoLabel : public nekoIBlock
	{
	protected:

	public:

		nekoLabel();
		virtual ~nekoLabel();

		/// 인스턴스를 만든다
		virtual nekoIBlockInstance *CreateInstance();

	};
}


#endif // __nekoLabel_h__