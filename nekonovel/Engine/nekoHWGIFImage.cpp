#include "nekoPrecompiledHeader.h"
#include "nekoHWGIFImage.h"
#include "nekoLogger.h"
#include "nekoHWResourceManager.h"
#include "nekoUtility.h"
#include "NekoNovel.h"

#define ALIGN sizeof(int)				// Windows GDI expects all int-aligned

namespace neko
{
#	pragma pack(1)

	struct nekoGIFColor { unsigned char b,g,r,x; };

	// Init: Allocates space for raster and palette in GDI-compatible structures.
	void nekoGIFImage::Init(int iWidth, int iHeight, int iBPP) 
	{
		// Standard members setup
		Transparent=-1;
		BytesPerRow = mWidth = iWidth; mHeight=iHeight; BPP=iBPP;
		// Animation Extra members setup:

		if (BPP==24)
		{
			BytesPerRow*=3;
		}
		else
		{
			Palette= nekoNew nekoGIFColor[(1<<BPP)*sizeof(nekoGIFColor)];
		}

		BytesPerRow += (ALIGN-mWidth%ALIGN) % ALIGN;	// Align BytesPerRow
		Raster = nekoNew char8[BytesPerRow*mHeight];
	}

	// ****************************************************************************
	// * SaveBMP                                                                  *
	// *   Save the content of a nekoGIFImage object into a BMP file                   *
	// *                        (c) Sept2000, Juan Soulie <jsoulie@cplusplus.com> *
	// ****************************************************************************
	result nekoGIFImage::OnReady()
	{
		int n;

		// determine BPP for file:
		int SaveBPP;
		if (BPP == 1) SaveBPP=1;
		else if (BPP <= 4) SaveBPP=4;
		else if (BPP <= 8) SaveBPP=8;
		else 
		{
			//SaveBPP=24;
			LogPrint(LOG_LEVEL_WARN, "24비트 GIF는 지원하지 않습니다.");
			return E_FAIL;
		}

		int PixelsPerByte = 8/SaveBPP;	//used only if BPP are less than 8
		int BitMask = (1<<SaveBPP)-1;	//used only if BPP are less than 8

		nekoPoint mVideoSize = nekoPoint(1, 1);
		while(mVideoSize.x <= mWidth) mVideoSize.x <<= 1;
		while(mVideoSize.y <= mHeight) mVideoSize.y <<= 1;

		if(FAILED(GetNekoNovel()->GetVideoDriver()->GetHandle()->CreateTexture(mVideoSize.x, mVideoSize.y,
			1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, mTexture.Pptr(), 0)))
		{
			LogPrint(LOG_LEVEL_WARN, "텍스쳐 생성 실패(크기 %d, %d)", mVideoSize.x, mVideoSize.y);
			return E_FAIL;
		}

		D3DLOCKED_RECT lockRect;
		if(FAILED(mTexture->LockRect(0, &lockRect, 0, 0)))
		{
			LogPrint(LOG_LEVEL_WARN, "D3DPOOL_MANAGED 텍스쳐 락 실패");
			return E_FAIL;
		}

		uint32 *color = (uint32 *)lockRect.pBits;
		uint32 *rc;

		// Raster Data Rows are 32bit aligned in BMP files.
		//int RowAlignmentInFile = ((4- ((Width*SaveBPP+7)/8)%4)%4); // (bytes)
		for (int row=0; row< mHeight; row++)
		{
			rc = color;

			uchar8 * pPixel;
			// If height is positive the bmp is bottom-up, set adequate row info:
			pPixel= (uchar8*) Raster + BytesPerRow * row ;

			if (SaveBPP >= 8)
			{ // 8 or more BPP: Save as block.
				//buf.Append(pPixel, Width*SaveBPP/8);
				for (int col=0; col < mWidth; col+=PixelsPerByte)
				{
					if((int32)*pPixel == this->Transparent)
						*rc++ = 0x00;
					else
						*rc++ = 0xff000000 | ((Palette[*pPixel].r) << 16) | ((Palette[*pPixel].g) << 8) | (Palette[*pPixel].b);
					pPixel++;
				}
			}
			else				// Less than 8BPP: Save packing bytes.
			{
				unsigned char charToPut;
				for (int col=0; col < mWidth; col+=PixelsPerByte)
				{
					for (int bit=8 ; bit >0 ; bit -= BPP)
					{
						if((int32)*pPixel == this->Transparent)
							*rc = 0x00;
						else
							*rc++ = 0xff000000 | (Palette[*pPixel].r << 16) | (Palette[*pPixel].g << 8) | (Palette[*pPixel].b);
						pPixel++;
					}
				}
			}
			// Ignore aligment bytes of file:
			//for (int m=0; m<RowAlignmentInFile; m++) buf << (char)0;

			color += lockRect.Pitch / 4;
		}

		mTexture->UnlockRect(0);
		SafeDeleteArray(Palette);

		return S_OK;
	}

	nekoHWGIFImage::nekoHWGIFImage(nekoHWResourceManager *resMgr, const char8 *fileName)
		: nekoIHWResource(resMgr, EHRT_GIFIMAGE)
		, mTransparentEnabled(false)
	{
		Catch();
		mName = fileName;
		mAutoDropping = false;
	}

	/// Destructor
	nekoHWGIFImage::~nekoHWGIFImage()
	{

	}
	// pre-declaration:
	int LZWDecoder (char*, char*, short, int, int, int, const int);

	result nekoHWGIFImage::_LoadGIF(const char8 *fileName)
	{
		int32 n, nPage;

		int32 GlobalBPP;						// Bits per Pixel.
		nekoGIFColor * GlobalColorMap;			// Global colormap (allocate)
		int32 FrameWidth, FrameHeight;

		struct GIFGCEtag {				// GRAPHIC CONTROL EXTENSION
			unsigned char BlockSize;		// Block Size: 4 bytes
			unsigned char PackedFields;		// Packed Fields. Bits detail:
			//    0: Transparent Color Flag
			//    1: User Input Flag
			//  2-4: Disposal Method
			unsigned short Delay;			// Delay Time (1/100 seconds)
			unsigned char Transparent;		// Transparent Color Index
		} gifgce;
		int GraphicExtensionFound = 0;

		char8 *fileData;
		int32 length;
		if(FAILED(GetNekoNovel()->GetFileSystem()->LoadData(mName.c_str(), &fileData, &length, true)))
		{
			LogPrint(LOG_LEVEL_WARN, "GIF 이미지 '%s' 파일이 존재하지 않습니다.", mName.c_str());
			return E_FAIL;
		}

		nekoByteBuffer buf;
		buf.AdoptBuffer(fileData, length);

		// *1* READ HEADER (SIGNATURE + VERSION)
		char8 szSignature[6];				// First 6 bytes (GIF87a or GIF89a)
		buf.Read(szSignature, 6);
		if(memcmp(szSignature,"GIF", 2) != 0)
		{
			nekoLogger::Log(LOG_LEVEL_WARN, "'%s' 파일은 올바른 GIF 이미지가 아닙니다.", fileName);
			return E_FAIL;
		}

		// *2* READ LOGICAL SCREEN DESCRIPTOR
		struct GIFLSDtag {
			unsigned short ScreenWidth;		// Logical Screen Width
			unsigned short ScreenHeight;	// Logical Screen Height
			unsigned char PackedFields;		// Packed Fields. Bits detail:
			//  0-2: Size of Global Color Table
			//    3: Sort Flag
			//  4-6: Color Resolution
			//    7: Global Color Table Flag
			unsigned char Background;		// Background Color Index
			unsigned char PixelAspectRatio;	// Pixel Aspect Ratio
		} giflsd;

		buf.Read(&giflsd, sizeof(GIFLSDtag));

		GlobalBPP = (giflsd.PackedFields & 0x07) + 1;

		FrameWidth = giflsd.ScreenWidth;
		FrameHeight = giflsd.ScreenHeight;

		// *3* READ/GENERATE GLOBAL nekoGIFColor MAP
		GlobalColorMap = nekoNew nekoGIFColor [1<<GlobalBPP];
		if (giflsd.PackedFields & 0x80)	// File has global color map?
			for (n=0;n< 1<<GlobalBPP;n++)
			{
				buf >> GlobalColorMap[n].r
					>> GlobalColorMap[n].g
					>> GlobalColorMap[n].b;
			}

		else	// GIF standard says to provide an internal default Palette:
			for (n=0;n<256;n++)
				GlobalColorMap[n].r=GlobalColorMap[n].g=GlobalColorMap[n].b=n;

		nPage = 0;

		// *4* NOW WE HAVE 3 POSSIBILITIES:
		//  4a) Get and Extension Block (Blocks with additional information)
		//  4b) Get an Image Separator (Introductor to an image)
		//  4c) Get the trailer Char (End of GIF File)
		int32 prevdispmeth = 0;
		do
		{
			int32 charGot = buf.Read<uchar8>();

			if (charGot == 0x21)		// *A* EXTENSION BLOCK 
			{
				switch (buf.Read<uchar8>())
				{

				case 0xF9:			// Graphic Control Extension

					buf.Read(&gifgce, sizeof(gifgce));
					GraphicExtensionFound++;
					buf.Read<uchar8>(); // Block Terminator (always 0)
					break;

				case 0xFE:			// Comment Extension: Ignored
				case 0x01:			// PlainText Extension: Ignored
				case 0xFF:			// Application Extension: Ignored
				default:			// Unknown Extension: Ignored
					// read (and ignore) data sub-blocks
					while (int nBlockLength = buf.Read<uchar8>())
						for (n=0;n<nBlockLength;n++) buf.Read<uchar8>();
					break;
				}
			}


			else if (charGot == 0x2c) {	// *B* IMAGE (0x2c Image Separator)

				// Create a new Image Object:
				nekoGIFImage* NextImage;
				NextImage = nekoNew nekoGIFImage;

				// Read Image Descriptor
				struct GIFIDtag {	
					unsigned short xPos;			// Image Left Position
					unsigned short yPos;			// Image Top Position
					unsigned short Width;			// Image Width
					unsigned short Height;			// Image Height
					unsigned char PackedFields;		// Packed Fields. Bits detail:
					//  0-2: Size of Local Color Table
					//  3-4: (Reserved)
					//    5: Sort Flag
					//    6: Interlace Flag
					//    7: Local Color Table Flag
				} gifid;

				buf.Read((char*)&gifid, sizeof(gifid));

				//?????????????????????????????????????????????????????????
				//int LocalColorMap = (gifid.PackedFields & 0x08)? 1 : 0;
				int LocalColorMap = (gifid.PackedFields & 0x80)? 1 : 0;
				//?????????????????????????????????????????????????????????

				NextImage->Init (gifid.Width, gifid.Height,
					LocalColorMap ? (gifid.PackedFields&7)+1 : GlobalBPP);

				// Fill NextImage Data
				NextImage->mX = gifid.xPos;
				NextImage->mY = gifid.yPos;
				if (GraphicExtensionFound)
				{
					NextImage->Transparent =
						((gifgce.PackedFields&0x01) ? gifgce.Transparent : -1);
					NextImage->Transparency=
						(((gifgce.PackedFields&0x1c)>1) ? 1 : 0);
					if(NextImage->Transparent >= 0)
						mTransparentEnabled = true;

					if(!gifgce.Delay)
					{
						NextImage->mDelay = 0.1f;
					}
					else
					{
						NextImage->mDelay = gifgce.Delay*10; 
						NextImage->mDelay /= 1000.0f;
					}
				}

				if (LocalColorMap)		// Read Color Map (if descriptor says so)
				{
					//buf.Read((char*)NextImage->Palette,
					//	sizeof(nekoGIFColor)*(1<<NextImage->BPP));
					//local table made of 3 bytes blocks (not 4)
					int nb=(1<<NextImage->BPP);
					for (int i=0; i<nb; i++)
					{
						buf >> NextImage->Palette[i].r
							>> NextImage->Palette[i].g
							>> NextImage->Palette[i].b;
					}
				}
				else					// Otherwise copy Global
				{
					memcpy (NextImage->Palette, GlobalColorMap,
						sizeof(nekoGIFColor)*(1<<NextImage->BPP));
				}

				short firstbyte=buf.Read<uchar8>();	// 1st byte of img block (CodeSize)

				// Calculate compressed image block size
				// to fix: this allocates an extra byte per block
				long ImgStart,ImgEnd;				
				ImgEnd = ImgStart = buf.GetReadPosition();
				while (n=buf.Read<uchar8>())
					buf.SetReadPosition(ImgEnd+=n+1);
				buf.SetReadPosition(ImgStart);

				// Allocate Space for Compressed Image
				char8 * pCompressedImage = nekoNew char8 [ImgEnd-ImgStart+4];

				// Read and store Compressed Image
				char * pTemp = pCompressedImage;
				while (int nBlockLength = buf.Read<uchar8>())
				{
					buf.Read(pTemp, nBlockLength);
					pTemp+=nBlockLength;
				}

				// Call LZW/GIF decompressor
				n=LZWDecoder(
					(char*) pCompressedImage,
					(char*) NextImage->Raster,
					firstbyte, NextImage->BytesPerRow,//NextImage->AlignedWidth,
					gifid.Width, gifid.Height,
					((gifid.PackedFields & 0x40)?1:0)	//Interlaced?
					);

				if (n)
				{
					if(NextImage->Transparency && nPage != 0)
						_ApplyTransparency(NextImage, NextImage->Transparent);
					if(SUCCEEDED(NextImage->OnReady()))
					{
						_AddImage(NextImage);
						nPage++;
					}
					else
					{
						delete NextImage;
					}
				}
				else
				{
					delete NextImage;
				}

				prevdispmeth = (gifgce.PackedFields >> 2) & 0x7;

				// Some cleanup
				delete[]pCompressedImage;
				GraphicExtensionFound=0;
			}


			else if (charGot == 0x3b) {	// *C* TRAILER: End of GIF Info
				break; // Ok. Standard End.
			}

		} while (!buf.IsReadEndPosition());

		// Add this line. 
		delete [] GlobalColorMap;

		mSize = nekoPoint(FrameWidth, FrameHeight);

		// 임시 메모리 제거
		list< nekoAutoPtr<nekoGIFImage> >::iterator iter = mImageList.begin();
		for(;iter != mImageList.end();++iter)
		{
			(*iter)->ClearRaster();
		}

		return S_OK;
	}

	void nekoHWGIFImage::_ApplyTransparency(nekoGIFImage *pImage, short nColorIndex)
	{
		nekoGIFImage * pFirst = (*mImageList.begin());
		BYTE    *lpSource = (BYTE *)pFirst->Raster;
		BYTE    *lpDest   = (BYTE *)pImage->Raster;
		short   i, j;

		lpSource += pFirst->BytesPerRow * pImage->mY;
		for (i=0; i<pImage->mHeight; i++)
		{
			for (j=0; j<pImage->mWidth; j++)
			{
				if (*(lpDest+j) == nColorIndex)
					*(lpDest+j) = *(lpSource+j+pImage->mX);
			}
			lpDest += pImage->BytesPerRow;
			lpSource += pFirst->BytesPerRow;
		}
	}

	/// 실제로 로드/언로드를 시도하는 메소드
	result nekoHWGIFImage::OnCatching()
	{
		mLockSeciton.Begin();

		if(mState != EHRS_LOADING)
		{
			mLockSeciton.End();
			return S_OK;
		}

		if(FAILED(_LoadGIF(mName.c_str())))
		{
			mState = EHRS_NONE;
			mLockSeciton.End();
			return (mLastReturnCode = E_FAIL_); // 오류 코드 수정 필요.
		}

		mState = EHRS_LOADED;
		mLockSeciton.End();

		return nekoIHWResource::OnCatching();
	}

	result nekoHWGIFImage::OnDropping()
	{
		mLockSeciton.Begin();

		if(mState != EHRS_LOADED)
		{
			mLockSeciton.End();
			return S_OK;
		}

		mImageList.clear();

		mLockSeciton.End();
		return nekoIHWResource::OnDropping();
	}

	// ****************************************************************************
	// * LZWDecoder (C/C++)                                                       *
	// * Codec to perform LZW (GIF Variant) decompression.                        *
	// *                         (c) Nov2000, Juan Soulie <jsoulie@cplusplus.com> *
	// ****************************************************************************
	//
	// Parameter description:
	//  - bufIn: Input buffer containing a "de-blocked" GIF/LZW compressed image.
	//  - bufOut: Output buffer where result will be stored.
	//  - InitCodeSize: Initial CodeSize to be Used
	//    (GIF files include this as the first byte in a picture block)
	//  - AlignedWidth : Width of a row in memory (including alignment if needed)
	//  - Width, Height: Physical dimensions of image.
	//  - Interlace: 1 for Interlaced GIFs.
	//
	int LZWDecoder (char * bufIn, char * bufOut,
		short InitCodeSize, int AlignedWidth,
		int Width, int Height, const int Interlace)
	{
		int n;
		int row=0,col=0;				// used to point output if Interlaced
		int nPixels, maxPixels;			// Output pixel counter

		short CodeSize;					// Current CodeSize (size in bits of codes)
		short ClearCode;				// Clear code : resets decompressor
		short EndCode;					// End code : marks end of information

		long whichBit;					// Index of next bit in bufIn
		long LongCode;					// Temp. var. from which Code is retrieved
		short Code;						// Code extracted
		short PrevCode;					// Previous Code
		short OutCode;					// Code to output

		// Translation Table:
		short Prefix[4096];				// Prefix: index of another Code
		unsigned char Suffix[4096];		// Suffix: terminating character
		short FirstEntry;				// Index of first free entry in table
		short NextEntry;				// Index of next free entry in table

		unsigned char OutStack[4097];	// Output buffer
		int OutIndex;					// Characters in OutStack

		int RowOffset;					// Offset in output buffer for current row

		// Set up values that depend on InitCodeSize Parameter.
		CodeSize = InitCodeSize+1;
		ClearCode = (1 << InitCodeSize);
		EndCode = ClearCode + 1;
		NextEntry = FirstEntry = ClearCode + 2;

		whichBit=0;
		nPixels = 0;
		maxPixels = Width*Height;
		RowOffset =0;

		while (nPixels<maxPixels) {
			OutIndex = 0;							// Reset Output Stack

			// GET NEXT CODE FROM bufIn:
			// LZW compression uses code items longer than a single byte.
			// For GIF Files, code sizes are variable between 9 and 12 bits 
			// That's why we must read data (Code) this way:
			LongCode=*((long*)(bufIn+whichBit/8));	// Get some bytes from bufIn
			LongCode>>=(whichBit&7);				// Discard too low bits
			Code =(LongCode & ((1<<CodeSize)-1) );	// Discard too high bits
			whichBit += CodeSize;					// Increase Bit Offset

			// SWITCH, DIFFERENT POSIBILITIES FOR CODE:
			if (Code == EndCode)					// END CODE
				break;								// Exit LZW Decompression loop

			if (Code == ClearCode) {				// CLEAR CODE:
				CodeSize = InitCodeSize+1;			// Reset CodeSize
				NextEntry = FirstEntry;				// Reset Translation Table
				PrevCode=Code;				// Prevent next to be added to table.
				continue;							// restart, to get another code
			}
			if (Code < NextEntry)					// CODE IS IN TABLE
				OutCode = Code;						// Set code to output.

			else {									// CODE IS NOT IN TABLE:
				OutIndex++;			// Keep "first" character of previous output.
				OutCode = PrevCode;					// Set PrevCode to be output
			}

			// EXPAND OutCode IN OutStack
			// - Elements up to FirstEntry are Raw-Codes and are not expanded
			// - Table Prefices contain indexes to other codes
			// - Table Suffices contain the raw codes to be output
			while (OutCode >= FirstEntry) {
				if (OutIndex > 4096)
					return 0;
				OutStack[OutIndex++] = Suffix[OutCode];	// Add suffix to Output Stack
				OutCode = Prefix[OutCode];				// Loop with preffix
			}

			// NOW OutCode IS A RAW CODE, ADD IT TO OUTPUT STACK.
			if (OutIndex > 4096)
				return 0;
			OutStack[OutIndex++] = (unsigned char) OutCode;

			// ADD NEW ENTRY TO TABLE (PrevCode + OutCode)
			// (EXCEPT IF PREVIOUS CODE WAS A CLEARCODE)
			if (PrevCode!=ClearCode) {
				Prefix[NextEntry] = PrevCode;
				Suffix[NextEntry] = (unsigned char) OutCode;
				NextEntry++;

				// Prevent Translation table overflow:
				if (NextEntry > 4096)
					return 0;

				// INCREASE CodeSize IF NextEntry IS INVALID WITH CURRENT CodeSize
				if (NextEntry >= (1<<CodeSize)) {
					if (CodeSize < 12) CodeSize++;
					else {}				// Do nothing. Maybe next is Clear Code.
				}
			}

			PrevCode = Code;

			// Avoid the possibility of overflow on 'bufOut'.
			if (nPixels + OutIndex > maxPixels) OutIndex = maxPixels-nPixels;

			// OUTPUT OutStack (LAST-IN FIRST-OUT ORDER)
			for (n=OutIndex-1; n>=0; n--) {
				if (col==Width)						// Check if new row.
				{
					if (Interlace) {				// If interlaced::
						if ((row&7)==0) {row+=8; if (row>=Height) row=4;}
						else if ((row&3)==0) {row+=8; if (row>=Height) row=2;}
						else if ((row&1)==0) {row+=4; if (row>=Height) row=1;}
						else row+=2;
					}
					else							// If not interlaced:
						row++;

					RowOffset=row*AlignedWidth;		// Set new row offset
					col=0;
				}
				bufOut[RowOffset+col]=OutStack[n];	// Write output
				col++;	nPixels++;					// Increase counters.
			}

		}	// while (main decompressor loop)

		return whichBit;
	}

	// Refer to WINIMAGE.TXT for copyright and patent notices on GIF and LZW.

#	pragma pack()
}