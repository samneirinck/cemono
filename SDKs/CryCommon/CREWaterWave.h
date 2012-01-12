#ifndef _CREWATERWAVE_
#define _CREWATERWAVE_

class CREWaterWave : public CRendElementBase
{
public:
	CREWaterWave();

	virtual ~CREWaterWave();
	virtual void mfPrepare(bool bCheckOverflow);
	virtual bool mfDraw( CShader* ef, SShaderPass* sfm );
	virtual float mfDistanceToCameraSquared( Matrix34& matInst );
  virtual void mfGetPlane(Plane& pl);
  virtual void mfCenter(Vec3& vCenter, CRenderObject *pObj);

	virtual void GetMemoryUsage(ICrySizer *pSizer) const 
	{
		pSizer->AddObject(this, sizeof(*this));
	}

public:
	struct SParams
	{
		SParams(): m_pVertices( 0 ), m_pIndices( 0 ), m_nVertices( 0 ), m_nIndices( 0 ), m_pCenter( 0, 0, 0 ),
      m_fSpeed( 1.0f ) , m_fSpeedVar( 0.0f ), m_fLifetime( 1.0f ) , m_fLifetimeVar( 0.0f ), m_fposVar( 0.0f )
		{
		}

		const SVF_P3F_C4B_T2F* m_pVertices;
		const uint16* m_pIndices;

		size_t m_nVertices;
		size_t m_nIndices;

		Vec3 m_pCenter;

    float m_fSpeed, m_fSpeedVar;
    float m_fLifetime, m_fLifetimeVar;
    float m_fposVar;
	};

public:

  const SParams* m_pParams;

};


#endif