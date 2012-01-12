 
#ifndef __CREDUMMY_H__
#define __CREDUMMY_H__

//=============================================================

class CREDummy : public CRendElementBase
{
  friend class CRender3D;

public:

  CREDummy()
  {
    mfSetType(eDATA_Dummy);
    mfUpdateFlags(FCEF_TRANSFORM);
  }

  virtual ~CREDummy()
  {
  }

	virtual void GetMemoryUsage(ICrySizer *pSizer) const 
	{
		pSizer->AddObject(this, sizeof(*this));
	}

  virtual void mfPrepare(bool bCheckOverflow);
  virtual bool mfDraw(CShader *ef, SShaderPass *sfm);
};

#endif  // __CREMotionBlur_H__
