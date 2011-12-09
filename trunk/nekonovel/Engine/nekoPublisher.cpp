#include "nekoPrecompiledHeader.h"
#include "nekoPublisher.h"
#include "NekoNovel.h"
#include "nekoAutoPtr.h"
#include "nekoUtility.h"

namespace neko
{
	static const char8 *gPackageNames[][3] =
	{
		{"이미지\\*", "Images.nkpack", "이미지\\"},
		{"스크립트\\*", "Scripts.nkpack", "스크립트\\"},
		{"Command.xml", "Core.nkpack", ""},
		{"효과음\\*", "FX.nkpack", "효과음\\"},
		{"파티클\\*", "Particles.nkpack", "파티클\\"},
		{"보이스\\*", "Voice.nkpack", "보이스\\"},
		{"배경음악\\*", "BGM.nkpack", "배경음악\\"},
	};

	nekoPublisher::nekoPublisher()
	{

	}

	nekoPublisher::~nekoPublisher()
	{

	}

	// 패키징 하지 않는 파일들 복사..
	void CopyNoPackageFiles(string name)
	{
		string strSrc, strDest;
		HANDLE hFile;
		WIN32_FIND_DATA foundData;

		name = '\\' + name;
		name += '\\';

		strSrc = GetNekoNovel()->GetAppFolderPath();;
		strSrc += name; strSrc += '*';
		hFile = FindFirstFile(strSrc.c_str(), &foundData);

		if(hFile == NULL)
			return;

		strDest = GetNekoNovel()->GetCurrentGame()->GetEnvironment().mLastPublishFolder;
		strDest += name;
		CreateDirectory(strDest.c_str(), NULL);

		do 
		{
			if(foundData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;

			strSrc = GetNekoNovel()->GetAppFolderPath();
			strSrc += name;
			strSrc += foundData.cFileName;

			strDest = GetNekoNovel()->GetCurrentGame()->GetEnvironment().mLastPublishFolder;
			strDest += name;
			strDest += foundData.cFileName;
			CopyFile(strSrc.c_str(), strDest.c_str(), false);

		} while (FindNextFile(hFile, &foundData));

		FindClose(hFile);
	}

	// 로컬 파일들 복사
	void CopyLocalFiles(string format)
	{
		string strSrc, strDest;
		HANDLE hFile;
		WIN32_FIND_DATA foundData;

		strSrc = GetNekoNovel()->GetAppFolderPath();
		strSrc += format;
		hFile = FindFirstFile(strSrc.c_str(), &foundData);

		if(hFile == NULL)
			return;

		do 
		{
			if(foundData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;

			strSrc = GetNekoNovel()->GetAppFolderPath();
			strSrc += foundData.cFileName;

			strDest = GetNekoNovel()->GetCurrentGame()->GetEnvironment().mLastPublishFolder;
			strDest += '\\';
			strDest += foundData.cFileName;
			CopyFile(strSrc.c_str(), strDest.c_str(), false);

		} while (FindNextFile(hFile, &foundData));

		FindClose(hFile);
	}

	bool CopyToPublishRoot(const char8 *src, const char8 *dest)
	{
		string strSrc;
		string strDest;

		strSrc = GetNekoNovel()->GetAppFolderPath();
		strSrc += '\\'; strSrc += src;

		strDest = GetNekoNovel()->GetCurrentGame()->GetEnvironment().mLastPublishFolder;
		strDest += '\\'; strDest += dest;
		return (CopyFile(strSrc.c_str(), strDest.c_str(), false));
	}

	/// Do something
	uint32 nekoPublisher::Do()
	{
		nekoFileSystem *fs = GetNekoNovel()->GetFileSystem();

		float32 total = 5;
		string strSrc;
		string strDest;
		nekoEvent e(EVENT_PUBLISHING_STATUS);

		int32 i = 0;
		for(;i <= 5;++i)
		{
			// 중단 요청.
			if(mValid == false)
			{
				e.value[0] = -3;
				GetNekoNovel()->OnEvent(e);
				return -1;
			}

			strSrc = GetNekoNovel()->GetAppFolderPath();
			strSrc += "\\"; strSrc += gPackageNames[i][0];

			strDest = GetNekoNovel()->GetCurrentGame()->GetEnvironment().mLastPublishFolder;
			strDest += "\\"; strDest += gPackageNames[i][1];

			if(FAILED(nekoPackage::GeneratePackage(strSrc.c_str(), strDest.c_str(), gPackageNames[i][2])))
			{
				e.value[0] = -2;
				GetNekoNovel()->OnEvent(e);

				return -1;
			}

			e.value[0] = (int32)((i + 1) / total * 100);
			GetNekoNovel()->OnEvent(e);
		}

		// 중단 요청.
		if(mValid == false)
		{
			e.value[0] = -3;
			GetNekoNovel()->OnEvent(e);
			return -1;
		}

		// 동영상
		CopyNoPackageFiles("동영상");

		e.value[0] = (int32)80;
		GetNekoNovel()->OnEvent(e);

		// BGM도 압축.
		if(GetNekoNovel()->GetCurrentGame()->GetEnvironment().mCompressBGM)
		{
			strSrc = GetNekoNovel()->GetAppFolderPath();
			strSrc += "\\"; strSrc += gPackageNames[i][0];

			strDest = GetNekoNovel()->GetCurrentGame()->GetEnvironment().mLastPublishFolder;
			strDest += "\\"; strDest += gPackageNames[i][1];

			if(FAILED(nekoPackage::GeneratePackage(strSrc.c_str(), strDest.c_str(), gPackageNames[i][2])))
			{
				e.value[0] = -2;
				GetNekoNovel()->OnEvent(e);

				return -1;
			}

			e.value[0] = (int32)90;
			GetNekoNovel()->OnEvent(e);
		}
		else
		{
			// 배경음악 
			CopyNoPackageFiles("배경음악");

			e.value[0] = (int32)90;
			GetNekoNovel()->OnEvent(e);
		}

		// 중단 요청.
		if(mValid == false)
		{
			e.value[0] = -3;
			GetNekoNovel()->OnEvent(e);
			return -1;
		}

		// 외부폰트 파일 복사하기
		CopyLocalFiles("*.ttc");
		CopyLocalFiles("*.ttf");

		// 게임 파일 복사하기
		static const char8 *neededFiles[] = {"Game.exe", "Engine.dll", "audiere.dll", "d3dx9_40.dll", "Game.exe",
		/*"msvcm80.dll", */"msvcp80.dll", "msvcr80.dll", "Engine.dll.intermediate.manifest",
		"Microsoft.VC80.CRT.manifest", "Game.exe.intermediate.manifest", "BugTrap.dll", "Flash.ocx", 0};
		i = 0;
		while(neededFiles[i])
		{
			CopyToPublishRoot(neededFiles[i], neededFiles[i]);
			++i;
		}

		e.value[0] = 100;
		GetNekoNovel()->OnEvent(e);

		// 끝내기!
		e.value[0] = -1;
		GetNekoNovel()->OnEvent(e);
		return 0;
	}
}