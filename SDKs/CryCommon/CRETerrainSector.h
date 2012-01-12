
#ifndef __CRECommon_H__
#define __CRECommon_H__

//=============================================================
//class CTerrain;

class CRECommon : public CRendElementBase
{
  friend class CRender3D;

public:

  CRECommon()
  {
    mfSetType(eDATA_TerrainSector);
    mfUpdateFlags(FCEF_TRANSFORM);
  }

  virtual ~CRECommon()
  {
  }

  virtual void mfPrepare(bool bCheckOverflow);
	virtual bool mfDraw(CShader *ef, SShaderPass *sfm) { return true; }

	virtual void GetMemoryUsage(ICrySizer *pSizer) const 
	{
		pSizer->AddObject(this, sizeof(*this));
	}
};

class CREFarTreeSprites : public CRendElementBase
{
public:
  PodArray<struct SVegetationSpriteInfo>* m_arrVegetationSprites[2][2];

  CREFarTreeSprites()
  {
    mfSetType(eDATA_FarTreeSprites);
    mfUpdateFlags(FCEF_TRANSFORM);
    for (int i=0; i<2; i++)
    {
      m_arrVegetationSprites[i][0] = NULL;
      m_arrVegetationSprites[i][1] = NULL;
    }
  }
  virtual bool mfDraw(CShader *ef, SShaderPass *sfm);
  virtual void mfPrepare(bool bCheckOverflow);
  virtual float mfDistanceToCameraSquared(Matrix34& matInst) { return 999999999.0f; }

	virtual void GetMemoryUsage(ICrySizer *pSizer) const 
	{
		pSizer->AddObject(this, sizeof(*this));
	}
};

class CRETerrainDetailTextureLayers: public CRECommon
{
public:
  CRETerrainDetailTextureLayers()
  {
    mfSetType(eDATA_TerrainDetailTextureLayers);
    mfUpdateFlags(FCEF_TRANSFORM);
  }
  virtual bool mfDraw(CShader *ef, SShaderPass *sfm);
};

class CRETerrainParticles: public CRECommon
{
public:
  CRETerrainParticles()
  {
    mfSetType(eDATA_TerrainParticles);
    mfUpdateFlags(FCEF_TRANSFORM);
  }
  virtual bool mfDraw(CShader *ef, SShaderPass *sfm);
};

class CREShadowMapGen: public CRECommon
{
public:

  CREShadowMapGen()
  {
    mfSetType(eDATA_ShadowMapGen);
    mfUpdateFlags(FCEF_TRANSFORM);
  }

	virtual bool mfDraw(CShader *ef, SShaderPass *sfm) { return true; }
};


#endif  // __CRECommon_H__
