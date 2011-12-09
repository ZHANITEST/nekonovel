#include "nekoPrecompiledHeader.h"
#include "NekoNovel.h"
#include "nekoNovelReal.h"

namespace neko
{
	static NekoNovel *gNekoNovel = NULL;

	NekoNovel *GetNekoNovel()
	{
		if(gNekoNovel == NULL)
			return (gNekoNovel = nekoNew nekoNovelReal);
		return gNekoNovel;
	}

	void ReleaseNekoNovel()
	{
		if(gNekoNovel == NULL)
			return;

		SafeRelease(gNekoNovel);
	}
}