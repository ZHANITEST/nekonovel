#include "nekoPrecompiledHeader.h"
#include "nekoMovie.h"
#include "nekoNovelReal.h"

#ifdef _DEBUG
#	pragma comment(lib, "strmbasd.lib")
#else
#	pragma comment(lib, "strmbase.lib")
#endif

namespace neko
{
	//-----------------------------------------------------------------------------
	// CTextureRenderer constructor
	//-----------------------------------------------------------------------------
	CTextureRenderer::CTextureRenderer(LPUNKNOWN pUnk, HRESULT *phr, nekoHWTexture *targetTexture, nekoMovieInstance *moveInst)
		: CBaseVideoRenderer(__uuidof(CLSID_TextureRenderer)
		, NAME("Texture Renderer"), pUnk, phr)
		//, m_bUseDynamicTextures(FALSE)
		, mTargetTexture(targetTexture)
		, mMovieInst(moveInst)
	{

	}


	//-----------------------------------------------------------------------------
	// CTextureRenderer destructor
	//-----------------------------------------------------------------------------
	CTextureRenderer::~CTextureRenderer()
	{
		// Do nothing
	}


	//-----------------------------------------------------------------------------
	// CheckMediaType: This method forces the graph to give us an R8G8B8 video
	// type, making our copy to texture memory trivial.
	//-----------------------------------------------------------------------------
	HRESULT CTextureRenderer::CheckMediaType(const CMediaType *pmt)
	{
		HRESULT   hr = E_FAIL;
		VIDEOINFO *pvi=0;

		if(!pmt)
			return E_FAIL;

		// Reject the connection if this is not a video type
		if( *pmt->FormatType() != FORMAT_VideoInfo ) {
			return E_INVALIDARG;
		}

		// Only accept RGB24 video
		pvi = (VIDEOINFO *)pmt->Format();

		if(IsEqualGUID( *pmt->Type(),    MEDIATYPE_Video)  &&
			IsEqualGUID( *pmt->Subtype(), MEDIASUBTYPE_RGB24))
		{
			hr = S_OK;
		}

		return hr;
	}

	//-----------------------------------------------------------------------------
	// SetMediaType: Graph connection has been made.
	//-----------------------------------------------------------------------------
	HRESULT CTextureRenderer::SetMediaType(const CMediaType *pmt)
	{
		HRESULT hr;

		// Retrive the size of this media type
		D3DCAPS9 caps;
		VIDEOINFO *pviBmp;                      // Bitmap info header
		pviBmp = (VIDEOINFO *)pmt->Format();

		m_lVidWidth  = pviBmp->bmiHeader.biWidth;
		m_lVidHeight = abs(pviBmp->bmiHeader.biHeight);
		m_lVidPitch  = (m_lVidWidth * 3 + 3) & ~(3); // We are forcing RGB24

		/*
		// here let's check if we can use dynamic textures
		ZeroMemory( &caps, sizeof(D3DCAPS9));
		hr = g_pd3dDevice->GetDeviceCaps( &caps );
		if( caps.Caps2 & D3DCAPS2_DYNAMICTEXTURES )
		{
		m_bUseDynamicTextures = TRUE;
		}
		*/

		return S_OK;
	}


	//-----------------------------------------------------------------------------
	// DoRenderSample: A sample has been delivered. Copy it to the texture.
	//-----------------------------------------------------------------------------
	HRESULT CTextureRenderer::DoRenderSample( IMediaSample * pSample )
	{
		BYTE  *pBmpBuffer, *pTxtBuffer; // Bitmap buffer, texture buffer
		LONG  lTxtPitch;                // Pitch of bitmap, texture

		BYTE  * pbS = NULL;
		DWORD * pdwS = NULL;
		DWORD * pdwD = NULL;
		UINT row, col, dwordWidth;

		CheckPointer(pSample,E_POINTER);

		// Get the video bitmap buffer
		pSample->GetPointer( &pBmpBuffer );

		// Lock the Texture
		D3DLOCKED_RECT d3dlr;
		/*
		if( m_bUseDynamicTextures )
		{
		if( FAILED(g_pTexture->LockRect(0, &d3dlr, 0, D3DLOCK_DISCARD)))
		return E_FAIL;
		}
		else
		{
		if (FAILED(g_pTexture->LockRect(0, &d3dlr, 0, 0)))
		return E_FAIL;
		}
		*/
		mTargetTexture->Refresh();

		if(!mTargetTexture->GetHandle())
			return E_FAIL;

		if (FAILED(mTargetTexture->GetHandle()->LockRect(0, &d3dlr, 0, 0)))
			return E_FAIL;

		// Get the texture buffer & pitch
		pTxtBuffer = static_cast<byte *>(d3dlr.pBits);
		lTxtPitch = d3dlr.Pitch;

		dwordWidth = m_lVidWidth / 4;

		for( row = 0; row< (UINT)m_lVidHeight; row++)
		{
			pdwS = ( DWORD*)pBmpBuffer;
			pdwD = ( DWORD*)pTxtBuffer + dwordWidth * 4 + (m_lVidWidth % 4) - 4;

			for( col = 0; col < dwordWidth; col ++ )
			{
				pdwD[3] =  pdwS[0] | 0xFF000000;
				pdwD[2] = ((pdwS[1]<<8)  | 0xFF000000) | (pdwS[0]>>24);
				pdwD[1] = ((pdwS[2]<<16) | 0xFF000000) | (pdwS[1]>>16);
				pdwD[0] = 0xFF000000 | (pdwS[2]>>8);
				pdwD -=4;
				pdwS +=3;
			}

			pdwD --;

			// we might have remaining (misaligned) bytes here
			pbS = (BYTE*) pdwS;
			for( col = 0; col < (UINT)m_lVidWidth % 4; col++)
			{
				*pdwD = 0xFF000000     |
					(pbS[2] << 16) |
					(pbS[1] <<  8) |
					(pbS[0]);
				pdwD--;
				pbS += 3;
			}

			pBmpBuffer  += m_lVidPitch;
			pTxtBuffer += lTxtPitch;
		}// for rows

		// Unlock the Texture
		if (FAILED(mTargetTexture->GetHandle()->UnlockRect(0)))
			return E_FAIL;

		return S_OK;
	}

	nekoMovieInstance::nekoMovieInstance(nekoIBlock *block)
		: nekoIBlockInstance(block)
		, mTextureRenderer(0)
		, mSkipAbleTime(0)
	{

	}

	nekoMovieInstance::~nekoMovieInstance()
	{
		if (m_pMC) m_pMC->Stop();

		if (m_pBA) m_pBA.Release();
		if (m_pMC) m_pMC.Release();
		if (m_pME) m_pME.Release();
		if (m_pMP) m_pMP.Release();
		if (m_pGB) m_pGB.Release();
		mTargetTexture = 0;

		if(mTextureRenderer) mTextureRenderer->Release();
	}

	/// Set Movie
	result nekoMovieInstance::SetMovie(const char8 *fileName, float32 skipAbleTime)
	{
		CComPtr<IBaseFilter> pFSrc;
		CComPtr<IPin>        pFSrcPinOut;    // Source Filter Output Pin
		HRESULT hr;

		// 이미 있다면 정리하기
		if (m_pMC) m_pMC->Stop();

		if (m_pBA) m_pBA.Release();
		if (m_pMC) m_pMC.Release();
		if (m_pME) m_pME.Release();
		if (m_pMP) m_pMP.Release();
		if (m_pGB) m_pGB.Release();
		mTargetTexture = 0;

		if(mTextureRenderer) mTextureRenderer->Release();


		mFileName = (string("동영상\\") + fileName);

		// Create the filter graph
		if(FAILED(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (VOID **)&m_pGB)))
			return E_FAIL;

		mTargetTexture = nekoNew nekoHWTexture(GetNekoNovel()->GetHWResourceManager(), mBoundingBox.GetWidth(), mBoundingBox.GetHeight(), true);

		// Create the Texture Renderer object
		mTextureRenderer = nekoNew CTextureRenderer(0, &hr, mTargetTexture, this);

		if(FAILED(hr))
		{
			nekoLogger::Log(LOG_LEVEL_WARN, "동영상 재생을 위한 텍스처 렌더러 초기화에 실패하였습니다.");
			return E_FAIL;
		}

		if(FAILED(m_pGB->AddFilter(mTextureRenderer, L"TEXTURERENDERER")))
		{
			nekoLogger::Log(LOG_LEVEL_WARN, "동영상 재생을 위한 텍스쳐 렌더러 필터를 활성화 할 수 없습니다.");
			return E_FAIL;
		}

		mTextureRenderer->AddRef();

		if(FAILED(m_pGB->AddSourceFilter(CComBSTR(mFileName.c_str()).m_str, L"SOURCE", &pFSrc)))
		{
			nekoLogger::Log(LOG_LEVEL_WARN, "동영상 파일 '%s' 을 불러올 수 없습니다.", mFileName.c_str());
			return E_FAIL;
		}


		if (FAILED(hr = pFSrc->FindPin(L"1", &pFSrcPinOut))
			&& FAILED(hr = pFSrc->FindPin(L"Out", &pFSrcPinOut))
			&& FAILED(hr = pFSrc->FindPin(L"Output", &pFSrcPinOut)))
		{
			nekoLogger::Log(LOG_LEVEL_WARN, "출력 핀을 찾을 수 없습니다.");
			return hr;
		}

		if (FAILED(hr = m_pGB->Render(pFSrcPinOut)))
		{
			nekoLogger::Log(LOG_LEVEL_WARN, "동영상을 렌더링 할 수 없습니다.");
			return hr;
		}

		m_pGB.QueryInterface(&m_pMC);
		m_pGB.QueryInterface(&m_pMP);
		m_pGB.QueryInterface(&m_pME);
		m_pGB.QueryInterface(&m_pBA);

		// Start the graph running;
		if (FAILED(m_pMC->Run()))
		{
			nekoLogger::Log(LOG_LEVEL_WARN, "DirectShow 그래프를 실행 할 수 없습니다.");
			return E_FAIL;
		}
		
		mAlpha = -1;
		mSkipAbleTime = skipAbleTime;
		SetAlpha(GetNekoNovel()->GetCurrentGame()->GetEnvironment().mDefFXVolume / 100.0f * 255.0f);

		mTargetTexture->Refresh();
		return S_OK;
	}

	void nekoMovieInstance::CheckMovieStatus()
	{
		long lEventCode;
		LONG_PTR lParam1, lParam2;
		HRESULT hr;

		if (!m_pME)
			return;

		// Check for completion events
		hr = m_pME->GetEvent(&lEventCode, &lParam1, &lParam2, 0);
		if (SUCCEEDED(hr))
		{
			// If we have reached the end of the media file, reset to beginning
			if (EC_COMPLETE == lEventCode)
			{
				m_pMC->Stop();
			}

			// Free any memory associated with this event
			hr = m_pME->FreeEventParams(lEventCode, lParam1, lParam2);
		}
	}

	void nekoMovieInstance::Play()
	{
		if(m_pMC)
		{
			m_pMC->Run();
		}
	}

	void nekoMovieInstance::Stop(bool pause)
	{
		if(m_pMC)
		{
			if(pause)
				m_pMC->Pause();
			else
				m_pMC->Stop();
		}
	}

	void nekoMovieInstance::Update(float32 frameTime)
	{
		if(mSkipAbleTime > 0.0f)
		{
			mSkipAbleTime = max(mSkipAbleTime - frameTime, 0);
		}

		CheckMovieStatus();
	}

	void nekoMovieInstance::Draw()
	{
		if(!IsVisible() || !m_pMP)
			return;

		nekoVideoDriver *videoDrv = GetNekoNovel()->GetVideoDriver();

		float32 addZOrder = (GetZOrder() - 200) * 0.0001f;

		if(mFlags & EBIF_IGNORE_CAMERA)
			videoDrv->EnableCamera(false);

		videoDrv->DrawImage(mTargetTexture, mBoundingBox.left, mBoundingBox.top, mBoundingBox.GetWidth(), mBoundingBox.GetHeight(), 
			0xffffffff, 0, 0, mTextureRenderer->m_lVidWidth, mTextureRenderer->m_lVidHeight, mRotation + MATH_PI, 1, 1, 0.6f + addZOrder);


		if(mFlags & EBIF_IGNORE_CAMERA)
			videoDrv->EnableCamera(true);
	}

	/// 데이터 덤프하기
	result nekoMovieInstance::Dump(int32 version, nekoByteBuffer &outBuf)
	{
		nekoIBlockInstance::Dump(version, outBuf);

		outBuf << mFileName
			<< mSkipAbleTime;

		return S_OK;
	}

	result nekoMovieInstance::Load(int32 version, nekoByteBuffer &inBuf)
	{
		nekoIBlockInstance::Load(version, inBuf);
		float32 skipAbleTime = 0.0f;

		inBuf >> mFileName;

		if(version >= 1125)
			inBuf >> skipAbleTime;

		if(!mFileName.empty())
		{
			SetMovie(mFileName.c_str(), skipAbleTime);
		}

		// 꼼수...
		int32 alpha = mAlpha;
		mAlpha = -1;
		SetAlpha(alpha);

		return S_OK;
	}

	/// Constructor
	nekoMovie::nekoMovie()
		: nekoIBlock(BLOCK_MOVIE)
	{

	}

	/// Destructor
	nekoMovie::~nekoMovie()
	{

	}

	/// 인스턴스를 만든다
	nekoIBlockInstance *nekoMovie::CreateInstance()
	{
		return nekoNew nekoMovieInstance(this);
	}
}