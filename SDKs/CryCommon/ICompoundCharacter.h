/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2007-2008.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Compound Character Interface

-------------------------------------------------------------------------
History:
- 21:03:2008   21:00 : Created by Stas Spivakov

*************************************************************************/
#include DEVIRTUALIZE_HEADER_FIX(ICompoundCharacter.h)

#ifndef __ICOMPOUNDCHARACTER_H__
#define __ICOMPOUNDCHARACTER_H__

#pragma once

enum EGender
{
	GENDER_MALE,
	GENDER_FEMALE,
	//
	SerGENDER_FIRST = GENDER_MALE,
	SerGENDER_LAST = GENDER_FEMALE,
};

//-------------------------------------------------------------------------
UNIQUE_IFACE struct ICompoundCharacter
{
public:
	virtual ~ICompoundCharacter(){}
  enum EFlags
  {
    eFlag_Default = 0,
    eFlag_CommitOnCache = BIT(0),
  };
  enum ECacheFlags
  {
    eCacheFlag_Default = 0,
    eCacheFlag_DoNotCachePhysics  = BIT(0),
    eCacheFlag_CacheMorphsOnly    = BIT(1),
    eCacheFlag_DoNotMergeTextures = BIT(2),
    eCacheFlag_Verbose            = BIT(3),
  };
public:
  //
  virtual ICharacterInstance* GetCharacterInstance() = 0;
  //
  virtual const char* GetName() const = 0;
  //
  virtual void SetFatness(const float& fFatness) = 0;
  virtual float GetFatness() const = 0;
  //
  virtual void SetNeckFatness(const float& fFatness) = 0;
  virtual float GetNeckFatness() const = 0;
	//
	virtual void SetScale(const float& fScale) = 0;
	virtual float GetScale() const = 0;
  //
  virtual void SetGender(const EGender& gender) = 0;
  virtual EGender GetGender() const = 0;
  // Parts attachment and detachment
  virtual bool AttachPart(const char* partName, const char* material,const char* surface_type=NULL) = 0;
  virtual int GetNumSocketAttachments(const char* partName) const = 0;
  virtual const char* GetSocketName(const char* partName,int socketIndex) const = 0;
  virtual const char* GetSocketFilename(const char* partName,int socketIndex) const = 0;
  virtual bool AddSocketAttachment(const char* partName,const char* socketName,const char* BindingPath) = 0;
  virtual bool HasFaceAttachment(const char* partName,const char* pocketName) = 0;
  virtual bool AddFaceAttachment(const char* partName,const char* pocketName,const char* BindingPath,const QuatT& pt,bool bUseCenterPointDist) = 0;
  virtual bool MoveFaceAttachment(const char* partName,const char* pocketName,const QuatT& pt,bool bUseCenterPointDist) = 0;
  // 
  virtual void DetachPart(const char* name) = 0;
  virtual void DetachAllParts() = 0;

  virtual void ReAttachPart(const char* name) = 0;

  virtual void ChangeAttachedPartMaterial(const char* partName, const char* material) = 0;
  virtual const char* GetAssignedMaterial(const char* partName) = 0;
  // sets/gets surface type for physics proxy
  virtual void SetSurfaceType(const char* partName, const char* surface_type) = 0;
  virtual const char* GetSurfaceType(const char* partName) = 0;

  virtual void ApplyVisibilityMask(int mask) = 0;

  // Attached parts info
  virtual int GetNumAttached() const = 0;
  virtual const char* GetAttachedPartName(int index) const = 0;
  //
  virtual void AddRef() = 0;
  virtual void Release() = 0;
  //
  virtual void Cache(bool bScheduleOnly,ECacheFlags flags=eCacheFlag_Default) = 0;
  //
  virtual void CacheCHRs(bool bCache) = 0;
	virtual void SetCustomHeadName(const char* headName) = 0;
	virtual const string& GetCustomHeadName() const = 0;
};

#endif // __ICOMPOUNDCHARACTER_H__
