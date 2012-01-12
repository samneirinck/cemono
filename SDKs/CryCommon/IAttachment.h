#include DEVIRTUALIZE_HEADER_FIX(IAttachment.h)
#ifndef IAttachment_h
#define IAttachment_h

#include <ICryAnimation.h>

struct IStatObj;
struct IMaterial;
struct IPhysicalEntity;
struct ICharacterInstance;

struct IAttachment;
struct IAttachmentObject;

//

enum EAttachFlags
{
	FLAGS_ATTACH_ZOFFSET = 1, // Used to make sure a binding is more visible to the camera despite its distance.
	FLAGS_ATTACH_PHYSICALIZED_RAYS = 2,
	FLAGS_ATTACH_PHYSICALIZED_COLLISIONS = 4,
	FLAGS_ATTACH_PHYSICALIZED = FLAGS_ATTACH_PHYSICALIZED_RAYS|FLAGS_ATTACH_PHYSICALIZED_COLLISIONS,
};

//Flags used by game DO NOT REORDER
enum AttachmentTypes
{
	CA_BONE,
	CA_FACE,
	CA_SKIN,
	CA_PART, // character part
};

enum EVisibilityMask
{
	VISIBLE_THIRD_PERSON = 0x01, // Visible in third person view on the character.
	VISIBLE_FIRST_PERSON = 0x02, // Visible in first person view on the character.
	VISIBLE_HIGHQUALITY_THIRD_PERSON = 0x04, // Visible when first person view of high quality third person parts is enabled.
	VISIBLE_SHADOW_GENERATION = 0x08, // Visible in shadow.
	
	// NOTE: add string compare to CCharacterPartsManager::LoadPart() if you add something here
};


UNIQUE_IFACE struct IAttachmentManager
{
	virtual ~IAttachmentManager(){}
	virtual uint32 LoadAttachmentList(const char* pathname ) = 0;
	virtual uint32 SaveAttachmentList(const char* pathname ) = 0;

	virtual IAttachment* CreateAttachment( const char* szName, uint32 type, const char* szBoneName=0,bool bCallProject=true,bool bAllowDuplicates=false, bool bNoDefaultPose = false ) =0;
	virtual int32 RemoveAttachmentByInterface( const IAttachment* ptr  ) =0;
	virtual int32 RemoveAttachmentByName( const char* szName ) =0;

	virtual IAttachment* GetInterfaceByName( const char* szName ) const =0;
	virtual IAttachment* GetInterfaceByIndex( uint32 c) const =0;

	virtual int32 GetAttachmentCount() const =0;
	virtual int32 GetIndexByName( const char* szName ) const =0;

	virtual uint32 ProjectAllAttachment()=0;

	virtual void PhysicalizeAttachment( int idx, IPhysicalEntity *pent=0, int nLod=0 )=0;
	virtual void DephysicalizeAttachment( int idx, IPhysicalEntity *pent=0 )=0;

  // See EVisibilityMask for possible mask bits
  virtual void ApplyVisibilityMask(int mask) = 0;
  virtual void BakeMaterials() = 0;

  virtual ICharacterInstance* GetSkinInstance() const = 0; 
  virtual ICharacterInstance* GetSkelInstance() const = 0;
};

UNIQUE_IFACE struct IAttachment
{
	virtual ~IAttachment(){}
	virtual const char* GetName() const=0;   
	virtual uint32 ReName( const char* szBoneName ) =0;   

	virtual uint32 GetType() const =0; 
	virtual uint32 SetType(uint32 type, const char* szBoneName=0)=0;

	virtual uint32 GetFlags()=0;
	virtual void SetFlags(uint32 flags)=0;

	//attachment location in default model-pose
	virtual const QuatT& GetAttAbsoluteDefault() const =0; 
	virtual void SetAttAbsoluteDefault(const QuatT& rot)=0; 

	//attachment location relative to the attachment point (bone,face). Similar to an additional rotation 
	//its is the location in the default-pose 
	virtual void SetAttRelativeDefault(const QuatT& mat)=0; 
	virtual const QuatT& GetAttRelativeDefault() const =0; 

	//its is the location of the attachment in the animated-pose is model-space 
	virtual const QuatT& GetAttModelRelative() const=0; 
	//its is the location of the attachment in the animated-pose in world-space 
	virtual const QuatT& GetAttWorldAbsolute() const=0; 


	virtual uint32 ProjectAttachment()=0;

	virtual void HideAttachment( uint32 x )=0;
	virtual uint32 IsAttachmentHidden()=0;
	virtual void HideInRecursion( uint32 x )=0;
	virtual uint32 IsAttachmentHiddenInRecursion()=0;
	virtual void HideInShadow( uint32 x )=0;
	virtual uint32 IsAttachmentHiddenInShadow()=0;

	virtual void AlignBoneAttachment( uint32 x )=0;
	virtual uint32 GetAlignBoneAttachment() const = 0;

	virtual uint32 GetBoneID() const =0; 

	virtual uint32 AddBinding( IAttachmentObject* pModel) =0;   
	virtual IAttachmentObject* GetIAttachmentObject() const=0; 
	virtual void ClearBinding() =0;   

	virtual void GetHingeParams(int &idx,f32 &limit,f32 &damping) = 0;
	virtual void SetHingeParams(int idx=-1,f32 limit=120.0f,f32 damping=2.0f) = 0;

  virtual void SetFaceNr(uint32 faceNr) = 0;
};

// Description:
//     This interface define a way to allow an object to be bound to a character.
struct IAttachmentObject
{
	virtual ~IAttachmentObject(){}
	virtual void GetMemoryUsage( ICrySizer *pSizer ) const {/*REPLACE LATER*/}

	enum EType
	{
		eAttachment_Unknown,
		eAttachment_StatObj,
		eAttachment_Character,
		eAttachment_Entity,
		eAttachment_Light,
		eAttachment_Effect,
	};
	virtual EType GetAttachmentType() = 0;

	virtual void UpdateAttachment( IAttachment* pIAttachment, const QuatT& mtx, float fZoomAdjustedDistanceFromCamera, uint32 OnRender ) =0;
	virtual void RenderAttachment( SRendParams& rParams, IAttachment *pAttachment) {};
  // returns handled state
  virtual bool PhysicalizeAttachment( IAttachmentManager* pManager, int idx, int nLod, IPhysicalEntity *pent, const Vec3 &offset ) { return false; }

  virtual bool UpdatePhysicalizedAttachment( IAttachmentManager* pManager, int idx, IPhysicalEntity *pent, const QuatT &offset) { return false; }

	virtual bool StartAnimation (const char* szAnimName, const struct CryCharAnimationParams& Params) { return 0; }
	virtual bool StopAnimationInLayer (int nLayer) { return 0; }

  virtual AABB GetAABB()=0;
	virtual float GetRadiusSqr()=0;

	virtual IStatObj* GetIStatObj() { return 0; }
	SPU_INDIRECT(CommandBufferExecute(M))
	virtual ICharacterInstance* GetICharacterInstance() const { return 0; }

	virtual IMaterial *GetMaterial() = 0;
	virtual void SetMaterial(IMaterial *pMaterial) = 0;
	virtual void SetReplacementMaterial(IMaterial *pMaterial) = 0;
	virtual IMaterial* GetMaterialOverride() = 0;

	virtual void Release() = 0;

  virtual void OnRemoveAttachment(IAttachmentManager* pManager, int idx){}

};


//

struct CCGFAttachment : public IAttachmentObject
{
	virtual EType GetAttachmentType() { return eAttachment_StatObj; };
	void UpdateAttachment(IAttachment *pIAttachment, const QuatT &m, float fZoomAdjustedDistanceFromCamera, uint32 OnRender) {}
	void RenderAttachment(SRendParams &rParams, IAttachment *pAttachment )
  {
		rParams.pInstance = this;
		IMaterial *pPrev = rParams.pMaterial;
		if (m_pReplacementMaterial)
			rParams.pMaterial = m_pReplacementMaterial;
		else if (pMaterial)
			rParams.pMaterial = pMaterial;
		pObj->Render(rParams);
		rParams.pMaterial = pPrev;
	};

	AABB GetAABB() { return pObj->GetAABB(); };
	float GetRadiusSqr() { return sqr(pObj->GetRadius()); };
	IStatObj* GetIStatObj() {	return pObj;	}
	void Release() { delete this;	}
	IMaterial *GetMaterial() { return pMaterial? (IMaterial*)pMaterial : pObj->GetMaterial();};
	void SetMaterial(IMaterial *pMaterial) { this->pMaterial = pMaterial; };
	void SetReplacementMaterial(IMaterial *pMaterial) { m_pReplacementMaterial = pMaterial; };
	IMaterial* GetMaterialOverride() { return m_pReplacementMaterial ? m_pReplacementMaterial : pMaterial; }
	_smart_ptr<IStatObj> pObj;
	_smart_ptr<IMaterial> pMaterial;
	_smart_ptr<IMaterial> m_pReplacementMaterial;
//  Matrix34 m_lastRenderMatrix;
};

struct CCHRAttachment : public IAttachmentObject
{
  CCHRAttachment()
  {
  }

  virtual EType GetAttachmentType() { return eAttachment_Character; };
  void UpdateAttachment( IAttachment *pAttachment, const QuatT& rWorldLocation, float fZoomAdjustedDistanceFromCamera, uint32 OnRender )	
  {
		m_pCharInstance->ProcessAttachment( pAttachment, rWorldLocation, fZoomAdjustedDistanceFromCamera, OnRender);
  };
  void RenderAttachment( SRendParams &rParams, IAttachment *pAttachment  )	
  {
		rParams.pInstance = this;

    IMaterial *pPrev = rParams.pMaterial;
		if (m_pReplacementMaterial)
			rParams.pMaterial = m_pReplacementMaterial;
    else if (m_pMaterial)
      rParams.pMaterial = m_pMaterial;		
    m_pCharInstance->Render(rParams,QuatTS(IDENTITY));
    rParams.pMaterial = pPrev;
  };

	bool StartAnimation (const char* szAnimName, const struct CryCharAnimationParams& Params)	
	{
		return m_pCharInstance->GetISkeletonAnim()->StartAnimation(szAnimName,Params);
	}
	bool StopAnimationInLayer (int nLayer)	
	{ 
		return m_pCharInstance->GetISkeletonAnim()->StopAnimationInLayer(nLayer,0.0f);
	}

  AABB GetAABB() {	return m_pCharInstance ? m_pCharInstance->GetAABB() : AABB(AABB::RESET);	};
	float GetRadiusSqr() { return m_pCharInstance ? m_pCharInstance->GetRadiusSqr() : 0.0f;		}
	SPU_INDIRECT(CommandBufferExecute(M))
  ICharacterInstance* GetICharacterInstance() const { return m_pCharInstance; }

  void Release() 
  { 
    if (m_pCharInstance) 
      m_pCharInstance->OnDetach(); 
    delete this;	
  }

	IMaterial *GetMaterial() { return m_pMaterial ? (IMaterial*)m_pMaterial : (m_pCharInstance?m_pCharInstance->GetMaterial():NULL); }
  void SetMaterial(IMaterial *pMaterial) { m_pMaterial = pMaterial; }
	void SetReplacementMaterial(IMaterial *pMaterial) { m_pReplacementMaterial = pMaterial; }
	IMaterial* GetMaterialOverride() { return m_pReplacementMaterial ? m_pReplacementMaterial : m_pMaterial; }

  _smart_ptr<ICharacterInstance> m_pCharInstance;
  _smart_ptr<IMaterial> m_pMaterial;
	_smart_ptr<IMaterial> m_pReplacementMaterial;

  //
};


UNIQUE_IFACE struct ICharacterPartAttachment : public CCHRAttachment
{
  // Visibility mask
  virtual uint32 GetVisibilityMask() const = 0;
  virtual void SetVisibilityMask(uint32 newVisMask) = 0;
  virtual void AddFaceAttachment(const char* partName,const char* name, QuatT attRelativeDefault, QuatT attAbsoluteDefault, uint32 faceNr) = 0;
};

struct CEntityAttachment :	public IAttachmentObject
{
public:
	CEntityAttachment():m_scale(1.0f,1.0f,1.0f),m_id(0){}
	
	virtual EType GetAttachmentType() { return eAttachment_Entity; };
	void SetEntityId(EntityId id) { m_id = id; };
	EntityId GetEntityId() { return m_id; }

	void UpdateAttachment(IAttachment *pIAttachment,const QuatT &m, float fZoomAdjustedDistanceFromCamera, uint32 OnRender )
	{
		const QuatT& quatT = pIAttachment->GetAttWorldAbsolute();
		
		IEntity *pEntity = gEnv->pEntitySystem->GetEntity(m_id);

		if (pEntity)
		{
    //	float white[] = {1.0f, 1.0f, 1.0f, 1.0f};
		//	gEnv->pRenderer->Draw2dLabel(500.0f, 560.0f, 1.6f, white, false, "UpdateAttach transform: %.12f, %.12f, %.12f\n  rot: %.9f, %.9f, %.9f, %.9f", quatT.t.x, quatT.t.y, quatT.t.z, quatT.q.v.x, quatT.q.v.y, quatT.q.v.z, quatT.q.w);

			pEntity->SetPosRotScale( quatT.t, quatT.q, m_scale, ENTITY_XFORM_NO_PROPOGATE);
		}
	}

	AABB GetAABB()
	{
		IEntity *pEntity = gEnv->pEntitySystem->GetEntity(m_id);
		AABB aabb(Vec3(0,0,0), Vec3(0,0,0));

		if (pEntity) pEntity->GetLocalBounds(aabb);
		return aabb;
	};

	float GetRadiusSqr() 
	{
		IEntity *pEntity = gEnv->pEntitySystem->GetEntity(m_id);
		if(pEntity)
		{
			AABB aabb;
			pEntity->GetLocalBounds(aabb);
			return aabb.GetRadiusSqr();
		}
		else
		{
			return 0.0f;
		}
	};

	void Release() { delete this;	};

	IMaterial *GetMaterial() { return 0; };
	void SetMaterial(IMaterial *pMaterial) {};
	void SetReplacementMaterial(IMaterial *pMaterial) {};
	IMaterial* GetMaterialOverride() { return 0; }

  void SetScale(const Vec3& scale)
  {
    m_scale = scale;
  }

private:
	EntityId	m_id;
  Vec3 m_scale;
};

struct CLightAttachment : public IAttachmentObject
{
public:
	CLightAttachment(): m_pLightSource(0)	{};
	virtual ~CLightAttachment()
	{
		if (m_pLightSource)
		{
			gEnv->p3DEngine->UnRegisterEntity(m_pLightSource);
			m_pLightSource->ReleaseNode();
		}
	};

	virtual EType GetAttachmentType() { return eAttachment_Light; };

	void LoadLight(const CDLight &light)
	{
		m_pLightSource = gEnv->p3DEngine->CreateLightSource();
    if (m_pLightSource)
			m_pLightSource->SetLightProperties(light);
	}

  ILightSource* GetLightSource() { return m_pLightSource; }

	void UpdateAttachment(IAttachment *pAttachment, const QuatT &m, float fZoomAdjustedDistanceFromCamera, uint32 OnRender)	
	{
		if (m_pLightSource)
		{
			CDLight &light = m_pLightSource->GetLightProperties();

			Matrix34 worldMatrix = Matrix34(pAttachment->GetAttWorldAbsolute());
			Vec3 origin = worldMatrix.GetTranslation();
			light.SetPosition( origin );
			light.SetMatrix(worldMatrix);
			light.m_sName = pAttachment->GetName();
			m_pLightSource->SetMatrix(worldMatrix);
			f32 r = light.m_fRadius;
			m_pLightSource->SetBBox(AABB(Vec3(origin.x-r, origin.y-r, origin.z-r), Vec3(origin.x+r,origin.y+r,origin.z+r)));
			gEnv->p3DEngine->RegisterEntity(m_pLightSource);
		}
	}

	AABB GetAABB()
	{
		f32 r = m_pLightSource->GetLightProperties().m_fRadius;
		return AABB(Vec3(-r, -r, -r), Vec3(+r, +r, +r));
	};

	float GetRadiusSqr() { return sqr(m_pLightSource->GetLightProperties().m_fRadius); }

	void Release() { delete this; };

	IMaterial *GetMaterial() { return 0; };
	void SetMaterial(IMaterial *pMaterial) {};
	void SetReplacementMaterial(IMaterial *pMaterial) {};
	IMaterial* GetMaterialOverride() { return 0; }

private:
	ILightSource *m_pLightSource;
};

struct CEffectAttachment : public IAttachmentObject
{
public:

	virtual EType GetAttachmentType() { return eAttachment_Effect; };

	CEffectAttachment(const char *effectName, const Vec3 &offset, const Vec3 &dir, f32 scale)
		:	m_offset(offset),
		m_dir(dir),
		m_scale(scale),
		m_pEmitter(0)
	{
		m_pEffect = gEnv->pParticleManager->FindEffect(effectName,"Character Attachment",true);
	}

	virtual ~CEffectAttachment()
	{
		if (m_pEmitter)
			m_pEmitter->Activate(false);
		m_pEmitter = 0;
		m_pEffect = 0;
	}

	void CreateEffect(const Matrix34A& a_baseMatrix)
	{
		if (!m_pEmitter && m_pEffect != 0)
		{
			if (m_dir.len2()>0)
				m_loc = Matrix34(Matrix33::CreateRotationVDir(m_dir));
			else
				m_loc.SetIdentity();
			m_loc.AddTranslation(m_offset);
			m_loc.Scale(Vec3(m_scale));

			const Matrix34 spawnMatrix = a_baseMatrix * m_loc;
			m_pEmitter = m_pEffect->Spawn(false, spawnMatrix);
		}
	}

	IParticleEmitter *GetEmitter()
	{
		return m_pEmitter;
	}

	void UpdateAttachment(IAttachment *pIAttachment,const QuatT &m, float fZoomAdjustedDistanceFromCamera, uint32 OnRender )
	{
		if (!pIAttachment->IsAttachmentHidden())
		{
			if (!m_pEmitter)
			{
				CreateEffect(Matrix34A(pIAttachment->GetAttWorldAbsolute()));
			}

			if (m_pEmitter)
				m_pEmitter->SetMatrix( Matrix34A(pIAttachment->GetAttWorldAbsolute()) * m_loc);
		}
		else
		{
			m_pEmitter = 0;
		}
	}

	AABB GetAABB()
	{
		if (m_pEmitter)
		{
			AABB bb;
			m_pEmitter->GetLocalBounds(bb);
			return bb;
		}
		else
		{
			return AABB(Vec3(-0.1f), Vec3(0.1f));
		}
	};

	float GetRadiusSqr()
	{
		if (m_pEmitter)
		{
			AABB bb;
			m_pEmitter->GetLocalBounds(bb);
			return bb.GetRadiusSqr();
		}
		else
		{
			return 0.1f;
		}
	}

	void Release() { delete this; };

	IMaterial *GetMaterial() { return 0; };
	void SetMaterial(IMaterial *pMaterial) {};
	void SetReplacementMaterial(IMaterial *pMaterial) {};
	IMaterial* GetMaterialOverride() { return 0; }

	void SetSpawnParams( const SpawnParams& params )
	{  
		if (m_pEmitter)
			m_pEmitter->SetSpawnParams(params);
	}

private:
	_smart_ptr<IParticleEmitter>	m_pEmitter;
	_smart_ptr<IParticleEffect>		m_pEffect;
	Vec3							m_offset;
	Vec3							m_dir;
	f32							m_scale;
	Matrix34					m_loc;
};

#endif // IAttachment_h
