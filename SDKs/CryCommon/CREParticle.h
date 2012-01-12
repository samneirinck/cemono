#include DEVIRTUALIZE_HEADER_FIX(CREParticle.h)

#ifndef __CREPARTICLE_H__
#define __CREPARTICLE_H__

#include "CryThread.h"

typedef SVF_P3F_C4B_I4B_PS4F		SVertexParticle;

// forward declarations
class CREParticle;
struct CRenderListData;

//#define PARTICLE_RENDER_ORDER






























struct SRenderVertices
{
	FixedDynArray<SVertexParticle>	aVertices;
	FixedDynArray<uint16>						aIndices;
	int															nBaseVertexIndex;
	float														fMaxPixels;
	float														fPixels;

	SRenderVertices()
		{ memset(this, 0, sizeof(*this)); }

	inline void CopyVertices( Array<SVertexParticle>& aSrc )
	{
		int nVerts = min(aSrc.size(), aVertices.available());
		cryMemcpy(aVertices.grow_raw(nVerts), &aSrc[0], sizeof(SVertexParticle) * nVerts);
		aSrc.erase_front(nVerts);
	}

	inline void ExpandQuadVertices()
	{
		SVertexParticle* aV = aVertices.grow_raw(3)-1;

		aV[3] = aV[2] = aV[1] = aV[0];

		aV[1].info.tex_x = 255;
		aV[2].info.tex_y = 255;
		aV[3].info.tex_x = 255;
		aV[3].info.tex_y = 255;
	}

	inline void ExpandQuadVertices(const SVertexParticle& RESTRICT_REFERENCE part)
	{
		SVertexParticle* aV = aVertices.grow_raw(4);
		CryPrefetch(aV + 4);

		const uint32* s = reinterpret_cast<const uint32*>(&part);
		uint32* a = reinterpret_cast<uint32*>(aV);
		uint32* b = reinterpret_cast<uint32*>(aV + 1);
		uint32* c = reinterpret_cast<uint32*>(aV + 2);
		uint32* d = reinterpret_cast<uint32*>(aV + 3);

		for (size_t i = 0; i < sizeof(SVertexParticle) / sizeof(uint32); ++ i)
		{
			uint32 v = s[i];
			a[i] = v;
			b[i] = v;
			c[i] = v;
			d[i] = v;
		}

		aV[1].info.tex_x = 255;
		aV[2].info.tex_y = 255;
		aV[3].info.tex_x = 255;
		aV[3].info.tex_y = 255;
	}

	inline void ExpandOctVertices()
	{
		SVertexParticle* aV = aVertices.grow_raw(7) - 1;

		aV[7] = aV[6] = aV[5] = aV[4] = aV[3] = aV[2] = aV[1] = aV[0];

		aV[0].info.tex_x = 75;
		aV[0].info.tex_y = 0;
		aV[1].info.tex_x = 180;
		aV[1].info.tex_y = 0;
		aV[2].info.tex_x = 255;
		aV[2].info.tex_y = 75;
		aV[3].info.tex_x = 255;
		aV[3].info.tex_y = 180;
		aV[4].info.tex_x = 180;
		aV[4].info.tex_y = 255;
		aV[5].info.tex_x = 75;
		aV[5].info.tex_y = 255;
		aV[6].info.tex_x = 0;
		aV[6].info.tex_y = 180;
		aV[7].info.tex_x = 0;
		aV[7].info.tex_y = 75;
	}

	inline void ExpandOctVertices(const SVertexParticle& RESTRICT_REFERENCE part)
	{
		SVertexParticle* aV = aVertices.grow_raw(8);
		CryPrefetch(aV + 8);

		const uint32* s = reinterpret_cast<const uint32*>(&part);
		uint32* a = reinterpret_cast<uint32*>(aV);
		uint32* b = reinterpret_cast<uint32*>(aV + 1);
		uint32* c = reinterpret_cast<uint32*>(aV + 2);
		uint32* d = reinterpret_cast<uint32*>(aV + 3);
		uint32* e = reinterpret_cast<uint32*>(aV + 4);
		uint32* f = reinterpret_cast<uint32*>(aV + 5);
		uint32* g = reinterpret_cast<uint32*>(aV + 6);
		uint32* h = reinterpret_cast<uint32*>(aV + 7);

		for (size_t i = 0; i < sizeof(SVertexParticle) / sizeof(uint32); ++ i)
		{
			uint32 v = s[i];
			a[i] = v;
			b[i] = v;
			c[i] = v;
			d[i] = v;
			e[i] = v;
			f[i] = v;
			g[i] = v;
			h[i] = v;
		}

		aV[0].info.tex_x = 75;
		aV[0].info.tex_y = 0;
		aV[1].info.tex_x = 180;
		aV[1].info.tex_y = 0;
		aV[2].info.tex_x = 255;
		aV[2].info.tex_y = 75;
		aV[3].info.tex_x = 255;
		aV[3].info.tex_y = 180;
		aV[4].info.tex_x = 180;
		aV[4].info.tex_y = 255;
		aV[5].info.tex_x = 75;
		aV[5].info.tex_y = 255;
		aV[6].info.tex_x = 0;
		aV[6].info.tex_y = 180;
		aV[7].info.tex_x = 0;
		aV[7].info.tex_y = 75;
	}

	inline void SetQuadIndices(int nVertAdvance = 4)
	{
		uint16* pIndices = aIndices.grow(6);

		pIndices[0] = 0 + nBaseVertexIndex;
		pIndices[1] = 1 + nBaseVertexIndex;
		pIndices[2] = 2 + nBaseVertexIndex;

		pIndices[3] = 3 + nBaseVertexIndex;
		pIndices[4] = 2 + nBaseVertexIndex;
		pIndices[5] = 1 + nBaseVertexIndex;

		nBaseVertexIndex += nVertAdvance;
	}

	inline void SetQuadsIndices()
	{
		assert((aVertices.size() & 3) == 0);
		int nQuads = aVertices.size() >> 2;
		assert(aIndices.available() >= nQuads*6);
		while (nQuads-- > 0)
			SetQuadIndices();
	}

	inline void SetOctIndices()
	{
		uint16* pIndices = aIndices.grow(18);

		pIndices[0] = 0 + nBaseVertexIndex;
		pIndices[1] = 1 + nBaseVertexIndex;
		pIndices[2] = 2 + nBaseVertexIndex;
		pIndices[3] = 0 + nBaseVertexIndex;
		pIndices[4] = 2 + nBaseVertexIndex;
		pIndices[5] = 4 + nBaseVertexIndex;
		pIndices[6] = 2 + nBaseVertexIndex;
		pIndices[7] = 3 + nBaseVertexIndex;
		pIndices[8] = 4 + nBaseVertexIndex;
		pIndices[9] = 0 + nBaseVertexIndex;
		pIndices[10] = 4 + nBaseVertexIndex;
		pIndices[11] = 6 + nBaseVertexIndex;
		pIndices[12] = 4 + nBaseVertexIndex;
		pIndices[13] = 5 + nBaseVertexIndex;
		pIndices[14] = 6 + nBaseVertexIndex;
		pIndices[15] = 6 + nBaseVertexIndex;
		pIndices[16] = 7 + nBaseVertexIndex;
		pIndices[17] = 0 + nBaseVertexIndex;

		nBaseVertexIndex += 8;
	}

	inline void SetOctsIndices()
	{
		assert((aVertices.size() & 7) == 0);
		int nOcts = aVertices.size() >> 3;
		assert(aIndices.available() >= nOcts*18);

		for (int i = 0; i < nOcts; i++)
		{
			SetOctIndices();
		}
	}

	inline void SetPolyIndices( int nVerts )
	{
		nVerts >>= 1;
		while (--nVerts > 0)
			SetQuadIndices(2);

		// Final quad.
		nBaseVertexIndex += 2;
	}

	void SetPoliesIndices( Array<SVertexParticle>& aSrcVerts, Array<uint16>& aSrcVertCounts )
	{
		int nAvailVerts = aVertices.available();
		int nVerts = 0;
		int nPolygon = 0;
		for (; nPolygon < aSrcVertCounts.size(); nPolygon++)
		{
			int nPolyVerts = aSrcVertCounts[nPolygon];
			if (nVerts + nPolyVerts > nAvailVerts)
				break;
			nVerts += nPolyVerts;
			SetPolyIndices(nPolyVerts);
		}
		aSrcVertCounts.erase_front(nPolygon);

		cryMemcpy(aVertices.grow_raw(nVerts), aSrcVerts.begin(), sizeof(SVertexParticle) * nVerts);
		aSrcVerts.erase_front(nVerts);
	}
};

struct IAllocRender: SRenderVertices
{
	bool		bDirect;
	bool		bGeomShader;

	IAllocRender()
		: bDirect(false), bGeomShader(false)
	{}

	// Render existing SVertices, alloc new ones.
	virtual void Alloc( int nAllocVerts, int nAllocInds = 0 ) = 0; 
	virtual CREParticle* RenderElement() const { return 0; }
	virtual ~IAllocRender(){}
};

struct SParticleRenderContext
{
	Vec3		m_vCamPos;
	Vec3		m_vCamDir;
	float		m_fAngularRes;		// Pixels per radian
	uint16	m_nWidth;					// Screen dimensions.
	uint16	m_nHeight;
	bool		m_bOctogonal;
};

UNIQUE_IFACE struct IParticleVertexCreator
{
	// Create the vertices for the particle emitter.
	virtual void ComputeVertices( SParticleRenderContext& context, IAllocRender& alloc, bool bIsParticleThread = false ) = 0;
	virtual float GetDistSquared( const Vec3& vPos ) const = 0;
	virtual uint32 GetRenderOrder() const = 0;

	// Reference counting.
	virtual void AddRef() = 0;
	virtual void Release() = 0;
	virtual ~IParticleVertexCreator(){}
};

class CREParticle : public CRendElementBase
{
public:
	CREParticle( IParticleVertexCreator* pVC, const SParticleRenderContext& context );
	void Reset( IParticleVertexCreator* pVC, const SParticleRenderContext& context );

	// Custom copy constructor required to avoid m_Lock copy.
	CREParticle( const CREParticle& in )
	: m_pVertexCreator(in.m_pVertexCreator)
	, m_ParticleComputed(in.m_ParticleComputed)
	, m_Context(in.m_Context)
	, m_fPixels(0.f)
#if defined(PARTICLE_RENDER_ORDER)
	, m_nRenderOrder(0)
#endif
	{
	}

	virtual void GetMemoryUsage(ICrySizer *pSizer) const 
	{
		//pSizer->AddObject(this, sizeof(*this)); // allocated in own allocator
	}
	// CRendElement implementation.
	static CREParticle* Create( IParticleVertexCreator* pVC, const SParticleRenderContext& context, int threadList );
	static void ClearSPUQueue();
	static void PushAllOntoQueue(int threadList, CREParticle** particles, size_t numParticles);
	static void WakeUp();
	static void SetParticleFillThread(int nThreadList);
	static int GetParticleFillThread() { return m_nParticleFillThread; }

	virtual CRendElementBase* mfCopyConstruct()
	{
		return new CREParticle(*this);
	}
  virtual ~CREParticle();
  virtual int Size()
	{
		return sizeof(*this);
	}

	virtual void mfPrepare(bool bCheckOverflow);
	virtual float mfDistanceToCameraSquared( Matrix34& matInst );

	virtual bool mfPreDraw( SShaderPass *sl );
	virtual bool mfDraw( CShader *ef, SShaderPass *sl );

	// Additional methods.
	void PushOntoQueue(int threadList);

	void StoreVertices( bool bWait, bool bParticleThread = false );
	void TransferVertices() const;

	void SetVertices( Array<SVertexParticle> aVerts, Array<uint16> aVertCounts, float fPixels )
	{
		m_aVerts = aVerts;
		m_aVertCounts = aVertCounts;
		assert(m_aVerts.empty() || !m_aVertCounts.empty());
		m_fPixels = fPixels;
	}

	bool operator< (const CREParticle& r) const
	{
#if defined(PARTICLE_RENDER_ORDER)	
		return m_nRenderOrder < r.m_nRenderOrder;
#else
		return false;
#endif
	}







	
	void ResetVertexCreator()
	{
		m_pVertexCreator = NULL;
	}

private:
	CryCriticalSectionNonRecursive			m_Lock;							// Serialises access to vertex creator and verts.
	IParticleVertexCreator*							m_pVertexCreator;		// Particle object which computes vertices.
	volatile bool												m_ParticleComputed;
	SParticleRenderContext							m_Context;					// Camera position and resolution.
	Array<SVertexParticle>							m_aVerts;						// Computed particle vertices.
	Array<uint16>												m_aVertCounts;			// Verts in each particle (multi-seg particles only).
	float																m_fPixels;					// Total pixels rendered.
#if defined(PARTICLE_RENDER_ORDER)
	uint32															m_nRenderOrder;			// Copied from VertexCreator upon assignment.
#endif

	static int													m_nParticleFillThread; // Thread ID used for the particle thread this frame




	bool Lock(const bool bWait)
	{



		if (bWait)
		{
			m_Lock.Lock();
			return true;
		}
		return m_Lock.TryLock();

	}
	void Unlock()
	{
#if !defined(__SPU__)
		m_Lock.Unlock();
#endif // !SPU
	}
};

#endif  // __CREPARTICLE_H__
