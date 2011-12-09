// ===============================================================
//  nekoFileSystem - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoFileSystem_h__
#define __nekoFileSystem_h__

#include <list>
using namespace std;
#include "nekoLockSection.h"
#include "nekoIUnknown.h"

namespace neko
{
#	define PACKAGE_CODENAME "NEKONOVEL PACKAGE"
	enum E_FILESYSTEM_MODE
	{
		EFM_LOCALFILE = 0,
		EFM_PACKAGE,
	};

	struct nekoPackageNode
	{
		string mName;

		uint32 mOffset;
		uint32 mOriginalSize;
		uint32 mCompressedSize;

		nekoPackageNode() {}
		~nekoPackageNode() {}
	};

	class nekoPackage : public nekoIUnknown
	{
	protected:

		string mName;
		list<nekoPackageNode> mNodeList;
		FILE *mFileHandle;
		nekoLockSection mLockSection;

	public:

		nekoPackage();
		~nekoPackage();

		result Load(const char8 *fileName);

		inline const char8 *GetName() { return mName.c_str(); }

		result ReadFile(const char8 *fileName, char8 **data, int32 *length);

		/// Get node list
		inline list<nekoPackageNode> &GetNodeList() { return mNodeList; }

		/// Generate a package
		static result GeneratePackage(const char8 *srcFolder, const char8 *destName, const char8 *virtualRootPath);
	};

	class nekoFileSystem : public nekoIUnknown
	{
	protected:

		list<nekoPackage *> mPackageList;
		E_FILESYSTEM_MODE mMode;

	public:
		
		/// Constructor
		nekoFileSystem(E_FILESYSTEM_MODE mode);
		
		/// Destructor
		~nekoFileSystem();

		/// Load a package
		result LoadPackage(const char8 *fileName);

		/// Get package
		nekoPackage *GetPackage(const char8 *fileName);

		/// Load Data.
		result LoadData(const char8 *fileName, char8 **fileData, int32 *length = NULL, bool noError = false);

		inline E_FILESYSTEM_MODE GetMode() { return mMode; }
		
	};
}


#endif // __nekoFileSystem_h__