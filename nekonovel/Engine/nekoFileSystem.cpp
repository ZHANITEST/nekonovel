#include "nekoPrecompiledHeader.h"
#include "nekoFileSystem.h"
#include "nekoLogger.h"
#include "nekoUtility.h"
#include "nekoNovel.h"
extern "C"
{
#	include <zlib.h>
};
#pragma comment(lib, "zlibstat.lib")

namespace neko
{
	nekoPackage::nekoPackage()
		: mFileHandle(NULL)
	{

	}

	nekoPackage::~nekoPackage()
	{
		if(mFileHandle)
			fclose(mFileHandle);
	}

	result nekoPackage::Load(const char8 *fileName)
	{
		return S_OK;
	}

	result nekoPackage::ReadFile(const char8 *fileName, char8 **data, int32 *length)
	{
		return E_NOT_SUPPORTED;
	}

	/// Generate a package
	result nekoPackage::GeneratePackage(const char8 *srcFolder, const char8 *destName, const char8 *virtualRootPath)
	{
		LogPrint(LOG_LEVEL_WARN, "'%s' 패키지 파일을 만들 수 없습니다!", destName);
		return E_FAIL;
	}

	/// Constructor
	nekoFileSystem::nekoFileSystem(E_FILESYSTEM_MODE mode)
		: mMode(mode)
	{

	}

	/// Destructor
	nekoFileSystem::~nekoFileSystem()
	{
		list<nekoPackage *>::iterator iter = mPackageList.begin();
		for(;iter != mPackageList.end();++iter)
			delete (*iter);
	}

	/// Load a package
	result nekoFileSystem::LoadPackage(const char8 *fileName)
	{
		nekoPackage *pk = nekoNew nekoPackage;
		result res;

		if(FAILED(res = pk->Load(fileName)))
		{
			pk->Release();
			return res;
		}

		mPackageList.push_back(pk);
		return S_OK;
	}

	/// Get package
	nekoPackage *nekoFileSystem::GetPackage(const char8 *fileName)
	{
		list<nekoPackage *>::iterator iter = mPackageList.begin();
		for(;iter != mPackageList.end();++iter)
		{
			if(stricmp((*iter)->GetName(), fileName) == 0)
				return (*iter);
		}

		return NULL;
	}

	/// Load Data.
	result nekoFileSystem::LoadData(const char8 *fileName, char8 **fileData, int32 *length, bool noError)
	{
		int32 len = -1;
		char8 *ptr = 0;

		result res;
		switch(mMode)
		{
		case EFM_PACKAGE:
			{
				list<nekoPackage *>::iterator iter = mPackageList.begin();
				for(;iter != mPackageList.end();++iter)
				{
					if(SUCCEEDED(res = (*iter)->ReadFile(fileName, &ptr, &len)))
					{
						break;
					}
				}

				if(SUCCEEDED(res))
					break;
			}

		case EFM_LOCALFILE:
			{
				string str = GetNekoNovel()->GetAppFolderPath();
				FILE *fp = fopen((str + fileName).c_str(), "rb");
				if(!fp)
				{
					if(!noError)
						LogPrint(LOG_LEVEL_WARN, "'%s' 파일이 존재하지 않습니다.", fileName);
					return E_NOT_EXIST;
				}

				fseek(fp, 0, SEEK_END);
				len = ftell(fp);
				fseek(fp, 0, SEEK_SET);

				ptr = nekoNew char8[len + 1];
				ptr[len] = '\0';
				fread(ptr, 1, len, fp);

				fclose(fp);
			}
			break;

		default:
			return E_NOT_SUPPORTED;
		}

		if(ptr == NULL)
			return E_NOT_EXIST;

		*fileData = ptr;
		if(length)
			*length = len;
		
		return S_OK;
	}

}