#include "nekoPrecompiledHeader.h"
#include "nekoHWFont.h"
#include "nekoVideoDriver.h"
#include "nekoHWResourceManager.h"
#include "nekoLogger.h"
#include "nekoUtility.h"

namespace neko
{
	nekoHWFont::nekoHWFont(nekoHWResourceManager *resMgr, const char8 *faceName, int32 width, int32 height, bool bold)
		: mWidth(width)
		, mHeight(height)
		, mBold(bold)
		, nekoIHWResource(resMgr, EHRT_FONT)
		, mHandle(NULL)
	{
		mName = faceName;
		nekoUtility::TrimRight(mName);
	}

	nekoHWFont::~nekoHWFont()
	{
		if(mHandle)
			mHandle->Release();
	}

	/// On Device Lost
	void nekoHWFont::OnDeviceLost()
	{
		if(mHandle)
			mHandle->OnLostDevice();
	}

	/// On Device Reset
	void nekoHWFont::OnDeviceReset()
	{
		if(mHandle)
			mHandle->OnResetDevice();
	}

	/// 실제로 로드/언로드를 시도하는 메소드
	result nekoHWFont::OnCatching()
	{
		mLockSeciton.Begin();

		if(mHandle != NULL)
		{
			mState = EHRS_LOADED;
			mLockSeciton.End();
			return S_OK;
		}

		if(FAILED(D3DXCreateFont(mResManager->GetVideoDriver()->GetHandle(), mHeight, mWidth, (mBold) ? FW_BOLD : 0, 1, false, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH|FF_DONTCARE, mName.c_str(), &mHandle)))
		{
			LogPrint(LOG_LEVEL_WARN, "'%s' 폰트 로드 실패", mName.c_str());
			mState = EHRS_NONE;
			mLockSeciton.End();
			return (mLastReturnCode = E_FAIL);
		}

		mLockSeciton.End();

		return nekoIHWResource::OnCatching();
	}

	result nekoHWFont::OnDropping()
	{

		mLockSeciton.Begin();

		if(mHandle == NULL)
		{
			mState = EHRS_NONE;
			mLockSeciton.End();
			return S_OK;
		}

		mHandle->Release();
		mHandle = NULL;

		mLockSeciton.End();
		return nekoIHWResource::OnDropping();
	}
}