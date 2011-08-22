/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2007-2008.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Character parts manager interface.

-------------------------------------------------------------------------
History:
- 10:12:2007   15:30 : Created by Maxim Dyachenko

*************************************************************************/
#ifndef __ICHARACTERPARTSMANAGER_H__
#define __ICHARACTERPARTSMANAGER_H__

#include DEVIRTUALIZE_HEADER_FIX(ICharacterPartsManager.h)

#pragma once

struct IAttachmentManager;
struct ICharacterPartAttachment;

//-------------------------------------------------------------------------

#include "ICompoundCharacter.h"

//-------------------------------------------------------------------------
UNIQUE_IFACE struct IPartMorph
{
	virtual ~IPartMorph(){}
  // Morph switch
  virtual const char* GetSwitch() const = 0;
  virtual void SetSwitch(const char* switchName) = 0;

  // Morph target inside model
  virtual const char* GetMorphTarget() const = 0;
  virtual void SetMorphTarget(const char* morphTarget) = 0;

  // Set
  void Set(const IPartMorph* op)
  {
    SetSwitch(op->GetSwitch());
    SetMorphTarget(op->GetMorphTarget());
  }
};

//-------------------------------------------------------------------------
UNIQUE_IFACE struct IPartMaterial
{
	virtual ~IPartMaterial(){}
  // Material name
  virtual const char* GetName() const = 0;
  virtual void SetName(const char* name) = 0;

  // Material filename
  virtual const char* GetFilename() const = 0;
  virtual void SetFilename(const char* filename) = 0;

  // Set
  void Set(const IPartMaterial* op)
  {
    SetName(op->GetName());
    SetFilename(op->GetFilename());
  }
};

//-------------------------------------------------------------------------
UNIQUE_IFACE struct IPartSocket
{
	virtual ~IPartSocket(){}
  // Socket name
  virtual const char* GetName() const = 0;
  virtual void SetName(const char* name) = 0;

  // Edge of socket axis
  virtual int GetEdge() const = 0;
  virtual void SetEdge(int edge) = 0;

  // Limit angle
  virtual float GetAngle() const = 0;
  virtual void SetAngle(float angle) = 0;

  // Damping
  virtual float GetDamping() const = 0;
  virtual void SetDamping(float damping) = 0;

  // Allowed socket attachment types
  virtual int GetNumAllowedTypes() const = 0;
  virtual const char* GetAllowedTypeByIndex(int index) const = 0;

  virtual void ClearAllowedTypes() = 0;
  virtual void ReserveAllowedTypes(int size) = 0;
  virtual void AddAllowedType(const char* name) = 0;

  // Set
  void Set(const IPartSocket* op)
  {
    SetName(op->GetName());
    SetEdge(op->GetEdge());
    SetAngle(op->GetAngle());
    SetDamping(op->GetDamping());

    ClearAllowedTypes();
    ReserveAllowedTypes(op->GetNumAllowedTypes());
    for (int i = 0; i < op->GetNumAllowedTypes(); ++i)
      AddAllowedType(op->GetAllowedTypeByIndex(i));
  }
};

//-------------------------------------------------------------------------
UNIQUE_IFACE struct ICharacterPartTemplate
{
	virtual ~ICharacterPartTemplate(){}
  // Morphs
  virtual int GetNumMorphs(EGender gender) const = 0;
  virtual IPartMorph* GetMorphByIndex(EGender gender, int index) = 0;
  virtual const IPartMorph* GetMorphByIndex(EGender gender, int index) const = 0;

  // Switches
  virtual int GetNumSwitches(EGender gender) const = 0;
  virtual const char* GetSwitchByIndex(EGender gender, int index) const = 0;

  // Template filename
  virtual const char* GetFilename() const = 0;

  // Template name
  virtual const char* GetName() const = 0;

  // Part visibility mask
  virtual uint32 GetVisibilityMask() const = 0;
};

//-------------------------------------------------------------------------
UNIQUE_IFACE struct IPartModel
{
	virtual ~IPartModel(){}
  // Model filename (.CHR)
  virtual const char* GetFilename() const = 0;
  virtual void SetFilename(const char* filename) = 0;

	// Model FaceGenTemplate filename (.FGT)
	virtual const char* GetFGTFilename() const = 0;
	virtual void SetFGTFilename(const char* filename) = 0;

  // Model pockets mask filename (.tiff)
  virtual const char* GetPocketsMaskFilename() const = 0;
  virtual void SetPocketsMaskFilename(const char* filename) = 0;

  // Morphs
  virtual int GetNumMorphs() const = 0;
  virtual IPartMorph* GetMorphByIndex(int index) = 0;
  virtual const IPartMorph* GetMorphByIndex(int index) const = 0;

  virtual void ClearMorphs() = 0;
  virtual void ReserveMorphs(int size) = 0;
  virtual IPartMorph* AddMorph(const char* switchName, const char* morphTarget) = 0;
  virtual void AddMorph(const IPartMorph* source) = 0;
  virtual void InsertMorphAtIndex(int index, const char* switchName, const char* morphTarget) = 0;
  virtual void SetMorphAtIndex(int index, const char* switchName = NULL, const char* morphTarget = NULL) = 0;
  virtual void RemoveMorphAtIndex(int index) = 0;

  virtual void SetMorphArrayPos(int oldPos, int newPos) = 0;

  // Materials
  virtual int GetNumMaterials() const = 0;
  virtual IPartMaterial* GetMaterialByIndex(int index) = 0;
  virtual const IPartMaterial* GetMaterialByIndex(int index) const = 0;

  virtual void ClearMaterials() = 0;
  virtual void ReserveMaterials(int size) = 0;
  virtual IPartMaterial* AddMaterial(const char* name, const char* filename) = 0;
  virtual void AddMaterial(const IPartMaterial* source) = 0;
  virtual void RemoveMaterialAtIndex(int index) = 0;

  // Switches
  virtual int GetNumSwitches() const = 0;
  virtual const char* GetSwitchByIndex(int index) const = 0;

  virtual void ClearSwitches() = 0;
  virtual void ReserveSwitches(int size) = 0;
  virtual void AddSwitch(const char* switchName) = 0;
  virtual void InsertSwitchAtIndex(int index, const char* switchName) = 0;
  virtual void SetSwitchAtIndex(int index, const char* switchName) = 0;
  virtual void RemoveSwitchAtIndex(int index) = 0;

  // Sockets
  virtual int GetNumSockets() const = 0;
  virtual IPartSocket* GetSocketByIndex(int index) = 0;
  virtual const IPartSocket* GetSocketByIndex(int index) const = 0;

  virtual void ClearSockets() = 0;
  virtual void ReserveSockets(int size) = 0;
  virtual IPartSocket* AddSocket(const char* name, int edge, float angle, float damping) = 0;
  virtual void AddSocket(const IPartSocket* source) = 0;

  //-------------------------------------------------------------------------
  // Sets model data from other definition
  void Set(const IPartModel* op)
  {
    SetFilename(op->GetFilename());
		SetFGTFilename(op->GetFGTFilename());
    SetPocketsMaskFilename(op->GetPocketsMaskFilename());

    ClearMorphs();
    ReserveMorphs(op->GetNumMorphs());
    for (int i = 0; i < op->GetNumMorphs(); ++i)
      AddMorph(op->GetMorphByIndex(i));

    ClearMaterials();
    ReserveMaterials(op->GetNumMaterials());
    for (int i = 0; i < op->GetNumMaterials(); ++i)
      AddMaterial(op->GetMaterialByIndex(i));

    ClearSwitches();
    ReserveSwitches(op->GetNumSwitches());
    for (int i = 0; i < op->GetNumSwitches(); ++i)
      AddSwitch(op->GetSwitchByIndex(i));

    ClearSockets();
    ReserveSockets(op->GetNumSockets());
    for (int i = 0; i < op->GetNumSockets(); ++i)
      AddSocket(op->GetSocketByIndex(i));
  }

  //-------------------------------------------------------------------------
  // Sets model data from template
  void Set(const ICharacterPartTemplate* pTemplate, EGender gender)
  {
    ClearMorphs();
    ClearSwitches();

    ReserveMorphs(pTemplate->GetNumMorphs(gender));
    for (int i = 0; i < pTemplate->GetNumMorphs(gender); ++i)
      AddMorph(pTemplate->GetMorphByIndex(gender, i));
    ReserveSwitches(pTemplate->GetNumSwitches(gender));
    for (int i = 0; i < pTemplate->GetNumSwitches(gender); ++i)
      AddSwitch(pTemplate->GetSwitchByIndex(gender, i));
  }
};

//-------------------------------------------------------------------------
UNIQUE_IFACE struct ICharacterPart
{
	virtual ~ICharacterPart(){}
  // Part name
  virtual const char* GetName() const = 0;
  virtual void SetName(const char* name) = 0;

  // Part slot
  virtual const char* GetSlot() const = 0;
  virtual void SetSlot(const char* slot) = 0;

  // Part filename
  virtual const char* GetFilename() const = 0;
  virtual void SetFilename(const char* filename) = 0;

  // Part template
  virtual const char* GetTemplateName() const = 0;
  virtual void SetTemplateName(const char* filename) = 0;

  // Part visibility mask
  virtual uint32 GetVisibilityMask() const = 0;
  virtual void SetVisibilityMask(uint32 mask) = 0;

  // Part model data
  virtual IPartModel* GetModel(EGender gender) = 0;
  virtual const IPartModel* GetModel(EGender gender) const = 0;

  // Copies character part data from other definition
  void Set(const ICharacterPart* op)
  {
    SetName(op->GetName());
    SetSlot(op->GetSlot());
		SetFilename(op->GetFilename());
    SetTemplateName(op->GetTemplateName());
    SetVisibilityMask(op->GetVisibilityMask());

    GetModel(GENDER_MALE)->Set(op->GetModel(GENDER_MALE));
    GetModel(GENDER_FEMALE)->Set(op->GetModel(GENDER_FEMALE));
  }

  // Copies character definition from template
  void Set(const ICharacterPartTemplate* op)
  {
    GetModel(GENDER_MALE)->Set(op, GENDER_MALE);
    GetModel(GENDER_FEMALE)->Set(op, GENDER_FEMALE);

    SetVisibilityMask(op->GetVisibilityMask());
  }
};

//-------------------------------------------------------------------------
UNIQUE_IFACE struct ICharacterPartsManager
{
  virtual ~ICharacterPartsManager() {};
  virtual void ScanForParts(const char* folder) = 0;
  virtual void AddNewPart(const char* filename) = 0;

  // Global parts list
  virtual const ICharacterPart* GetPartByName(const char* name) const = 0;
  virtual const ICharacterPart* GetPartByIndex(int index) const = 0;
  virtual int GetNumParts() const = 0;
  virtual void ReloadPart(const char* name) = 0;
  virtual void RedefinePart(const char* name, const ICharacterPart* pNewPartData) = 0;
  virtual int GetPartIndexByName(const char* name) const = 0;

  // Part templates list
  virtual int GetNumTemplates() const = 0;
  virtual const ICharacterPartTemplate* GetTemplateByName(const char* name) const = 0;
  virtual const ICharacterPartTemplate* GetTemplateByIndex(int index) const = 0;
  // if pInstance is NULL base character will be taken from cpf file
  virtual ICompoundCharacter* LoadCPF(const char* cpfFilename, ICharacterInstance* pInstance,bool bUseCache=false) const = 0;
  // pInstance is a base chr
  virtual ICompoundCharacter* CreateCompoundCharacter(ICharacterInstance* pInstance,const char* dbgname,ICompoundCharacter::EFlags flags = ICompoundCharacter::eFlag_Default) const = 0;
  //
  virtual void PrintDebugInfo() const = 0;
};

#endif // __ICHARACTERPARTSMANAGER_H__
