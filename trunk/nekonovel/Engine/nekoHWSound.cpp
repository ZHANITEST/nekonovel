#include "nekoPrecompiledHeader.h"
#include "nekoHWSound.h"
#include "nekoLogger.h"
#include "nekoHWResourceManager.h"
#include <audiere.h>
#include "nekoNovelReal.h"

namespace neko
{
	nekoHWSound::nekoHWSound(nekoHWResourceManager *resMgr, const char8 *fileName)
		: nekoIHWResource(resMgr, EHRT_SOUND)
		, mHandle(NULL)
		, mVolume(1.0f)
		, mPlayWhenLoad(true)
	{
		Catch();
		mName = fileName;
		nekoUtility::TrimRight(mName);

		SetTimeout(30);
	}

	/// Destructor
	nekoHWSound::~nekoHWSound()
	{
		if(mHandle)
		{
			mHandle->stop();
			mHandle->unref();
		}
	}

	/// 실제로 로드/언로드를 시도하는 메소드
	result nekoHWSound::OnCatching()
	{
		mLockSeciton.Begin();

		if(mHandle != NULL)
		{
			mState = EHRS_LOADED;
			mLockSeciton.End();
			return S_OK;
		}
		
		char8 *data;
		int32 length;

		if(FAILED(GetNekoNovel()->GetFileSystem()->LoadData(mName.c_str(), &data, &length)))
		{
			LogPrint(LOG_LEVEL_WARN, "'%s' 사운드 파일을 로드할 수 없습니다.", mName.c_str());
			mState = EHRS_NONE;
			mLockSeciton.End();
			return (mLastReturnCode = E_NOT_EXIST);
		}

		File *filePtr = hidden::AdrCreateMemoryFile(data, length);
		delete []data;

		mHandle = OpenSoundEffect(mResManager->GetSoundDriver()->GetHandle(), filePtr, MULTIPLE);
		
		//mHandle = OpenSound(mResManager->GetSoundDriver()->GetHandle(), filePtr, true);

		if(!mHandle)
		{
			mState = EHRS_NONE;

			mLastReturnCode = E_FAIL;
			mLockSeciton.End();

			LogPrint(LOG_LEVEL_WARN, "'%s' 사운드 로드 실패", mName.c_str());
			return (mLastReturnCode = E_FAIL);
		}

		// 원래 Audiere는 주로 Auto pointer로 받기 때문에 꼭 ref를 해주어야한다.
		mHandle->ref();
		mHandle->setVolume(mVolume);
		if(mPlayWhenLoad)
			mHandle->play();

		mLockSeciton.End();
		return nekoIHWResource::OnCatching();
	}

	result nekoHWSound::OnDropping()
	{
		mLockSeciton.Begin();

		if(mHandle == NULL)
		{
			mState = EHRS_NONE;
			mLockSeciton.End();
			return S_OK;
		}

		mHandle->stop();
		mHandle->unref();

		mHandle = NULL;

		mLockSeciton.End();
		return nekoIHWResource::OnDropping();
	}

	/// 정지 부탁!
	void nekoHWSound::Stop()
	{
		if(mHandle)
			mHandle->stop();

		mPlayWhenLoad = false;
	}

	/// 재생 부탁!
	void nekoHWSound::Play()
	{
		if(mHandle)
			mHandle->play();

		if(SUCCEEDED(mLastReturnCode))
			Refresh();
	}

	void nekoHWSound::SetVolume(float32 volume)
	{
		mLockSeciton.Begin();

		mVolume = volume;

		if(mHandle)
			mHandle->setVolume(volume);
		mLockSeciton.End();
	}
}