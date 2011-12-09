#include "nekoPrecompiledHeader.h"
#include "nekoMessageLog.h"
#include "nekoNovelReal.h"

namespace neko
{
	bool nekoMessageLogInstance::_OnMouseMove(WPARAM wParam, int32 x, int32 y)
	{
		if(mPickedType == EMPT_NOTHING)
			return false;

		if(mPickedType == EMPT_SCROLLBAR
			&& (wParam)
			&& mMaxValue > 0)
		{
			mValue = (float32)((y - mScrollTemp) - (mBoundingBox.top + 10 + mScrollBarHeight)) 
				/ (mScrollSpaceSize - mScrollBarSize) * mMaxValue;
			mValue = min(max(mValue, 0), mMaxValue);

			_OnValueUpdated();
			return true;
		}

		return false;
	}
	E_MESSAGELOG_PICKED_TYPE nekoMessageLogInstance::_CheckPickedType(int32 mx, int32 my)
	{
		int32 x = mBoundingBox.right - 5 - ScrollBarButtonWidth;
		int32 y = 10 + mBoundingBox.top;
		if(nekoRectangle(x, y, x + ScrollBarButtonWidth, y + mScrollBarHeight).IsPointIn(mx, my))
			return EMPT_UPBUTTON;

		y += mScrollBarHeight;

		if(mMaxValue > 0 && nekoRectangle(
			x
			, y + (float32)mValue / mMaxValue * (mScrollSpaceSize - mScrollBarSize)
			, x + ScrollBarButtonWidth
			, y + (float32)mValue / mMaxValue * (mScrollSpaceSize - mScrollBarSize) + mScrollBarSize).IsPointIn(mx, my))
			return EMPT_SCROLLBAR;

		if(nekoRectangle(x, y, x + ScrollBarButtonWidth, y + mScrollSpaceSize).IsPointIn(mx, my))
			return EMPT_SCROLLSPACE;

		y += mBoundingBox.GetHeight() - (mScrollBarHeight * 2 + 20);
		if(nekoRectangle(x, y, x + ScrollBarButtonWidth, y + mScrollBarHeight).IsPointIn(mx, my))
			return EMPT_DOWNBUTTON;

		// 보이스 선택하기!
		if(!mValueUpdated)
		{
			// 스크롤 컨버스 좌표로 변환하기
			mx -= mBoundingBox.left;
			my -= mBoundingBox.top + 10;
			list<nekoMessageLogBlock> &lstBlock = mMessageLogger->GetBlocks();
			list<nekoMessageLogBlock>::iterator iter = mIterFirstBlock;
			int32 i = 0;
			for(;iter != lstBlock.end();++iter,++i)
			{
				if((*iter).mBoundingBox.top - mValue > _GetOnePageSize())
					break;

				if(nekoRectangle(0, (*iter).mBoundingBox.top - mValue, 40, (*iter).mBoundingBox.top - mValue + 23).IsPointIn(mx, my)
					&& !(*iter).mVoiceName.empty())
				{
					mVoicedPicked = iter;
					return EMPT_VOICEBUTTON;
				}
			}

			mVoicedPicked = lstBlock.end();
		}

		return EMPT_NOTHING;
	}

	nekoMessageLogInstance::nekoMessageLogInstance(nekoIBlock *block)
		: nekoIBlockInstance(block)
		, mValue(0)
		, mMaxValue(0)
		, mScrollBarSize(0)
		, mPickedType(EMPT_NOTHING)
		, mMousePressed(false)
		, mScrollTemp(0)
		, mCharHeight(0)
		, mAutoScrollTimer(0.6f)
		, mValueUpdated(true)
		, mInfoUpdated(false)
		, mScrollBarLimitedSize(0)
	{
		mMessageLogger = GetNekoNovel()->GetCurrentGame()->GetMessageLogger();
	}

	nekoMessageLogInstance::~nekoMessageLogInstance()
	{

	}

	void nekoMessageLogInstance::SetScrollBar(int32 scrollBarLimitedSize, int32 scrollBarHeight)
	{
		mScrollBarHeight = scrollBarHeight;
		mScrollBarLimitedSize = max(scrollBarLimitedSize, 0);
	}
	
	void nekoMessageLogInstance::Initialize(const char8 *skinFileName)
	{
		mBoundingBox.SetSize(mMessageLogger->GetSize().x, mMessageLogger->GetSize().y);

		mSkinImage = GetNekoNovel()->GetHWResourceManager()->LoadTexture((string("이미지\\") + skinFileName).c_str());
		mScrollSpaceSize = mBoundingBox.GetHeight() - (mScrollBarHeight * 2 + 20);

		mInfoUpdated = false;

	}

	void nekoMessageLogInstance::Update(float32 frameTime)
	{
		// 첫 설정!!
		if(!mInfoUpdated)
		{
			if(mMessageLogger->IsUpdateNeeded() == true)
				return;

			mMaxValue = mMessageLogger->GetTotalHeight() - _GetOnePageSize();
			if(mMaxValue > 0)
			{
				if(mScrollBarLimitedSize > 0)
					mScrollBarSize = mScrollBarLimitedSize;
				else
					mScrollBarSize = max(mScrollSpaceSize * _GetOnePageSize() / mMessageLogger->GetTotalHeight(), 16);
			}

			mCharHeight = mMessageLogger->GetCharHeight();
			mInfoUpdated = true;
			mValue = mMaxValue;
		}

		if(mMousePressed)
		{
			mAutoScrollTimer.Update(frameTime);
			if(mAutoScrollTimer.IsCompleted())
			{
				if(mPickedType == EMPT_UPBUTTON)
					mValue -= 20;
				else if(mPickedType == EMPT_DOWNBUTTON)
					mValue += 20;
				else
					return;

				mAutoScrollTimer.SetTime(0.05f);
				mValue = min(max(mValue, 0), mMaxValue);
				_OnValueUpdated();
			}
		}
	}

	void nekoMessageLogInstance::Draw()
	{
		if(!mInfoUpdated)
			return;

		nekoVideoDriver *videoDrv = GetNekoNovel()->GetVideoDriver();

		if(mFlags & EBIF_IGNORE_CAMERA)
			videoDrv->EnableCamera(false);

		list<nekoMessageLogBlock> &lstBlock = mMessageLogger->GetBlocks();
		list<nekoMessageLogBlock>::iterator iter = lstBlock.begin();

		// 값 업데이트가 안되었다면..
		if(mValueUpdated == false)
		{
			iter = mIterFirstBlock;
		}

		int32 x, y;
		int32 tempU;
		int32 tempD;
		int32 tempLD;
		int32 tempID;
		int32 ot;
		
		int32 realHeight = mBoundingBox.GetHeight() - 20;
		
		x = mBoundingBox.left;
		for(;iter != lstBlock.end();++iter)
		{
			uint32 flags = 0;

			y = (*iter).mBoundingBox.top - mValue;

			if(y + (*iter).mBoundingBox.GetHeight() < 0)
				continue;

			if(y > realHeight)
				break;

			// 값 업데이트가 되었다면!
			if(mValueUpdated)
			{
				mIterFirstBlock = iter;
				mValueUpdated = false;
			}

			// 위로 짤릴 때
			if(y < 0)
			{
				ot = 0;
				flags = DT_BOTTOM;
				tempU = -y;
				tempD = (*iter).mBoundingBox.GetHeight() + y;
				tempLD = mCharHeight + y;
				tempID = -y;
			}

			// 밑으로 짤릴 때
			else if(y + (*iter).mBoundingBox.GetHeight() > realHeight)
			{
				ot = 1;
				flags = DT_TOP;
				tempU = 0;
				tempD = realHeight - y;
				tempLD = tempD;
				tempID = tempD;
			}
			else
			{
				ot = 0;
				flags = 0;
				tempU = 0;
				tempD = mBoundingBox.GetHeight();
				tempLD = mCharHeight;
				tempID = 0;
			}

			y += mBoundingBox.top + 10;

			if(!(*iter).mVoiceName.empty()) 
			{
				int32 k = (!ot) ? (23 - tempID) : tempID;
				k = min(max(k, 0), 23);
				videoDrv->DrawImage(mSkinImage, x
					, y + tempU , 40
					, k
					, 0xffffff | (mAlpha << 24)
					, 0
					, ((!ot) ? tempID : 0) + ((mPickedType == EMPT_VOICEBUTTON && &(*mVoicedPicked) == &(*iter)) ? 23 : 0)
					, 40
					, k
					, 0, 1, 1, 0.4f);
			}
			if(!(*iter).mName.empty())
			{
				videoDrv->DrawText(mMessageLogger->GetNameFont(), 
					x + 40, y + tempU,
					mBoundingBox.GetWidth(), tempLD, (*iter).mName.c_str(), MixColorsAlpha((*iter).mColor, mAlpha), DT_WORDBREAK|flags);
			}

			videoDrv->DrawText(mMessageLogger->GetFont(), 
				x + (*iter).mBoundingBox.left, y + tempU,
				(*iter).mBoundingBox.GetWidth(), tempD, (*iter).mText.c_str(), MixColorsAlpha((*iter).mColor, mAlpha), DT_WORDBREAK|flags);

		}

		if(mSkinImage)
		{

			// 스크롤바 출력
			x = mBoundingBox.right - 5 - ScrollBarButtonWidth;
			y = 10 + mBoundingBox.top;
			videoDrv->DrawImage(mSkinImage, x, y, ScrollBarButtonWidth, mScrollBarHeight
				, (mAlpha << 24) | 0xffffff
				, 0, 46 + ((mPickedType == EMPT_UPBUTTON) ? mScrollBarHeight : 0),
				ScrollBarButtonWidth, mScrollBarHeight, 0, 1, 1, 0.4f);
			y += mScrollBarHeight;
			videoDrv->DrawImage(mSkinImage, x, y, ScrollBarButtonWidth
				, mScrollSpaceSize
				, (mAlpha << 24) | 0xffffff
				, 40, 0
				, ScrollBarButtonWidth
				, mSkinImage->GetSize().y, 0, 1, 1, 0.4f);

			// 스크롤바 현재 위치 그리기
			if(mMaxValue > 0)
			{
				videoDrv->DrawImage(mSkinImage, x,
					y + (float32)mValue / mMaxValue * (mScrollSpaceSize - mScrollBarSize), ScrollBarButtonWidth, mScrollBarSize
					, (mAlpha << 24) | 0xffffff
					, 40 + ScrollBarButtonWidth + ((mMousePressed && mPickedType == EMPT_SCROLLBAR) ? ScrollBarButtonWidth : 0)
					, 0, ScrollBarButtonWidth, (!mScrollBarLimitedSize) ? mSkinImage->GetSize().y : mScrollBarLimitedSize, 0, 1, 1, 0.4f);
			}
			y += mBoundingBox.GetHeight() - (mScrollBarHeight * 2 + 20);
			videoDrv->DrawImage(mSkinImage, x, y, ScrollBarButtonWidth, mScrollBarHeight
				, (mAlpha << 24) | 0xffffff
				, 0, 46 + mScrollBarHeight * 2 + ((mPickedType == EMPT_DOWNBUTTON) ? mScrollBarHeight : 0)
				, ScrollBarButtonWidth, mScrollBarHeight, 0, 1, 1, 0.4f);

		}

		if(mFlags & EBIF_IGNORE_CAMERA)
			videoDrv->EnableCamera(true);
	}

	// 인스턴스로 오는 메세지는 선별된 것임.
	bool nekoMessageLogInstance::OnEvent(const nekoEvent &event)
	{
		if(event.type == EVENT_WINDOW_MESSAGE)
		{
			if(event.uMsg == WM_LBUTTONDOWN)
			{
				mPickedType = _CheckPickedType(LOWORD(event.lParam), HIWORD(event.lParam));
				mMousePressed = true;
				if(mPickedType == EMPT_SCROLLBAR)
				{
					mScrollTemp = HIWORD(event.lParam) - 
						(mBoundingBox.top + 10 + _GetScrollBarPos());
				}
				else if(mPickedType == EMPT_SCROLLSPACE)
				{
					mPickedType = EMPT_SCROLLBAR;
					mScrollTemp = mScrollBarSize / 2;
					_OnMouseMove(MK_LBUTTON, LOWORD(event.lParam), HIWORD(event.lParam));
				}
				else if(mPickedType == EMPT_UPBUTTON && mMaxValue > 0)
				{
					mValue -= 30;
					mValue = min(max(mValue, 0), mMaxValue);
					mAutoScrollTimer.Clear();

					_OnValueUpdated();
				}
				else if(mPickedType == EMPT_DOWNBUTTON && mMaxValue > 0)
				{
					mValue += 30;
					mValue = min(max(mValue, 0), mMaxValue);
					mAutoScrollTimer.Clear();

					_OnValueUpdated();
				}
			}
			else if(event.uMsg == WM_MOUSEWHEEL)
			{
				int16 delta = HIWORD(event.wParam);
				if(delta < 0
					&& (mValue == mMaxValue))
				{
					GetNekoNovel()->GetCurrentGame()->CallCustomEvent(ECET_RECOLLECTED);
					return true;
				}
				mValue -= _GetOnePageSize() * delta / 240;
				mValue = min(max(mValue, 0), mMaxValue);

				_OnValueUpdated();
				return true;
			}
			else if(event.uMsg == WM_MOUSEMOVE)
			{
				_OnMouseMove(event.wParam, LOWORD(event.lParam), HIWORD(event.lParam));
				return true;
			}
			else if(event.uMsg == WM_LBUTTONUP)
			{
				if(mPickedType == EMPT_VOICEBUTTON)
				{
					list<nekoMessageLogBlock> &lstBlock = mMessageLogger->GetBlocks();
					if(mVoicedPicked != lstBlock.end())
					{
						GetNekoNovel()->GetCurrentGame()->PlayVoice((*mVoicedPicked).mVoiceName.c_str());
					}
				}
				mPickedType = EMPT_NOTHING;
				mMousePressed = false;
				mAutoScrollTimer.SetTime(0.6f);
				return true;
			}
		}

		return false;
	}

	/// 데이터 덤프하기
	result nekoMessageLogInstance::Dump(int32 version, nekoByteBuffer &outBuf)
	{
		nekoIBlockInstance::Dump(version, outBuf);

		return S_OK;
	}

	result nekoMessageLogInstance::Load(int32 version, nekoByteBuffer &inBuf)
	{
		nekoIBlockInstance::Load(version, inBuf);

		return S_OK;
	}

	/// Constructor
	nekoMessageLog::nekoMessageLog()
		: nekoIBlock(BLOCK_MESSAGELOG)
	{

	}

	/// Destructor
	nekoMessageLog::~nekoMessageLog()
	{

	}

	/// 인스턴스를 만든다
	nekoIBlockInstance *nekoMessageLog::CreateInstance()
	{
		return nekoNew nekoMessageLogInstance(this);
	}
}