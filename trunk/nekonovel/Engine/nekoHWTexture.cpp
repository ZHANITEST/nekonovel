#include "nekoPrecompiledHeader.h"
#include "nekoHWTexture.h"
#include "nekoLogger.h"
#include "nekoHWResourceManager.h"
#include "nekoUtility.h"
#include "NekoNovel.h"

namespace neko
{
	nekoHWTexture::nekoHWTexture(nekoHWResourceManager *resMgr, const char8 *fileName)
		: nekoIHWResource(resMgr, EHRT_TEXTURE)
		, mDataLength(0)
		, mDynamicBuffer(false)
		, mHandle(NULL)
		, mColorKey(0)
		, mPreserveSize(false)
	{
		Catch();
		mName = fileName;
	}

	nekoHWTexture::nekoHWTexture(nekoHWResourceManager *resMgr, int32 width, int32 height, bool dynamicBuffer)
		: nekoIHWResource(resMgr, EHRT_TEXTURE)
		, mImageSize(width, height)
		, mDataLength(0)
		, mDynamicBuffer(dynamicBuffer)
		, mHandle(NULL)
		, mColorKey(0)
		, mPreserveSize(false)
	{
		Catch();
		mAutoDropping = false;

		SetReleaseDelay(-1.0f);
	}

	nekoHWTexture::nekoHWTexture(nekoHWResourceManager *resMgr, char8 *data, int32 length)
		: nekoIHWResource(resMgr, EHRT_TEXTURE)
		, mData(data)
		, mDataLength(length)
		, mDynamicBuffer(false)
		, mHandle(NULL)
		, mColorKey(0)
		, mPreserveSize(false)
		, mVideoSize(1, 1)
	{
		SetReleaseDelay(-1.0f);

		Catch();
	}

	/// Destructor
	nekoHWTexture::~nekoHWTexture()
	{
#ifdef _DEBUG
		//nekoLogger::Log(LOG_LEVEL_DINFO, "텍스쳐 '%s'(or 데이터 : %d) 언로드", mName.c_str(), mDataLength);
#endif

		if(mHandle)
			mHandle->Release();

		if(mDataLength > 0)
			delete []mData;
	}

	/// On Device Lost
	void nekoHWTexture::OnDeviceLost()
	{
		if(mDynamicBuffer && mHandle)
		{
			mHandle->Release();
			mHandle = NULL;
		}
	}

	/// On Device Reset
	void nekoHWTexture::OnDeviceReset()
	{
		if(mDynamicBuffer)
		{
			if(FAILED(mResManager->GetVideoDriver()->GetHandle()->CreateTexture(mImageSize.x, mImageSize.y,
				1, (mDynamicBuffer) ? D3DUSAGE_DYNAMIC : 0, D3DFMT_A8R8G8B8, (mDynamicBuffer) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &mHandle, 0)))
			{
				LogPrint(LOG_LEVEL_WARN, "텍스쳐 생성 실패(크기 %d, %d)", mImageSize.x, mImageSize.y);
				mLastReturnCode = E_FAIL;
				mState = EHRS_NONE;
				return;
			}
		}
	}

	/// 실제로 로드/언로드를 시도하는 메소드
	result nekoHWTexture::OnCatching()
	{
		mLockSeciton.Begin();

		if(mHandle != NULL)
		{
			mState = EHRS_LOADED;
			mLockSeciton.End();
			return S_OK;
		}

		if(mName.length() > 0)
		{
			char8 *data;
			int32 length;

			if(FAILED(GetNekoNovel()->GetFileSystem()->LoadData(mName.c_str(), &data, &length)))
			{
				LogPrint(LOG_LEVEL_WARN, "'%s' 텍스쳐 파일을 로드할 수 없습니다.", mName.c_str());
				mState = EHRS_NONE;
				mLockSeciton.End();
				return (mLastReturnCode = E_NOT_EXIST);
			}

			D3DXIMAGE_INFO srcInfo;
			result res;
			if(FAILED(res = D3DXCreateTextureFromFileInMemoryEx(mResManager->GetVideoDriver()->GetHandle(),
				data, length,
				D3DX_DEFAULT_NONPOW2,
				D3DX_DEFAULT_NONPOW2,
				1, 0, D3DFMT_A8R8G8B8,
				D3DPOOL_MANAGED,
				D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR, mColorKey, &srcInfo, 0, &mHandle)))
			{
				delete []data;

				LogPrint(LOG_LEVEL_WARN, "'%s' 텍스쳐 로드 실패(오류 코드 : %d)", mName.c_str(), res);
				mState = EHRS_NONE;
				mLockSeciton.End();
				return (mLastReturnCode = res);
			}
			delete []data;

			mImageSize = nekoPoint(srcInfo.Width, srcInfo.Height);

			if(mPreserveSize)
			{
				mVideoSize = mImageSize;
			}
			else
			{

				D3DSURFACE_DESC desc;

				mVideoSize = nekoPoint(1, 1);
				if(FAILED(mHandle->GetLevelDesc(0, &desc)))
				{
					while(mVideoSize.x <= mImageSize.x) mVideoSize.x <<= 1;
					while(mVideoSize.y <= mImageSize.y) mVideoSize.y <<= 1;
				}
				else
				{
					mVideoSize = nekoPoint(desc.Width, desc.Height);
				}
			}
		}
		else if(mDataLength > 0)
		{
			D3DXIMAGE_INFO srcInfo;
			result res;
			if(FAILED(res = D3DXCreateTextureFromFileInMemoryEx(mResManager->GetVideoDriver()->GetHandle(),
				mData, mDataLength,
				D3DX_DEFAULT_NONPOW2,
				D3DX_DEFAULT_NONPOW2,
				1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
				D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR, 0, &srcInfo, 0, &mHandle)))
			{
				LogPrint(LOG_LEVEL_WARN, "메모리 텍스쳐 로드 실패(오류 코드 : %d)", res);
				mState = EHRS_NONE;
				mLockSeciton.End();
				return (mLastReturnCode = res);
			}


			mImageSize = nekoPoint(srcInfo.Width, srcInfo.Height);

			if(mPreserveSize)
			{
				mVideoSize = mImageSize;
			}
			else
			{
				D3DSURFACE_DESC desc;

				mVideoSize = nekoPoint(1, 1);
				if(FAILED(mHandle->GetLevelDesc(0, &desc)))
				{
					while(mVideoSize.x <= mImageSize.x) mVideoSize.x <<= 1;
					while(mVideoSize.y <= mImageSize.y) mVideoSize.y <<= 1;
				}
				else
				{
					mVideoSize = nekoPoint(desc.Width, desc.Height);
				}
			}
		}
		else
		{
			if(FAILED(mResManager->GetVideoDriver()->GetHandle()->CreateTexture(mImageSize.x, mImageSize.y,
				1, (mDynamicBuffer) ? D3DUSAGE_DYNAMIC : 0, D3DFMT_A8R8G8B8, (mDynamicBuffer) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &mHandle, 0)))
			{
				LogPrint(LOG_LEVEL_WARN, "텍스쳐 생성 실패(크기 %d, %d)", mImageSize.x, mImageSize.y);
				mState = EHRS_NONE;
				mLockSeciton.End();
				return (mLastReturnCode = E_FAIL);
			}

			D3DSURFACE_DESC desc;

			mVideoSize = nekoPoint(1, 1);
			if(FAILED(mHandle->GetLevelDesc(0, &desc)))
			{
				while(mVideoSize.x <= mImageSize.x) mVideoSize.x <<= 1;
				while(mVideoSize.y <= mImageSize.y) mVideoSize.y <<= 1;
			}
			else
			{
				mVideoSize = nekoPoint(desc.Width, desc.Height);
			}
		}

#ifdef _DEBUG
		//nekoLogger::Log(LOG_LEVEL_DINFO, "텍스쳐 '%s'(or 데이터 : %d) 로드", mName.c_str(), mDataLength);
#endif

		//mVideoSize = nekoPoint(desc.Width, desc.Height);

		mLockSeciton.End();

		return nekoIHWResource::OnCatching();
	}

	result nekoHWTexture::OnDropping()
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

#ifdef _DEBUG
		//nekoLogger::Log(LOG_LEVEL_DINFO, "텍스쳐 '%s'(or 데이터 : %d) 언로드", mName.c_str(), mDataLength);
#endif

		mLockSeciton.End();
		return nekoIHWResource::OnDropping();
	}
}