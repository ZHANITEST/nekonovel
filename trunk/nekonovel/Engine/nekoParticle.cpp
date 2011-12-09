#include "nekoPrecompiledHeader.h"
#include "nekoParticle.h"
#include "nekoNovelReal.h"
#include <tinyxml.h>
#include "nekoUtility.h"

namespace neko
{
	inline float32 RandBetween(float32 minV, float32 maxV)
	{
		if(IsFloatSame(minV, maxV))
			return minV;

		return ((maxV-minV)*((float)rand()/RAND_MAX))+minV;
	}

	void nekoParticleNode::Revive(const nekoParticleEmitter &e)
	{
		// 재생성!
		alive = true;
		pos = nekoPointF(e.mSpawnPos.x + 
			((e.mSpawnRadius.x) ? (rand() % (e.mSpawnRadius.x * 2)) : 0) - e.mSpawnRadius.x, 
			e.mSpawnPos.y + 
			((e.mSpawnRadius.y) ? (rand() % (e.mSpawnRadius.y * 2)) : 0) - e.mSpawnRadius.y);
		dir.x = RandBetween(e.mMinDir.x, e.mMaxDir.x);
		dir.y = RandBetween(e.mMinDir.y, e.mMaxDir.y);
		lifetime = RandBetween(e.mMinLifeTime, e.mMaxLifeTime);

		r = RandBetween((e.mMinStartColor >> 16) & 0xff, (e.mMaxStartColor >> 16) & 0xff);
		g = RandBetween((e.mMinStartColor >> 8) & 0xff, (e.mMaxStartColor >> 8) & 0xff);
		b = RandBetween((e.mMinStartColor >> 0) & 0xff, (e.mMaxStartColor >> 0) & 0xff);
		a = RandBetween((e.mMinStartColor >> 24) & 0xff, (e.mMaxStartColor >> 24) & 0xff);

		er = RandBetween((e.mMinEndColor >> 16) & 0xff, (e.mMaxEndColor >> 16) & 0xff);
		eg = RandBetween((e.mMinEndColor >> 8) & 0xff, (e.mMaxEndColor >> 8) & 0xff);
		eb = RandBetween((e.mMinEndColor >> 0) & 0xff, (e.mMaxEndColor >> 0) & 0xff);
		ea = RandBetween((e.mMinEndColor >> 24) & 0xff, (e.mMaxEndColor >> 24) & 0xff);

		if(lifetime > 0)
		{
			vr = (er - r) / lifetime;
			vg = (eg - g) / lifetime;
			vb = (eb - b) / lifetime;
			va = (ea - a) / lifetime;
		}
		else
		{
			vr = vg = vb = va = 0.0f;
		}

		delta = 0;

		scale = RandBetween(e.mMinScale, e.mMaxScale);
		//if(IsFloatZero(dir.GetLength()))
			rotation = e.mRotation;
		/*else
			rotation = acos(-dir.y / dir.GetLength());*/
		rotSpeed = e.mRotationSpeed / 180.0f * MATH_PI;
		scaleSpeed = e.mScaleSpeed;
		deltaColor = 0;
		eventFlags = 0;
		gravity = e.mGravity;
		enabled = true;
	}

	void nekoParticleEmitter::Initialize()
	{
		mEmitDelay = 1.0f;
		mMinDir = nekoPointF(-100, -100);
		mMaxDir = nekoPointF(100, 100);
		mMinScale = 0.2f;
		mMaxScale = 0.5f;
		mMaxCount = 100;
		mLoop = true;
		mSpawnPos = nekoPointF(400, 300);
		mSpawnRadius = nekoPoint(100, 100);
		mMinLifeTime = 5.0f;
		mMaxLifeTime = 10.0f;
		mMinStartColor = 0x00;
		mMaxStartColor = 0xffffffff;
		mMinEndColor = 0x00;
		mMaxEndColor = 0x00ffffff;
		mRotation = 0.0f;
		mGravity = nekoPointF(0, 0);
		mSrcBlend = 3;
		mDestBlend = 2;
		mRotationSpeed = mScaleSpeed = 0;
		mMinFirstAppearTime = 0.0f;
		mMaxFirstAppearTime = 1.0f;

		FillNodes();
	}

	void nekoParticleEmitter::FillNodes()
	{
		SafeDeleteArray(mNodeList);

		nekoParticleNode node;
		node.lifetime = 0.0f;

		if(mMaxCount > 0)
		{
			mNodeList = nekoNew nekoParticleNode[mMaxCount];
			/*if(mLoop)
			{*/
				for(uint32 i = 0;i < mMaxCount;++i)
				{
					mNodeList[i].delta = 0.0f;
					mNodeList[i].enabled = true;
					mNodeList[i].alive = false;
					mNodeList[i].lifetime = RandBetween(mMinFirstAppearTime, mMaxFirstAppearTime);
				}
				
			/*}
			else
			{
				for(uint32 i = 0;i < mMaxCount;++i)
					mNodeList[i].Revive(*this);
			}*/
		}
	}

	void nekoParticleEmitter::SetTexture(const char8 *name)
	{
		if(name == NULL)
		{
			mTexture = NULL;
			return;
		}

		mTexture = GetNekoNovel()->GetHWResourceManager()->LoadTexture((string("파티클\\") + name).c_str(), 0xff000000, true);
	}

	nekoParticleEvent *nekoParticleEmitter::GetEvent(int32 index)
	{
		list<nekoParticleEvent>::iterator iter = mEventList.begin();
		int32 i = 0;
		for(;iter != mEventList.end();++iter,++i)
		{
			if(i == index)
				return &(*iter);
		}

		return NULL;
	}

	bool nekoParticleEmitter::RemoveEvent(int32 index)
	{
		list<nekoParticleEvent>::iterator iter = mEventList.begin();
		int32 i = 0;
		for(;iter != mEventList.end();++iter,++i)
		{
			if(i == index)
			{
				mEventList.erase(iter);
				return true;
			}
		}

		return false;
	}

	//// 여기부터는 파티클 인스턴스 입니다...

	nekoParticleInstance::nekoParticleInstance(nekoIBlock *block)
		: nekoIBlockInstance(block)
		, mForUseOnly(false)
	{

	}

	nekoParticleInstance::~nekoParticleInstance()
	{
		mEmitters.clear();
	}

	nekoParticleEmitter *nekoParticleInstance::AddEmitter(const char8 *name)
	{
		mEmitters.push_back(nekoParticleEmitter());
		nekoParticleEmitter &emitter = (*--mEmitters.end());

		emitter.mName = name;
		emitter.Initialize();

		return &emitter;
	}

	nekoParticleEmitter *nekoParticleInstance::GetEmitter(int32 index)
	{
		list<nekoParticleEmitter>::iterator iter = mEmitters.begin();
		int32 i = 0;
		for(;iter != mEmitters.end();++iter,++i)
		{
			if(i == index)
				return &(*iter);
		}

		return NULL;
	}

	bool nekoParticleInstance::RemoveEmitter(int32 index)
	{
		list<nekoParticleEmitter>::iterator iter = mEmitters.begin();
		int32 i = 0;
		for(;iter != mEmitters.end();++iter,++i)
		{
			if(i == index)
			{
				mEmitters.erase(iter);
				return true;
			}
		}

		return false;
	}

	result nekoParticleInstance::LoadParticle(const char8 *fileName, bool forUseOnly)
	{
		char8 *fileData;

		TiXmlDocument doc;

		if(forUseOnly)
		{
			if(FAILED(GetNekoNovel()->GetFileSystem()->LoadData( (string("파티클\\") + fileName).c_str(), &fileData)))
				return E_NOT_EXIST;
			doc.Parse(fileData);

			// 메모리 없애기..
			delete []fileData;

			mForUseOnly = true;
		}
		else
		{
			doc.LoadFile(fileName);
		}
		mFileName = fileName;

		if(doc.Error())
		{
			LogPrint(LOG_LEVEL_WARN, "'%s' 파일은 XML 구문이 잘못되었습니다.", fileName);
			return E_FAIL;
		}

		TiXmlElement *root = doc.FirstChildElement("NekoNovel_Particle");
		if(!root)
		{
			LogPrint(LOG_LEVEL_WARN, "'%s' 파일에 파티클 정보가 존재하지 않습니다.", fileName);
			return E_FAIL;
		}

		Clear();

		TiXmlElement *emitter = root->FirstChildElement("Emitter");
		TiXmlElement *events;
		TiXmlElement *event;
		while(emitter)
		{
			nekoParticleEmitter *emt = AddEmitter(emitter->Attribute("Name"));

			emt->mEmitDelay = atof(emitter->Attribute("EmitDelay"));
			emt->mMinDir.x = atof(emitter->Attribute("MinDirX"));
			emt->mMinDir.y = atof(emitter->Attribute("MinDirY"));
			emt->mMaxDir.x = atof(emitter->Attribute("MaxDirX"));
			emt->mMaxDir.y = atof(emitter->Attribute("MaxDirY"));

			emt->mMinScale = atof(emitter->Attribute("MinScale"));
			emt->mMaxScale = atof(emitter->Attribute("MaxScale"));
			emt->mMaxCount = atoi(emitter->Attribute("MaxCount"));

			emt->mLoop = stricmp(emitter->Attribute("Loop"), "true") == 0;

			emt->mSpawnPos.x = atof(emitter->Attribute("SpawnPosX"));
			emt->mSpawnPos.y = atof(emitter->Attribute("SpawnPosY"));
			emt->mSpawnRadius.x = atof(emitter->Attribute("SpawnRadiusX"));
			emt->mSpawnRadius.y = atof(emitter->Attribute("SpawnRadiusY"));

			emt->mMinLifeTime = atof(emitter->Attribute("MinLifeTime"));
			emt->mMaxLifeTime = atof(emitter->Attribute("MaxLifeTime"));

			if(emitter->Attribute("MinFirstAppearTime"))
				emt->mMinFirstAppearTime = atof(emitter->Attribute("MinFirstAppearTime"));

			if(emitter->Attribute("MaxFirstAppearTime"))
				emt->mMaxFirstAppearTime = atof(emitter->Attribute("MaxFirstAppearTime"));

			emt->mMinStartColor = _atoi64(emitter->Attribute("MinStartColor"));
			emt->mMaxStartColor = _atoi64(emitter->Attribute("MaxStartColor"));

			emt->mMinEndColor = _atoi64(emitter->Attribute("MinEndColor"));
			emt->mMaxEndColor = _atoi64(emitter->Attribute("MaxEndColor"));

			emt->mRotation = atof(emitter->Attribute("Rotation"));
			emt->mRotationSpeed = atof(emitter->Attribute("RotationSpeed"));
			emt->mScaleSpeed = atof(emitter->Attribute("ScaleSpeed"));

			emt->mGravity.x = atof(emitter->Attribute("GravityX"));
			emt->mGravity.y = atof(emitter->Attribute("GravityY"));

			emt->mSrcBlend = atoi(emitter->Attribute("SrcBlend"));
			emt->mDestBlend = atoi(emitter->Attribute("DestBlend"));

			emt->SetTexture(emitter->Attribute("Image"));

			events = emitter->FirstChildElement("Events");
			if(events)
			{
				event = events->FirstChildElement("Event");
				while(event)
				{
					nekoParticleEvent pevt;

					pevt.mTime = atof(event->Attribute("Time"));
					pevt.mFlags = atoi(event->Attribute("Flags"));
					pevt.mMinDir.x = atof(event->Attribute("MinDirX"));
					pevt.mMinDir.y = atof(event->Attribute("MinDirY"));
					pevt.mMaxDir.x = atof(event->Attribute("MaxDirX"));
					pevt.mMaxDir.y = atof(event->Attribute("MaxDirY"));
					pevt.mGravity.x = atof(event->Attribute("GravityX"));
					pevt.mGravity.y = atof(event->Attribute("GravityY"));
					pevt.mRotationSpeed = atof(event->Attribute("RotationSpeed"));
					pevt.mScaleSpeed = atof(event->Attribute("ScaleSpeed"));
					pevt.mColor = _atoi64(event->Attribute("Color"));

					emt->ImportEvent(pevt);
					event = event->NextSiblingElement("Event");
				}
			}

			emt->FillNodes();
			emitter = emitter->NextSiblingElement("Emitter");
		}

		//LogPrint(LOG_LEVEL_INFO, "파티클 불러오기가 완료되었습니다. '%s'.", fileName);
		return S_OK;
	}

	void nekoParticleInstance::AtFirst()
	{
		list<nekoParticleEmitter>::iterator iter = mEmitters.begin();
		for(;iter != mEmitters.end();++iter)
			(*iter).FillNodes();
	}

	result nekoParticleInstance::SaveParticle(const char8 *fileName)
	{
		TiXmlDocument doc;

		TiXmlElement *root = nekoNew TiXmlElement("NekoNovel_Particle");
		{
			list<nekoParticleEmitter>::iterator iter = mEmitters.begin();
			for(;iter != mEmitters.end();++iter)
			{
				TiXmlElement *emitter = nekoNew TiXmlElement("Emitter");
				emitter->SetAttribute("Name", (*iter).mName.c_str());
				emitter->SetAttribute("EmitDelay", nekoUtility::FormString("%f", (*iter).mEmitDelay));
				emitter->SetAttribute("MinDirX", nekoUtility::FormString("%f", (*iter).mMinDir.x));
				emitter->SetAttribute("MinDirY", nekoUtility::FormString("%f", (*iter).mMinDir.y));
				emitter->SetAttribute("MaxDirX", nekoUtility::FormString("%f", (*iter).mMaxDir.x));
				emitter->SetAttribute("MaxDirY", nekoUtility::FormString("%f", (*iter).mMaxDir.y));

				emitter->SetAttribute("MinScale", nekoUtility::FormString("%f", (*iter).mMinScale));
				emitter->SetAttribute("MaxScale", nekoUtility::FormString("%f", (*iter).mMaxScale));

				emitter->SetAttribute("MaxCount", nekoUtility::FormString("%u", (*iter).mMaxCount));
				emitter->SetAttribute("Loop", ((*iter).mLoop) ? "true" : "false");

				emitter->SetAttribute("SpawnPosX", nekoUtility::FormString("%f", (*iter).mSpawnPos.x));
				emitter->SetAttribute("SpawnPosY", nekoUtility::FormString("%f", (*iter).mSpawnPos.y));
				emitter->SetAttribute("SpawnRadiusX", (*iter).mSpawnRadius.x);
				emitter->SetAttribute("SpawnRadiusY", (*iter).mSpawnRadius.y);

				emitter->SetAttribute("MinLifeTime", nekoUtility::FormString("%f", (*iter).mMinLifeTime));
				emitter->SetAttribute("MaxLifeTime", nekoUtility::FormString("%f", (*iter).mMaxLifeTime));

				emitter->SetAttribute("MinFirstAppearTime", nekoUtility::FormString("%f", (*iter).mMinFirstAppearTime));
				emitter->SetAttribute("MaxFirstAppearTime", nekoUtility::FormString("%f", (*iter).mMaxFirstAppearTime));

				emitter->SetAttribute("MinStartColor", nekoUtility::FormString("%u", (*iter).mMinStartColor));
				emitter->SetAttribute("MaxStartColor", nekoUtility::FormString("%u", (*iter).mMaxStartColor));

				emitter->SetAttribute("MinEndColor", nekoUtility::FormString("%u", (*iter).mMinEndColor));
				emitter->SetAttribute("MaxEndColor", nekoUtility::FormString("%u", (*iter).mMaxEndColor));

				emitter->SetAttribute("Rotation", nekoUtility::FormString("%f", (*iter).mRotation));
				emitter->SetAttribute("RotationSpeed", nekoUtility::FormString("%f", (*iter).mRotationSpeed));
				emitter->SetAttribute("ScaleSpeed", nekoUtility::FormString("%f", (*iter).mScaleSpeed));
				emitter->SetAttribute("GravityX", nekoUtility::FormString("%f", (*iter).mGravity.x));
				emitter->SetAttribute("GravityY", nekoUtility::FormString("%f", (*iter).mGravity.y));
				emitter->SetAttribute("SrcBlend", (*iter).mSrcBlend);
				emitter->SetAttribute("DestBlend", (*iter).mDestBlend);

				if((*iter).mTexture)
					emitter->SetAttribute("Image", nekoUtility::GetFileName((*iter).mTexture->GetName()));

				// 이벤트 저장
				TiXmlElement *events = nekoNew TiXmlElement("Events");

				list<nekoParticleEvent>::iterator itEvent = (*iter).mEventList.begin();
				for(;itEvent != (*iter).mEventList.end();++itEvent)
				{
					TiXmlElement *event = nekoNew TiXmlElement("Event");

					event->SetAttribute("Time", nekoUtility::FormString("%f", (*itEvent).mTime));
					event->SetAttribute("Flags", nekoUtility::FormString("%u", (*itEvent).mFlags));
					event->SetAttribute("MinDirX", nekoUtility::FormString("%f", (*itEvent).mMinDir.x));
					event->SetAttribute("MinDirY", nekoUtility::FormString("%f", (*itEvent).mMinDir.y));
					event->SetAttribute("MaxDirX", nekoUtility::FormString("%f", (*itEvent).mMaxDir.x));
					event->SetAttribute("MaxDirY", nekoUtility::FormString("%f", (*itEvent).mMaxDir.y));
					event->SetAttribute("GravityX", nekoUtility::FormString("%f", (*itEvent).mGravity.x));
					event->SetAttribute("GravityY", nekoUtility::FormString("%f", (*itEvent).mGravity.y));
					event->SetAttribute("RotationSpeed", nekoUtility::FormString("%f", (*itEvent).mRotationSpeed));
					event->SetAttribute("ScaleSpeed", nekoUtility::FormString("%f", (*itEvent).mScaleSpeed));
					event->SetAttribute("Color", nekoUtility::FormString("%u", (*itEvent).mColor));
					
					events->LinkEndChild(event);
				}

				emitter->LinkEndChild(events);
				root->LinkEndChild(emitter);
			}
		}

		doc.LinkEndChild(root);
		if(!doc.SaveFile(fileName))
			return E_FAIL;

		return S_OK;
	}


	void nekoParticleInstance::SetAllSpwanPosX(float32 pos)
	{
		list<nekoParticleEmitter>::iterator iter = mEmitters.begin();

		for(;iter != mEmitters.end();++iter)
			(*iter).mSpawnPos.x = pos;
	}

	void nekoParticleInstance::SetAllSpwanPosY(float32 pos)
	{
		list<nekoParticleEmitter>::iterator iter = mEmitters.begin();

		for(;iter != mEmitters.end();++iter)
			(*iter).mSpawnPos.y = pos;
	}

	inline void UpdateParticleColor(float32 &cur, float32 velocity, float32 delta, float32 end)
	{
		if(!velocity)
			return;

		if(!IsFloatSame(cur, end))
		{
			if(velocity > 0)
			{
				cur += velocity * delta;

				if(cur >= end)
					cur = end;
			}
			else
			{
				cur += velocity * delta;

				if(cur <= end)
					cur = end;
			}
		}
	}

	void nekoParticleInstance::Update(float32 frameTime)
	{
		list<nekoParticleEmitter>::iterator iter = mEmitters.begin();
		list<nekoParticleEvent>::iterator itEvent;

		bool finished = true;
		uint32 i;

		for(;iter != mEmitters.end();++iter)
		{
			nekoParticleEmitter &e = (*iter);

			if(e.mLoop)
				finished = false;

			for(i = 0;i < e.mMaxCount;++i)
			{
				nekoParticleNode &n = e.mNodeList[i];

				if(!n.alive)
				{
					if(n.enabled)
					{
						// 살았다!!
						finished = false;

						n.delta += frameTime;

						if(n.delta < n.lifetime)
							continue;

						n.Revive(e);
					}
				}
				else
				{
					n.delta += frameTime;

					// 죽었다!!
					if(n.delta >= n.lifetime)
					{
						if(e.mLoop == false)
							n.enabled = false;

						n.alive = false;
						n.lifetime = e.mEmitDelay;
						continue;
					}

					// 살았다!!
					finished = false;

					n.scale += e.mScaleSpeed * frameTime;
					n.rotation += n.rotSpeed * frameTime * 1000.0f;
					n.pos += nekoPointF((n.dir.x + n.gravity.x) * frameTime, (n.dir.y + n.gravity.y) * frameTime);

					UpdateParticleColor(n.r, n.vr, frameTime, n.er);
					UpdateParticleColor(n.g, n.vg, frameTime, n.eg);
					UpdateParticleColor(n.b, n.vb, frameTime, n.eb);
					UpdateParticleColor(n.a, n.va, frameTime, n.ea);

					// 이벤트 처리
					int32 i = 1;
					for(itEvent = e.mEventList.begin();itEvent != e.mEventList.end();++itEvent, i *= 2)
					{
						if(!(n.eventFlags & i) // 첫 실행!!
							&& n.delta >= (*itEvent).mTime)
						{
							if((*itEvent).mFlags & EPEF_DIR)
							{
								n.dir.x = RandBetween((*itEvent).mMinDir.x, (*itEvent).mMaxDir.x);
								n.dir.y = RandBetween((*itEvent).mMinDir.y, (*itEvent).mMaxDir.y);
							}

							if((*itEvent).mFlags & EPEF_GRAVITY)
							{
								n.gravity = (*itEvent).mGravity;
							}

							if((*itEvent).mFlags & EPEF_ROTATION)
							{
								n.rotSpeed = (*itEvent).mRotationSpeed / 180.0f * MATH_PI;
							}

							if((*itEvent).mFlags & EPEF_SCALE)
							{
								n.scaleSpeed = (*itEvent).mScaleSpeed;
							}

							if((*itEvent).mFlags & EPEF_COLOR)
							{
								n.r = ((*itEvent).mColor >> 16) & 0xff;
								n.g = ((*itEvent).mColor >> 8) & 0xff;
								n.b = (*itEvent).mColor & 0xff;
								n.a = ((*itEvent).mColor >> 24) & 0xff;
								n.vr = n.vg = n.vb = n.va = 0;
							}

							n.eventFlags |= i;
						}
					}
				}
			}
		}

		// 완료 됬구나!!
		if(finished && mForUseOnly)
		{
			SetValid(false);
		}
	}

	void nekoParticleInstance::Draw()
	{
		list<nekoParticleEmitter>::iterator iter;

		nekoVideoDriver *videoDrv = GetNekoNovel()->GetVideoDriver();
		IDirect3DDevice9 *device = videoDrv->GetHandle();
		D3DXMATRIX matWorld;
		D3DXMatrixIdentity(&matWorld);

		videoDrv->EndSprite();

		device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		device->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		device->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);

		uint32 color;

		
		int32 screenWidth = GetNekoNovel()->GetWidth();
		int32 screenHeight = GetNekoNovel()->GetHeight();
		for(iter = mEmitters.begin();iter != mEmitters.end();++iter)
		{
			nekoParticleEmitter &e = (*iter);

			if(!e.mTexture)
				continue;

			videoDrv->BeginSprite(D3DXSPRITE_DONOTMODIFY_RENDERSTATE);


			nekoPoint tsize = e.mTexture->GetSize();

			device->SetRenderState(D3DRS_SRCBLEND, e.mSrcBlend);
			device->SetRenderState(D3DRS_DESTBLEND, e.mDestBlend);

			e.mTexture->Refresh();

			for(uint32 i = 0;i < e.mMaxCount;++i)
			{
				nekoParticleNode &n = e.mNodeList[i];

				if(!n.alive || n.pos.x > screenWidth || n.pos.y > screenHeight)
					continue;

				D3DXMatrixTransformation2D(&matWorld, 0, 0, &D3DXVECTOR2(n.scale, n.scale),
					&D3DXVECTOR2(tsize.x * n.scale / 2, tsize.y * n.scale / 2), n.rotation,
					&D3DXVECTOR2(n.pos.x - tsize.x * n.scale / 2, n.pos.y - tsize.y * n.scale / 2));
				matWorld *= videoDrv->GetBaseMatrix();

				color = n.Color();

				videoDrv->GetSprite()->SetTransform(&matWorld);
				videoDrv->GetSprite()->Draw(e.mTexture->GetHandle(), NULL, NULL, &D3DXVECTOR3(0, 0, 0.5f), MixColorsAlpha(color, mAlpha));
			}

			videoDrv->EndSprite();
		}

		videoDrv->BeginSprite();

	}

	/// 데이터 덤프하기
	result nekoParticleInstance::Dump(int32 version, nekoByteBuffer &outBuf)
	{
		nekoIBlockInstance::Dump(version, outBuf);

		outBuf << mFileName;

		return S_OK;
	}

	result nekoParticleInstance::Load(int32 version, nekoByteBuffer &inBuf)
	{
		nekoIBlockInstance::Load(version, inBuf);

		if(version >= 1109)
		{
			string fileName;
			inBuf >> fileName;
			LoadParticle(fileName.c_str(), true);
		}

		return S_OK;
	}

	/// Constructor
	nekoParticle::nekoParticle()
		: nekoIBlock(BLOCK_PARTICLE)
	{

	}

	/// Destructor
	nekoParticle::~nekoParticle()
	{

	}

	/// 인스턴스를 만든다
	nekoIBlockInstance *nekoParticle::CreateInstance()
	{
		return nekoNew nekoParticleInstance(this);
	}
}