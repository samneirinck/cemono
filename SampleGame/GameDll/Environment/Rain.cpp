#include "StdAfx.h"
#include "Rain.h"
#include "Game.h"

CRain::CRain()
{
}

CRain::~CRain()
{
	for (TTextureList::iterator it = m_Textures.begin(), itEnd = m_Textures.end(); it != itEnd; ++ it)
	{
		(*it)->Release();
	}
}

//------------------------------------------------------------------------
bool CRain::Init(IGameObject *pGameObject)
{
	SetGameObject(pGameObject);
	PreloadTextures();

	return Reset();
}

//------------------------------------------------------------------------
void CRain::PreloadTextures()
{
	uint32 nDefaultFlags = FT_DONT_RESIZE | FT_DONT_STREAM;

	XmlNodeRef root = GetISystem()->LoadXmlFromFile( "Shaders/EngineAssets/raintextures.xml" );
	if (root)
	{
		for (int i = 0; i < root->getChildCount(); i++)
		{
			XmlNodeRef entry = root->getChild(i);
			if (!entry->isTag("entry"))
				continue;

			uint32 nFlags = nDefaultFlags;

			// check attributes to modify the loading flags
			int nNoMips = 0;
			if (entry->getAttr("nomips", nNoMips) && nNoMips)
				nFlags |= FT_NOMIPS;

			ITexture* pTexture = gEnv->pRenderer->EF_LoadTexture(entry->getContent(), nFlags);
			if (pTexture)	{
				m_Textures.push_back(pTexture);
			}
		}
	}
}

//------------------------------------------------------------------------
void CRain::PostInit(IGameObject *pGameObject)
{
	GetGameObject()->EnableUpdateSlot(this, 0);
}

//------------------------------------------------------------------------
bool CRain::ReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params )
{
	ResetGameObject();

	CRY_ASSERT_MESSAGE(false, "CRain::ReloadExtension not implemented");
	
	return false;
}

//------------------------------------------------------------------------
bool CRain::GetEntityPoolSignature( TSerialize signature )
{
	CRY_ASSERT_MESSAGE(false, "CRain::GetEntityPoolSignature not implemented");
	
	return true;
}

//------------------------------------------------------------------------
void CRain::Release()
{
	delete this;
}

//------------------------------------------------------------------------
void CRain::FullSerialize(TSerialize ser)
{
	ser.Value("bEnabled", m_bEnabled);
	ser.Value("fRadius", m_fRadius);
	ser.Value("fAmount", m_fAmount);
	ser.Value("clrColor", m_vColor);
	ser.Value("fReflectionAmount", m_fReflectionAmount);
	ser.Value("fFakeGlossiness", m_fFakeGlossiness);
	ser.Value("fPuddlesAmount", m_fPuddlesAmount);
	ser.Value("bRainDrops", m_bRainDrops);
	ser.Value("fRainDropsSpeed", m_fRainDropsSpeed);
	ser.Value("fUmbrellaRadius", m_fUmbrellaRadius);
}

//------------------------------------------------------------------------
void CRain::Update(SEntityUpdateContext &ctx, int updateSlot)
{
	const IActor * pClient = g_pGame->GetIGameFramework()->GetClientActor();
	if (pClient && Reset())
	{
		const Vec3 vCamPos = gEnv->pRenderer->GetCamera().GetPosition();
		Vec3 vR = (GetEntity()->GetWorldPos() - vCamPos) / max(m_fRadius, 1e-3f);
		float fAttenAmount = max(0.f, 1.0f - vR.dot(vR));
		fAttenAmount *= m_fAmount;

		// Force set if current values not valid
		float fCurRadius, fCurAmount;
		Vec3 vCurColor;
		bool bSet = !gEnv->p3DEngine->GetRainParams(vR, fCurRadius, fCurAmount, vCurColor);

		// Set if stronger
		bSet |= fAttenAmount > fCurAmount;

		if (bSet)
		{
			gEnv->p3DEngine->SetRainParams(GetEntity()->GetWorldPos(), m_fRadius, fAttenAmount, m_vColor);
			gEnv->p3DEngine->SetRainParams(0.5f * m_fReflectionAmount, m_fFakeGlossiness, m_fPuddlesAmount, m_bRainDrops, m_fRainDropsSpeed, m_fUmbrellaRadius);
		}
	}
}

//------------------------------------------------------------------------
void CRain::HandleEvent(const SGameObjectEvent &event)
{
}

//------------------------------------------------------------------------
void CRain::ProcessEvent(SEntityEvent &event)
{
	switch (event.event)
	{
	case ENTITY_EVENT_RESET:
		Reset();
		break;
	case ENTITY_EVENT_HIDE:
	case ENTITY_EVENT_DONE:
		if (gEnv && gEnv->p3DEngine)
		{
			static const Vec3 vZero(ZERO);
			gEnv->p3DEngine->SetRainParams(vZero, 0, 0, vZero);
		}
		break;
	}
}

//------------------------------------------------------------------------
void CRain::SetAuthority(bool auth)
{
}

//------------------------------------------------------------------------
bool CRain::Reset()
{
	//Initialize default values before (in case ScriptTable fails)
	m_bEnabled = false;
	m_fRadius = 50.f;
	m_fAmount = 1.f;
	m_vColor.Set(1,1,1);
	m_fReflectionAmount = 1.f;
	m_fFakeGlossiness = 1.f;
	m_fPuddlesAmount = 3.f;
	m_bRainDrops = true;
	m_fRainDropsSpeed = 1.f;
	m_fUmbrellaRadius = 0.f;

	SmartScriptTable props;
	IScriptTable* pScriptTable = GetEntity()->GetScriptTable();
	if (!pScriptTable || !pScriptTable->GetValue("Properties", props))
		return false;

	props->GetValue("fRadius", m_fRadius);
	props->GetValue("fAmount", m_fAmount);
	props->GetValue("clrColor", m_vColor);
	props->GetValue("fReflectionAmount", m_fReflectionAmount);
	props->GetValue("fFakeGlossiness", m_fFakeGlossiness);
	props->GetValue("fPuddlesAmount", m_fPuddlesAmount);
	props->GetValue("bRainDrops", m_bRainDrops);
	props->GetValue("fRainDropsSpeed", m_fRainDropsSpeed);
	props->GetValue("fUmbrellaRadius", m_fUmbrellaRadius);
	props->GetValue("bEnabled", m_bEnabled);
	if (!m_bEnabled)
		m_fAmount = 0;

	return true;
}
