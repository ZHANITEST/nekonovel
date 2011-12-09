#include "nekoPrecompiledHeader.h"
#include "nekoTrackBar.h"
#include "NekoNovel.h"
#include "nekoHWResourceManager.h"
#include "nekoUtility.h"

namespace neko
{
	nekoTrackBarInstance::nekoTrackBarInstance(nekoIBlock *baseBlock)
		: nekoIBlockInstance(baseBlock)
		, mMaxValue(100)
		, mTrackBarType(ETBIT_NOTHING)
		, mValue(0)
	{

	}

	nekoTrackBarInstance::~nekoTrackBarInstance()
	{

	}

	void nekoTrackBarInstance::SetBarTexture(const string &fileName)
	{
		mBarTexture = GetNekoNovel()->GetHWResourceManager()->LoadTexture((string("이미지\\") + fileName).c_str());
	}

	void nekoTrackBarInstance::SetTrackBarType(E_TRACK_BAR_INSTANCE_TYPE type)
	{
		switch(type)
		{
		case ETBIT_TEXTWINDOW:
			mMaxValue = 255;
			break;
		case ETBIT_SUB_TEXTSPEED:
			mMaxValue = 60;
			break;
		}
		mTrackBarType = type;
	}

	void nekoTrackBarInstance::Update(float32 frameTime)
	{
		switch(mTrackBarType)
		{
		case ETBIT_BGM:
			mValue = GetNekoNovel()->GetCurrentGame()->GetEnvironment().mDefBGMVolume;
			break;
		case ETBIT_VOICE:
			mValue = GetNekoNovel()->GetCurrentGame()->GetEnvironment().mDefVoiceVolume;
			break;
		case ETBIT_FX:
			mValue = GetNekoNovel()->GetCurrentGame()->GetEnvironment().mDefFXVolume;
			break;
		case ETBIT_TEXTWINDOW:
			mValue = GetNekoNovel()->GetCurrentGame()->GetEnvironment().mTextWindowAlpha;
			break;
		case ETBIT_VARIABLE:
			{
				nekoVariable *var = GetNekoNovel()->GetCurrentGame()->GetVariable(mVarName.c_str());
				if(var)
				{
					mValue = atoi(var->GetValue());
					mValue = min(max(mValue, 0), mMaxValue);
				}
			}
			break;
		case ETBIT_SUB_TEXTSPEED:
			mValue = -(int32)(GetNekoNovel()->GetCurrentGame()->GetEnvironment().mTextSubSpeed * 1000.0f) + 30;
			break;
		}
	}

	void nekoTrackBarInstance::Draw()
	{
		if(mBarTexture)
		{
			if(mFlags & EBIF_IGNORE_CAMERA)
				GetNekoNovel()->GetVideoDriver()->EnableCamera(false);

			GetNekoNovel()->GetVideoDriver()->DrawImage(mBarTexture,
				mBoundingBox.left - mBarTexture->GetSize().x / 2 + (float32)mValue / mMaxValue * mBoundingBox.GetWidth(), mBoundingBox.top, -1, mBoundingBox.GetHeight(), D3DCOLOR_ARGB(mAlpha, 255, 255, 255),
				0, 0, -1, -1, 0, 1, 1, 0.45f);

			if(mFlags & EBIF_IGNORE_CAMERA)
				GetNekoNovel()->GetVideoDriver()->EnableCamera(true);

		}
	}

	bool nekoTrackBarInstance::OnEvent(const nekoEvent &event)
	{
		if(event.type == EVENT_WINDOW_MESSAGE)
		{
			if(event.uMsg == WM_LBUTTONDOWN)
			{
				nekoPoint point = nekoPoint(LOWORD(event.lParam), HIWORD(event.lParam));
				float32 one = (float32)mBoundingBox.GetWidth() / mMaxValue;
				mValue = min(max((point.x - mBoundingBox.left) / one, 0), mMaxValue);
				OnValueUpdated();

				return true;
			}
			else if(event.uMsg == WM_MOUSEMOVE)
			{
				if(event.wParam & MK_LBUTTON)
				{
					nekoPoint point = nekoPoint(LOWORD(event.lParam), HIWORD(event.lParam));
					float32 one = (float32)mBoundingBox.GetWidth() / mMaxValue;

					mValue = min(max((point.x - mBoundingBox.left) / one, 0), mMaxValue);
					OnValueUpdated();
				}
				return true;
			}
		}
		return false;
	}

	void nekoTrackBarInstance::OnValueUpdated()
	{
		nekoGame *currentGame = GetNekoNovel()->GetCurrentGame();

		switch(mTrackBarType)
		{
		case ETBIT_VARIABLE:
			{
				if(mVarName.empty())
					return;

				nekoVariable *var = currentGame->GetVariable(mVarName.c_str());
				if(!var)
				{
					var = currentGame->AddVariable(mVarName.c_str());
				}

				if(var)
				{
					var->SetValue(nekoUtility::FormString("%d", mValue));
				}
			}
			break;
		case ETBIT_VOICE:
			currentGame->GetEnvironment().mDefVoiceVolume = mValue;
			if(currentGame->GetVoice())
				currentGame->GetVoice()->SetAlpha(mValue / 100.0f * 255.0f);
			break;
		case ETBIT_FX:
			currentGame->GetEnvironment().mDefFXVolume = mValue;
			break;
		case ETBIT_BGM:
			currentGame->GetEnvironment().mDefBGMVolume = mValue;
			if(currentGame->GetBGMusic())
				currentGame->GetBGMusic()->SetAlpha(mValue / 100.0f * 255.0f);
			break;
		case ETBIT_TEXTWINDOW:
			currentGame->GetEnvironment().mTextWindowAlpha = mValue;
			break;
		case ETBIT_SUB_TEXTSPEED:
			currentGame->GetEnvironment().mTextSubSpeed = -(mValue - 30) * 0.001f;
			break;
		}
	}

	/// 데이터 덤프하기
	result nekoTrackBarInstance::Dump(int32 version, nekoByteBuffer &outBuf)
	{
		nekoIBlockInstance::Dump(version, outBuf);

		outBuf << mMaxValue
			<< ((mBarTexture) ? mBarTexture->GetName() : "")
			<< mTrackBarType
			<< mVarName;

		return S_OK;
	}

	result nekoTrackBarInstance::Load(int32 version, nekoByteBuffer &inBuf)
	{
		nekoIBlockInstance::Load(version, inBuf);

		string path;
		inBuf >> mMaxValue
			>> path;

		if(!path.empty())
			mBarTexture = GetNekoNovel()->GetHWResourceManager()->LoadTexture(path.c_str());

		inBuf >> mTrackBarType;
		if(version >= 1113)
			inBuf >> mVarName;

		return S_OK;
	}

	/// Constructor
	nekoTrackBar::nekoTrackBar()
		: nekoIBlock(BLOCK_TRACKBAR)
	{

	}

	/// Destructor
	nekoTrackBar::~nekoTrackBar()
	{

	}

	/// 인스턴스를 만든다
	nekoIBlockInstance *nekoTrackBar::CreateInstance()
	{
		return nekoNew nekoTrackBarInstance(this);
	}
}