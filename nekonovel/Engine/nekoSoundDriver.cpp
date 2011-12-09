#include "nekoPrecompiledHeader.h"
#include "nekoSoundDriver.h"
#include "nekoLogger.h"

namespace neko
{
	/// Constructor
	nekoSoundDriver::nekoSoundDriver()
	{

	}

	/// Destructor
	nekoSoundDriver::~nekoSoundDriver()
	{
		mDevice = NULL;
	}

	/// Initialize
	result nekoSoundDriver::Initialize()
	{
		/// 오디오 초기화
		mDevice = OpenDevice();
		if(mDevice == 0)
		{
			LogPrint(LOG_LEVEL_WARN, "사운드 드라이버 초기와 실패하였습니다.");
			return E_FAIL;
		}

		LogPrint(LOG_LEVEL_INFO, "사운드 시스템이 정상적으로 시작되었어요!");

		return S_OK;
	}

	/// Update
	void nekoSoundDriver::Update(float32 frameTime)
	{
		if(mDevice == NULL)
			return;

		mLockSection.Begin();
		mDevice->update();
		mLockSection.End();
	}
}