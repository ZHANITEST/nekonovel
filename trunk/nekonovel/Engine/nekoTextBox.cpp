#include "nekoPrecompiledHeader.h"
#include "nekoTextBox.h"
#include "NekoNovel.h"
#include "nekoHWResourceManager.h"
#include "nekoUtility.h"

namespace neko
{
	void nekoTextBoxInstance::_PressOK()
	{
		if(mStringToDraw.empty())
			return;

		nekoVariable *var = GetNekoNovel()->GetCurrentGame()->AddVariable(mVarName.c_str());
		if(var)
			var->SetStringValue(mStringToDraw.c_str());

		GetNekoNovel()->GetCurrentGame()->OnTextInputCompleted();
	}

	nekoTextBoxInstance::nekoTextBoxInstance(nekoIBlock *baseBlock)
		: nekoIBlockInstance(baseBlock)
		, mNumeric(false)
		, mPassword(false)
		, mComp(false)
		, caret(0)
		, mMaxLength(128)
		, mButtonState(EBS_NORMAL)
		, mCaretPos(0)
	{
		mZOrder = 70;
		ClearCompBuffer();
		mIMEHandle = GetNekoNovel()->GetIMEHandle();
	}

	nekoTextBoxInstance::~nekoTextBoxInstance()
	{

	}

	result nekoTextBoxInstance::Initialize(const string &varName, nekoRectangle &rect, uint32 maxLength, const char8 *imageName, const char8 *fx1, const char8 *fx2,
		uint32 color, int32 fontSize, int32 fontBold, const char8 *fontName)
	{
		mVarName = varName;
		mBoundingBox = rect;
		mMaxLength = maxLength;
		mButtonTexture = GetNekoNovel()->GetHWResourceManager()->LoadTexture((string("이미지\\") + imageName).c_str());
		if(!fx1 || strcmp(fx1, "없음") == 0)
			mFxUp = 0;
		else
		{
			mFxUp = GetNekoNovel()->GetHWResourceManager()->LoadSound((string("효과음\\") + fx1).c_str());
			mFxUp->Stop();
		}
		if(!fx2 || strcmp(fx2, "없음") == 0)
			mFxDown = 0;
		else
		{
			mFxDown = GetNekoNovel()->GetHWResourceManager()->LoadSound((string("효과음\\") + fx2).c_str());
			mFxDown->Stop();
		}

		mColor = color;

		// 폰트 변경 필요 없음.
		if(fontSize == -1 && fontBold == -1 && fontName == 0)
		{
			return S_OK;
		}
		else
		{
			if(fontSize < 0)
			{
				if(mFont)
					fontSize = mFont->GetHeight();
				else
					fontSize = NEKO_DEFFONT_HEIGHT;
			}

			if(fontBold < 0)
			{
				if(mFont)
					fontBold = (mFont->IsBold()) ? 1 : 0;
				else
					fontBold = 0;
			}

			if(fontName == 0)
			{
				if(mFont)
					fontName = mFont->GetName();
				else
					fontName = NEKO_DEFFONT_NAME;
			}

			mFont = GetNekoNovel()->GetHWResourceManager()->LoadFont(fontName, 0, fontSize, (bool)fontBold);
		}

		return S_OK;
	}

	/// Update Caption
	void nekoTextBoxInstance::UpdateCaption()
	{
		if(!IsPassword())
		{
			mStringToDraw = mCaption;
			if(mComp && mCompBuf[0])
				mStringToDraw.insert(caret, mCompBuf);
		}
		else 
		{
			mStringToDraw = "";
			for(uint16 i = 0;i < mCaption.length() + strlen(mCompBuf);++i)
				mStringToDraw += '*';
		}

		// 캐럿 위치 계산하기
		char8 buf[3];
		SIZE size = {0, };
		int32 ourCaret = caret;

		if(mComp && mCompBuf[0])
			ourCaret++; 

		buf[2] = 0;
		mCaretPos = 0;
		for(uint32 i = 0;i < mStringToDraw.length() && i < ourCaret;++i)
		{
			if(nekoUtility::IsMultibyteCode(mStringToDraw[i]))
			{
				buf[0] = mStringToDraw[i];
				buf[1] = mStringToDraw[i++];
			}
			else
			{
				buf[0] = mStringToDraw[i];
				buf[1] = 0;
			}

			GetTextExtentPoint32((mFont) ? mFont->GetDC() : GetNekoNovel()->GetVideoDriver()->GetDefaultFont()->GetDC(), buf, (buf[1]) ? 2 : 1, &size);
			mCaretPos += size.cx;
		}

	}

	void nekoTextBoxInstance::Update(float32 frameTime)
	{
		if(GetNekoNovel()->GetCurrentGame()->GetPickedInstance() != this)
			mButtonState = EBS_NORMAL;
	}

	void nekoTextBoxInstance::Draw()
	{
		if(!IsVisible())
			return;

		nekoVideoDriver *videoDrv = GetNekoNovel()->GetVideoDriver();

		nekoRectangle btRect(mBoundingBox.right, mBoundingBox.top);
		btRect.left -= mButtonTexture->GetSize().x / 3;
		btRect.SetSize(mButtonTexture->GetSize().x / 3, mBoundingBox.GetHeight());

		videoDrv->DrawImage(mButtonTexture, btRect.left, btRect.top, btRect.GetWidth(), btRect.GetHeight(), -1, mButtonState * btRect.GetWidth(), 0, btRect.GetWidth(), -1, 0, 1, 1, _GetRealZ());

		// 텍스트 출력
		videoDrv->DrawText(mFont, mBoundingBox.left, mBoundingBox.top, btRect.left - mBoundingBox.left, mBoundingBox.GetHeight(), mStringToDraw.c_str(), mColor
			, DT_VCENTER|DT_SINGLELINE, _GetRealZ());
		videoDrv->DrawText(mFont, mBoundingBox.left + mCaretPos, mBoundingBox.top, btRect.left - mBoundingBox.left - mCaretPos, mBoundingBox.GetHeight(), "|", mColor
			, DT_VCENTER|DT_SINGLELINE, _GetRealZ());
	}

	bool nekoTextBoxInstance::OnEvent(const nekoEvent &event)
	{
		if(event.type == EVENT_WINDOW_MESSAGE)
		{
			if(event.uMsg == WM_MOUSEMOVE)
			{
				if(_IsInButton(LOWORD(event.lParam), HIWORD(event.lParam)))
				{
					if(mButtonState == EBS_NORMAL)
					{
						if(mFxUp)
						{
							mFxUp->SetVolume(
								GetNekoNovel()->GetCurrentGame()->GetEnvironment().mDefFXVolume / 100.0f
								);
							mFxUp->Play();
						}
						mButtonState = EBS_ROLLOVER;
					}
				}
				else
				{
					mButtonState = EBS_NORMAL;
				}
				return true;
			}
			else if(event.uMsg == WM_LBUTTONDOWN)
			{
				if(_IsInButton(LOWORD(event.lParam), HIWORD(event.lParam))
					&& mButtonState == EBS_ROLLOVER)
				{
					if(mFxDown)
					{
						mFxDown->SetVolume(
							GetNekoNovel()->GetCurrentGame()->GetEnvironment().mDefFXVolume / 100.0f
							);
						mFxDown->Play();
					}
					mButtonState = EBS_PRESSED;
					return true;
				}
			}
			else if(event.uMsg == WM_LBUTTONUP)
			{
				if(_IsInButton(LOWORD(event.lParam), HIWORD(event.lParam)))
				{
					_PressOK();
					mButtonState = EBS_ROLLOVER;
					return true;
				}
			}
			if(event.uMsg == WM_IME_STARTCOMPOSITION)
			{
				/// Is for number ?
				if(IsNumeric())
					return false;

				mComp = true;
				return true;
			}
			else if(event.uMsg == WM_IME_COMPOSITION)
			{
				/// Is for number ?
				if(IsNumeric())
					return false;

				int16 len;

				if(event.lParam & GCS_RESULTSTR)
				{
					len = ImmGetCompositionString(mIMEHandle, GCS_RESULTSTR, 0, 0);
					if(len > 0)
						ImmGetCompositionString(mIMEHandle, GCS_RESULTSTR, mCompBuf, len);
					mCompBuf[len] = '\0';

					if(len > 2)
					{
						if(mCompBuf[2] == ' ')
						{
							len = 2;
							mCompBuf[2] = '\0';
						}
					}

					// Not compositing.
					if(!mComp)
						return false;

					/// Over-range.
					if((mCaption.length() + (uint32)len) > mMaxLength)
						return false;

					mCaption.insert(caret, mCompBuf);
					caret += len;

					//if(!IsMultiByte(mCompBuf[len - 1]))
					//	mComp = false;
					ClearCompBuffer();
				}
				else
				{
					len = ImmGetCompositionString(mIMEHandle, GCS_COMPSTR, 0, 0);
					if(len > 0)
						ImmGetCompositionString(mIMEHandle, GCS_COMPSTR, mCompBuf, len);

					mCompBuf[len] = '\0';
				}

				UpdateCaption();
				return true;
			} else if(event.uMsg == WM_KEYDOWN)
			{
				switch(event.wParam)
				{
					case VK_ESCAPE:
					case VK_TAB:
					//case VK_KP_ENTER:
						return false;

					case VK_RETURN:
						_PressOK();
						return true;

					case VK_HOME:
						caret = 0;
						UpdateCaption();
						break;
					case VK_END:
						caret = mCaption.length();
						UpdateCaption();
						break;

					case VK_LEFT:
						mComp = false;
						mCompBuf[0] = '\0';

						if(caret > 0)
						{
							bool hangul = false;
							for(int32 i = 0;i < caret;++i)
							{
								if(nekoUtility::IsMultibyteCode(mCaption[i]))
								{	
									++i;
									hangul = true;
								}
								else
								{
									hangul = false;
								}
							}

							if(mCaption.length() >= 2 && hangul)
								caret--;

							caret--;
						}

						UpdateCaption();

						break;
					case VK_RIGHT:
						mComp = false;
						mCompBuf[0] = '\0';

						caret = min(caret + 1, (int32)mCaption.length());
						if(caret < (int32)mCaption.length())
						{
							if(nekoUtility::IsMultibyteCode(mCaption[caret - 1]))
								++caret;
						}

						UpdateCaption();

						break;

						// Paste
					//case VK_V:
					case (0x41 + ('V' - 'A')):
						if(GetAsyncKeyState(VK_CONTROL))
						{
							// Stop to composite.
							if(mComp)
							{
								if(mCompBuf[0] == '\0')
									mComp = false;
								return true;
							}

							// Open clipboard.
							if(!OpenClipboard(GetNekoNovel()->GetHWND()))
								break;

							// Open data buffer.
							HGLOBAL hglb = GetClipboardData(CF_TEXT);
							if(hglb == NULL)
								break;

							// Copy data.
							LPTSTR lptstr;
							lptstr = (LPTSTR)GlobalLock(hglb);
							mCaption.insert(caret, lptstr);
							caret += strlen(lptstr);

							if(mCaption.length() >= mMaxLength)
							{
								mCaption = mCaption.substr(0, mMaxLength);
								caret = min(caret, (int32)mMaxLength);
							}

							uint32 i = -1;
							while((i = mCaption.find('\n')) != -1
								|| (i = mCaption.find('\r')) != -1)
							{
								mCaption[i] = ' ';
							}

							GlobalUnlock(hglb);

							// Update mCaption.
							UpdateCaption();

							// Close clipboard.
							CloseClipboard();

							return true;
						}
						break;

						// Delete after word
					case VK_DELETE:

						// Stop to composite.
						if(mComp)
						{
							if(mCompBuf[0] == '\0')
								mComp = false;
							else
								return true;
						}

						if(caret >= (int32)mCaption.length())
							return true;

						if(nekoUtility::IsMultibyteCode(mCaption[caret]))
						{
							if(mCaption.length() - caret >= 2)
								mCaption.erase(caret, 2);
							else
								mCaption.erase(caret, 1);
						}
						else
						{
							mCaption.erase(caret, 1);
						}

						UpdateCaption();
						break;
				};

				return true;
			} else if(event.uMsg == WM_KEYUP)
			{
				return true;
			} else if(event.uMsg == WM_CHAR)
			{
				// 백스페이스.
				switch(event.wParam)
				{
				case '\t':
				case '\r':
				case '\n':
				case 27:
					return false;
				case VK_BACK:
				{
					// Stop to composite.
					if(mComp)
					{
						if(mCompBuf[0] == '\0')
							mComp = false;
						else
							return true;
					}

					/// Is Hangul ?
					if(caret >= 2 && mCaption.length() >= 2)
					{
						bool hangul = false;
						for(int32 i = 0;i < caret;++i)
						{
							if(nekoUtility::IsMultibyteCode(mCaption[i]))
							{	
								++i;
								hangul = true;
							}
							else
							{
								hangul = false;
							}
						}

						// Is Hangul ?
						if(hangul)
						{
							// Erase last character.
							mCaption.erase(caret - 2, 2);
							caret -= 2;

							UpdateCaption();

							return true;
						}
					}
					if(caret >= 1 && mCaption.length() >= 1)
					{
						// Erase last character.
						mCaption.erase(caret - 1, 1);
						caret --;
					}

					UpdateCaption();
					return true;
				}
				break;
				}

				// Pass..
				if((GetKeyState(VK_MENU) & 0x8000) || (GetKeyState(VK_CONTROL) & 0x8000))
				{
					return true;
				}

				/// Over-range.
				if(static_cast<int32>(mCaption.length()) + 1 > mMaxLength)
					return true;

				/// Is for number ?
				if(IsNumeric() &&
					!(event.wParam >= '0' && event.wParam <= '9'))
					return true;

				char8 buf[2] = {(char8)event.wParam, 0};
				mCaption.insert(caret, buf);
				caret++;

				// Stop to composite.
				mComp = false;
				ClearCompBuffer();
				UpdateCaption();

				return true;
			}
		}
		return false;
	}

	/// 데이터 덤프하기
	result nekoTextBoxInstance::Dump(int32 version, nekoByteBuffer &outBuf)
	{
		nekoIBlockInstance::Dump(version, outBuf);

		outBuf << mCaption
			<< mVarName
			<< mMaxLength
			<< mColor
			<< ((mButtonTexture) ? mButtonTexture->GetName() : "")
			<< ((mFxUp) ? mFxUp->GetName() : "")
			<< ((mFxDown) ? mFxDown->GetName() : "")
			<< ((mFont) ? mFont->GetName() : "");

		if(mFont)
		{
			outBuf << mFont->GetHeight()
				   << mFont->IsBold();
		}

		return S_OK;
	}

	result nekoTextBoxInstance::Load(int32 version, nekoByteBuffer &inBuf)
	{
		nekoIBlockInstance::Load(version, inBuf);

		string str;

		inBuf >> mCaption
			>> mVarName
			>> mMaxLength
			>> mColor
			>> str;

		if(!str.empty())
			mButtonTexture = GetNekoNovel()->GetHWResourceManager()->LoadTexture(str.c_str());

		inBuf >> str;
		if(!str.empty())
			mFxUp = GetNekoNovel()->GetHWResourceManager()->LoadSound(str.c_str());

		inBuf >> str;
		if(!str.empty())
			mFxDown = GetNekoNovel()->GetHWResourceManager()->LoadSound(str.c_str());

		inBuf >> str;
		if(!str.empty())
		{
			int32 height;
			bool bold;

			inBuf >> height
				>> bold;

			mFont = GetNekoNovel()->GetHWResourceManager()->LoadFont(str.c_str(), 0, height, bold);
		}

		UpdateCaption();
		return S_OK;
	}

	/// Constructor
	nekoTextBox::nekoTextBox()
		: nekoIBlock(BLOCK_TEXTBOX)
	{

	}

	/// Destructor
	nekoTextBox::~nekoTextBox()
	{

	}

	/// 인스턴스를 만든다
	nekoIBlockInstance *nekoTextBox::CreateInstance()
	{
		return nekoNew nekoTextBoxInstance(this);
	}
}