#ifndef _CREIRRADIANCEVOLUME_
#define _CREIRRADIANCEVOLUME_

#pragma once

#include "VertexFormats.h"

struct SReflectiveShadowMap
{
	Matrix44A mxLightViewProj;
	ITexture*	pDepthRT;
	ITexture*	pNormalRT;
	ITexture*	pColorRT;
	SReflectiveShadowMap()
	{
		mxLightViewProj.SetIdentity();
		pDepthRT = NULL;
		pNormalRT = NULL;
		pColorRT = NULL;
	}
	void Release();
};

class CREIrradianceVolume : public CRendElementBase
{
protected:
	friend struct IIrradianceVolumeRenderNode;
	friend class CIrradianceVolumeRenderNode;
	friend class CLightPropagationVolume;
	friend class CIrradianceVolumesManager;
private:

	// injects single texture or colored shadow map with vertex texture fetching
	void _injectWithVTF(SReflectiveShadowMap& rCSM);

	// injects single texture or colored shadow map with render to vertex buffer technique
	void _injectWithR2VB(SReflectiveShadowMap& rCSM);

	// injects occlusion from camera depth buffer
	void _injectOcclusionFromCamera();

protected:

	// injects single light
	bool InjectAllLightSources();

	// injects single light
	void InjectLight(const CDLight& rLight);

	// injects single colored shadow map
	void InjectColorMap(SReflectiveShadowMap& rCSM);

	// injects occlusion information from depth buffer
	void InjectOcclusion(SReflectiveShadowMap& rCSM, bool bCamera);

	// propagates irradiance
	void Simulate();

	// post-injection phase(injection after simulation)
	bool Postinject();

	// post-injects single ready light
	void PostnjectLight(const CDLight& rLight);

	// optimizations for faster deferred rendering
	void ResolveToVolumeTexture();

	// smart downsampling filtering for large RSMs
	void DownsampleRSM(SReflectiveShadowMap& rSourceRSM, SReflectiveShadowMap& rDestRSM);
public:

	// returns irradiance volume id (might be the same as light id for GI volumes)
	int GetId() const { return m_nId; }

	ILightSource* GetAttachedLightSource() { return m_pAttachedLightSource; }
	void AttachLightSource(ILightSource* pLightSource) { m_pAttachedLightSource = pLightSource; }

	CREIrradianceVolume();
	virtual ~CREIrradianceVolume();

	// flags for irradiance volume render element
	enum EFlags
	{
		efGIVolume				= 1ul<<0,
		efHasOcclusion		= 1ul<<1,
	};

	enum EStaticProperties
	{



		espMaxInjectRSMSize = 384ul,

	};

	// irradiance volume render settings
	struct Settings
	{
		Vec3 			m_pos;
		AABB			m_bbox;											// BBox
		Matrix44A	m_mat;
		Matrix44A	m_matInv;
		int				m_nGridWidth;								// grid dimensions in texels
		int				m_nGridHeight;							// grid dimensions in texels
		int				m_nGridDepth;								// grid dimensions in texels
		int				m_nNumIterations;						// number of iterations to propagate
		Vec4			m_gridDimensions;						// grid size
		Vec4			m_invGridDimensions;				// 1.f / (grid size)
		Settings();
	};

	// render colored shadowmap to render
	virtual void RenderReflectiveShadowMap( SReflectiveShadowMap& rRSM );

	// query point light source to render
	virtual void InsertLight( const CDLight &light );

	// propagates irradiance
	void Evaluate();

	// apply irradiance to accumulation RT
	void DeferredApply(ITexture* pDepthRT, ITexture* pNormalRT);

	// pass only CREIrradianceVolume::EFlags enum flags combination here
	void SetNewFlags(uint32 nFlags) { m_nGridFlags = nFlags; }

	// CREIrradianceVolume::EFlags enum flags returned
	uint32 GetFlags() const { return m_nGridFlags; }

	virtual void mfPrepare(bool bCheckOverflow);
	virtual bool mfPreDraw(SShaderPass *sl);
	virtual bool mfDraw(CShader* ef, SShaderPass* sfm);
	virtual float mfDistanceToCameraSquared(Matrix34& matInst);

	void Invalidate() { m_bIsUpToDate = false; }

	ITexture* GetLPVTexture(int iTex) { assert(iTex < 3); return m_pVolumeTextures[iTex]; }
	float GetVisibleDistance() const { return m_fDistance; }
	float GetIntensity() const { return m_fAmount; }

	// is it applicable to render in the deferred pass?
	inline bool IsRenderable() const { return m_bIsRenderable; }

	virtual Settings* GetFillSettings();

	virtual void GetMemoryUsage(ICrySizer *pSizer) const 
	{
		pSizer->AddObject(this, sizeof(*this));
		pSizer->AddContainer(m_lightsToInject[0][0]);
		pSizer->AddContainer(m_lightsToInject[0][1]);
		pSizer->AddContainer(m_lightsToInject[1][0]);
		pSizer->AddContainer(m_lightsToInject[1][1]);
		pSizer->AddContainer(m_lightsToPostinject[0][0]);
		pSizer->AddContainer(m_lightsToPostinject[0][1]);
		pSizer->AddContainer(m_lightsToPostinject[1][0]);
		pSizer->AddContainer(m_lightsToPostinject[1][1]);	
	}
	const Settings& GetRenderSettings() const;
protected:
	virtual void UpdateRenderParameters();
	virtual void EnableSpecular(const bool bEnabled);
	void Cleanup();

protected:
	Settings	m_Settings[RT_COMMAND_BUF_COUNT];

	int				m_nId;											// unique ID of the volume(need for RTs)
	uint32		m_nGridFlags;								// grid flags

	float			m_fDistance;								// max affected distance
	float			m_fAmount;									// affection scaler

	union 
	{	
		ITexture*	m_pRT[3];	
		CTexture*	m_pRTex[3];
	};																		// grid itself

	union																	// volume textures
	{
		ITexture*	m_pVolumeTextures[3];
		CTexture*	m_pVolumeTexs[3];
	};


#if PS3
	union																	// render to volume texture for PS3		
	{
		ITexture*	m_p2DVolumeUnwraps[3];
		CTexture*	m_p2DVolumeUnwrapsTex[3];
	};
#endif

	union																	// volume texture for fuzzy occlusion from depth buffers
	{
		ITexture*	m_pOcclusionTexture;
		CTexture*	m_pOcclusionTex;
	};

	SReflectiveShadowMap	m_downsampledRSM;		// for result of RSM downsampling

	struct IIrradianceVolumeRenderNode* m_pParent;

	ILightSource*	m_pAttachedLightSource;

	bool			m_bIsRenderable;						// is the grid not dark?
	bool			m_bNeedPropagate;						// is the grid needs to be propagated
	bool			m_bNeedClear;								// is the grid needs to be cleared after past frame
	bool			m_bIsUpToDate;							// invalidates dimensions
	bool			m_bHasSpecular;							// enables specular
	int				m_nUpdateFrameID;						// last frame updated

	typedef DynArray<CDLight> Lights;
	Lights		m_lightsToInject[RT_COMMAND_BUF_COUNT][2];			// lights in the grid
	Lights		m_lightsToPostinject[RT_COMMAND_BUF_COUNT][2];	// lights in the grid
};


#endif // #ifndef _CREIRRADIANCEVOLUME_
