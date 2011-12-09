#include "nekoPrecompiledHeader.h"
#include "nekoHWResourceManager.h"

namespace neko
{
	/// Constructor
	nekoHWResourceManager::nekoHWResourceManager(nekoVideoDriver *videoDriver, nekoSoundDriver *soundDriver)
		: mVideoDriver(videoDriver)
		, mSoundDriver(soundDriver)
		, mWaitingLoad(false)
	{

	}

	/// Destructor
	nekoHWResourceManager::~nekoHWResourceManager()
	{
		mLockSection.Begin();
		list<nekoIHWResource *>::iterator iter = mManagedResourceList.begin();
		for(;iter != mManagedResourceList.end();)
		{
			nekoIHWResource *res = (*iter++);

			res->Release();
		}
		mManagedResourceList.clear();
		mLockSection.End();
	}

	void nekoHWResourceManager::_AddHWResource(nekoIHWResource *resource)
	{
		nekoAssert(resource, "");

		mLockSection.Begin();
		mManagedResourceList.push_back(resource);
		mLockSection.End();
	}

	bool nekoHWResourceManager::_RemoveHWResource(nekoIHWResource *resource)
	{
		mLockSection.Begin();

		list<nekoIHWResource *>::iterator iter = mManagedResourceList.begin();
		for(;iter != mManagedResourceList.end();++iter)
		{
			if((*iter) == resource)
			{
				mManagedResourceList.erase(iter);
				mLockSection.End();
				return true;
			}
		}

		mLockSection.End();
		return false;
	}

	/// Load Sound
	nekoHWSound *nekoHWResourceManager::LoadSound(const char8 *fileName)
	{
		mLockSection.Begin();

		nekoHWSound *res = static_cast<nekoHWSound *>(GetResource(EHRT_SOUND, fileName));

		if(res)
		{
			res->AddRef();
			mLockSection.End();
			return res;
		}

		res = nekoNew nekoHWSound(this, fileName);

		if(FAILED(res->GetLastReturnCode()))
		{
			res->Release();
			mLockSection.End();
			return NULL;
		}

		res->AddRef();

		mLockSection.End();
		return res;
	}

	nekoHWFont *nekoHWResourceManager::LoadFont(const char8 *faceName, int32 width, int32 height, bool bold)
	{
		nekoHWFont *res = nekoNew nekoHWFont(this, faceName, width, height, bold);
		if(FAILED(res->GetLastReturnCode()))
		{
			res->Release();
			return NULL;
		}

		res->AddRef();
		return res;
	}

	nekoHWTexture *nekoHWResourceManager::LoadTexture(const char8 *fileName, uint32 colorKey, bool preserveSize)
	{
		mLockSection.Begin();

		nekoHWTexture *res = static_cast<nekoHWTexture *>(GetResource(EHRT_TEXTURE, fileName));

		if(res)
		{
			res->AddRef();
			mLockSection.End();
			return res;
		}

		res = nekoNew nekoHWTexture(this, fileName);
		res->SetColorKey(colorKey);
		res->SetPreserveSize(preserveSize);

		if(FAILED(res->GetLastReturnCode()))
		{
			res->Release();
			mLockSection.End();
			return NULL;
		}

		res->AddRef();
		mLockSection.End();
		return res;
	}

	nekoHWGIFImage *nekoHWResourceManager::LoadGIF(const char8 *fileName)
	{
		mLockSection.Begin();

		nekoHWGIFImage *res = static_cast<nekoHWGIFImage *>(GetResource(EHRT_GIFIMAGE, fileName));

		if(res)
		{
			res->AddRef();
			mLockSection.End();
			return res;
		}

		res = nekoNew nekoHWGIFImage(this, fileName);

		if(FAILED(res->GetLastReturnCode()))
		{
			res->Release();
			mLockSection.End();
			return NULL;
		}

		res->AddRef();
		mLockSection.End();
		return res;
	}

	nekoHWMusic *nekoHWResourceManager::LoadMusic(const char8 *fileName, uint32 repeat, float32 volume)
	{
		mLockSection.Begin();

		nekoHWMusic *res = nekoNew nekoHWMusic(this, fileName, repeat, volume);

		if(FAILED(res->GetLastReturnCode()))
		{
			res->Release();
			mLockSection.End();
			return NULL;
		}

		res->AddRef();
		mLockSection.End();
		return res;
	}

	/// Get Resource
	nekoIHWResource *nekoHWResourceManager::GetResource(E_HW_RESOURCE_TYPE type, const char8 *name)
	{
		mLockSection.Begin();

		list<nekoIHWResource *>::iterator iter = mManagedResourceList.begin();
		for(;iter != mManagedResourceList.end();++iter)
		{
			if(type == (*iter)->GetType() && (*iter)->IsSharable() &&
				stricmp((*iter)->GetName(), name) == 0)
			{
				mLockSection.End();
				return (*iter);
			}
		}

		mLockSection.End();
		return NULL;
	}

	/// Do something
	uint32 nekoHWResourceManager::Do()
	{
		while(mValid)
		{
			mLockSection.Begin();

			static uint32 lastTime = timeGetTime();
			float32 frameTime = (timeGetTime() - lastTime) * 0.001f;
			lastTime = timeGetTime();

			list<nekoIHWResource *>::iterator iter = mManagedResourceList.begin();
			for(;iter != mManagedResourceList.end();)
			{
				nekoIHWResource *res = (*iter++);

				if(res->GetState() == EHRS_LOADING)
					res->OnCatching();
				else if(res->GetState() == EHRS_UNLOADING)
					res->OnDropping();

				res->UpdateInThread(frameTime);

			}

			mLockSection.End();
			Sleep(1);
		}

		return 0;
	}

	void nekoHWResourceManager::OnDeviceLost()
	{
		mLockSection.Begin();

		list<nekoIHWResource *>::iterator iter = mManagedResourceList.begin();
		for(;iter != mManagedResourceList.end();++iter)
		{
			(*iter)->OnDeviceLost();
		}

		mLockSection.End();
	}

	void nekoHWResourceManager::OnDeviceReset()
	{
		mLockSection.Begin();

		list<nekoIHWResource *>::iterator iter = mManagedResourceList.begin();
		for(;iter != mManagedResourceList.end();++iter)
		{
			(*iter)->OnDeviceReset();
		}

		mLockSection.End();
	}
}