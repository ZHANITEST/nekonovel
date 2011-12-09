#include "nekoPrecompiledHeader.h"
#include "nekoLabel.h"
#include "NekoNovel.h"
#include "nekoHWResourceManager.h"
#include "nekoLogger.h"

namespace neko
{
	nekoLabelInstance::nekoLabelInstance(nekoIBlock *block)
		: nekoIBlockInstance(block)
		, mSpecialPurpose(false)
		, mAlign(ELAT_LEFT)
	{
		mZOrder = 0;
	}

	nekoLabelInstance::~nekoLabelInstance()
	{

	}

	void nekoLabelInstance::SetCaption(const string &caption, uint32 color)
	{
		mCaption = caption;
		mColor = color;

		// 특수용도 라벨
		if(strcmp(mName.c_str(), "%메세지라벨") == 0)
		{
			mAlign = ELAT_CENTER;
		}
	}

	void nekoLabelInstance::SetFont(const string &fontName, int32 size, int32 bold)
	{
		bool dfbold = (mFont) ? mFont->IsBold() : true;
		int32 dfsize = (mFont) ? mFont->GetHeight() : 15;

		if(fontName == "없음")
		{
			mFont = 0;
			return;
		}

		mFont = GetNekoNovel()->GetHWResourceManager()->LoadFont(fontName.c_str(), 0,
			(size < 0) ? dfsize : size,
			((bold < 0) ? dfbold : (bold == 1)));
	}

	void nekoLabelInstance::Update(float32 frameTime)
	{
		
	}

	void nekoLabelInstance::Draw()
	{
		if(!mVisible || !mAlpha || mCaption.size() == 0)
			return;

		nekoVideoDriver *videoDrv = GetNekoNovel()->GetVideoDriver();

		const char8 *text = GetNekoNovel()->GetCurrentGame()->ParseString(mCaption.c_str()).c_str();
		if(mAlign == ELAT_CENTER)
		{
			nekoRectangle newRect;
			videoDrv->CalculateText(mFont, text, newRect);
			newRect.MoveCenterTo(mBoundingBox.left, mBoundingBox.top);

			videoDrv->DrawText(mFont, newRect.left, newRect.top, newRect.GetWidth(), newRect.GetHeight(),
				text, MixColorsAlpha(mColor, mAlpha), DT_NOCLIP|DT_CENTER, _GetRealZ());
		}
		else if(mAlign == ELAT_RIGHT)
		{
			nekoRectangle newRect;
			videoDrv->CalculateText(mFont, text, newRect);
			newRect.MoveTo(mBoundingBox.left - newRect.GetWidth(), mBoundingBox.top);

			videoDrv->DrawText(mFont, newRect.left, newRect.top, newRect.GetWidth(), newRect.GetHeight(),
				text, MixColorsAlpha(mColor, mAlpha), DT_NOCLIP|DT_RIGHT, _GetRealZ());
		}
		else
		{
			videoDrv->DrawText(mFont, mBoundingBox.left, mBoundingBox.top, 1, 1,
				text, MixColorsAlpha(mColor, mAlpha), DT_NOCLIP, _GetRealZ());
		}
	}

	/// 데이터 덤프하기
	result nekoLabelInstance::Dump(int32 version, nekoByteBuffer &outBuf)
	{
		nekoIBlockInstance::Dump(version, outBuf);

		outBuf << mCaption << mColor
		 << (mFont != 0);

		if(mFont)
		{
			outBuf << mFont->GetName()
				<< mFont->GetHeight()
				<< mFont->IsBold();
		}

		return S_OK;
	}

	result nekoLabelInstance::Load(int32 version, nekoByteBuffer &inBuf)
	{
		nekoIBlockInstance::Load(version, inBuf);

		inBuf >> mCaption >> mColor;
		if(version >= 1108)
		{
			if(inBuf.Read<bool>())
			{
				string fontName;
				int32 height;
				bool bold;

				inBuf >> fontName
					>> height
					>> bold;

				SetFont(fontName, height, (bold) ? 1 : 0);
			}
		}

		return S_OK;
	}

	/// Constructor
	nekoLabel::nekoLabel()
		: nekoIBlock(BLOCK_LABEL)
	{

	}

	/// Destructor
	nekoLabel::~nekoLabel()
	{

	}

	/// 인스턴스를 만든다
	nekoIBlockInstance *nekoLabel::CreateInstance()
	{
		return nekoNew nekoLabelInstance(this);
	}
}