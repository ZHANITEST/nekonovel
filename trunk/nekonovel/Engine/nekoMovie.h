// ===============================================================
//  nekoMovie - 2009.7.7
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoMovie_h__
#define __nekoMovie_h__

#include <atlbase.h>
#include <atlwin.h>
#include <streams.h>

#include "nekoIBlock.h"
#include "nekoIBlockInstance.h"
#include "nekoHWTexture.h"

namespace neko
{
	class nekoMovieInstance;

	//-----------------------------------------------------------------------------
	// Define GUID for Texture Renderer
	// {71771540-2017-11cf-AE26-0020AFD79767}
	//-----------------------------------------------------------------------------
	struct __declspec(uuid("{71771540-2017-11cf-ae26-0020afd79767}")) CLSID_TextureRenderer;

	//-----------------------------------------------------------------------------
	// CTextureRenderer Class Declarations
	//-----------------------------------------------------------------------------
	class CTextureRenderer : public CBaseVideoRenderer
	{
	public:
		CTextureRenderer(LPUNKNOWN pUnk, HRESULT *phr, nekoHWTexture *targetTexture, nekoMovieInstance *moveInst);
		~CTextureRenderer();

	public:
		HRESULT CheckMediaType(const CMediaType *pmt );     // Format acceptable?
		HRESULT SetMediaType(const CMediaType *pmt );       // Video format notification
		HRESULT DoRenderSample(IMediaSample *pMediaSample); // New video sample

		BOOL m_bUseDynamicTextures;
		LONG m_lVidWidth;   // Video width
		LONG m_lVidHeight;  // Video Height
		LONG m_lVidPitch;   // Video Pitch
		nekoHWTexture *mTargetTexture;

		nekoMovieInstance *mMovieInst;
	};

	class nekoMovieInstance : public nekoIBlockInstance
	{
	protected:

		nekoAutoPtr<nekoHWTexture> mTargetTexture;
		CTextureRenderer *mTextureRenderer;
		CComPtr<IGraphBuilder>  m_pGB;          // GraphBuilder
		CComPtr<IMediaControl>  m_pMC;          // Media Control
		CComPtr<IMediaPosition> m_pMP;          // Media Position
		CComPtr<IMediaEvent>    m_pME;          // Media Event
		CComPtr<IBasicAudio>	m_pBA;			// Baisc Audio
		string mFileName;
		float32 mSkipAbleTime;

	public:

		nekoMovieInstance(nekoIBlock *block);
		virtual ~nekoMovieInstance();

		inline bool IsPlaying() { return (mTextureRenderer && !mTextureRenderer->IsStopped()); }
		result SetMovie(const char8 *fileName, float32 skipAbleTime);
		void CheckMovieStatus();
		void Play();
		void Stop(bool pause = false);

		void SetAlpha(int32 alpha)
		{
			if(mAlpha == alpha)
				return;

			mAlpha = min(max(alpha, 0), 255);

			if(m_pBA)
			{
				m_pBA->put_Volume(-(255 - mAlpha)  / 255.0f * 10000);
			}
		}

		result Dump(int32 version, nekoByteBuffer &outBuf);
		result Load(int32 version, nekoByteBuffer &inBuf);

		void Update(float32 frameTime);
		void Draw();

		inline bool IsSkipAble() const
		{
			if(IsFloatZero(mSkipAbleTime))
				return true;

			if(mSkipAbleTime < 0.0f)
				return false;

			return false;
		}
	};

	class nekoMovie : public nekoIBlock
	{
	protected:


	public:
		
		nekoMovie();
		virtual ~nekoMovie();

		/// 인스턴스를 만든다
		nekoIBlockInstance *CreateInstance();
	
	};
}


#endif // __nekoMovie_h__