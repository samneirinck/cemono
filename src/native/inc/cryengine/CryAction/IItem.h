/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2004.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Item interface.

-------------------------------------------------------------------------
History:
- 29:11:2005   13:52 : Created by Márcio Martins

*************************************************************************/
#ifndef __IITEM_H__
#define __IITEM_H__

#pragma once

#include <IGameObject.h>

/*
enum EItemUpdateSlots
{
	eIUS_General = 0,
	eIUS_Zooming,
	eIUS_FireMode,  
	eIUS_Scheduler,
	eIUS_Last,
};
*/
enum EViewMode
{
	eIVM_Hidden = 0,
	eIVM_FirstPerson = 1,
	eIVM_ThirdPerson = 2,
};

struct IWeapon;
struct IParticleEmitter;
struct SAttachmentHelper;


struct IItemBox //: public IGameObjectExtension
{
	virtual ~IItemBox(){}
	//-------------------- OWNER --------------------
	//virtual void SetOwnerId(EntityId ownerId) = 0;
	virtual EntityId GetOwnerId() const = 0;
	virtual bool IsOwnerFP() = 0;
	virtual IEntity *GetOwner() const = 0;

	virtual void SetParentId(EntityId parentId) = 0;

	//-------------------- PHYSICALIZE --------------------
	virtual void Physicalize(bool enable, bool rigid) = 0;
	//virtual void Impulse(const Vec3 &position, const Vec3 &direction, float impulse) = 0;

	//-------------------- HOLDING --------------------
	virtual void Drop(float impulseScale=1.0f, bool selectNext=true, bool byDeath=false) = 0;
	virtual void PickUp(EntityId picker, bool sound, bool select=true, bool keepHistory=true) = 0;
	//virtual void Pickalize(bool enable, bool dropped) = 0;
	virtual bool CanPickUp(EntityId userId) const = 0;
	virtual bool CanDrop() const = 0;
	virtual bool IsNoDrop() const = 0;
	virtual bool IsCurrentItem() = 0;
	virtual void FillAmmo() = 0;//bad code, remove this!!!

	//-------------------- MOUNTED --------------------
	virtual bool IsMountable() const = 0;
	virtual bool IsMounted() const = 0;
	virtual bool IsRippable() const = 0;
	virtual bool IsRippedOff() const = 0;
	virtual void MountAt(const Vec3 &pos) = 0;
	virtual void MountAtEntity(EntityId entityId, const Vec3 &pos, const Ang3 &angles) = 0;
	virtual Vec3 GetMountedAngleLimits() const = 0;
	virtual Vec3 GetMountedDir() const = 0;
	virtual void SetMountedAngleLimits(float min_pitch, float max_pitch, float yaw_range) = 0;

	//-------------------- ATTACHMENT --------------------
	virtual void ResetCharacterAttachment(int slot, const char *name) = 0;
	virtual void SetCharacterAttachmentLocalTM(int slot, const char *name, const Matrix34 &tm) = 0;
	virtual bool AttachToHand(bool attach, bool checkAttachment = false) = 0;
	virtual bool AttachToBack(bool attach) = 0;
	//virtual const char* CurrentAttachment(const char* attachmentPoint) = 0;
	//virtual const SAttachmentHelper* GetAttachmentHelper(int idx) = 0;
	//virtual int GetAttachmentHelpersCount() = 0;
	virtual void ReAttachAccessories() = 0;

	/*virtual uint32 GetSocketsCount() const = 0;
	virtual const char* GetSocketBone(uint32 index) const = 0;
	virtual uint32 GetSocketTypesCount(uint32 socketId) const = 0;
	virtual const char* GetSocketType(uint32 socketId, uint32 typeId) const = 0;
	virtual const QuatT& GetHelperPos(uint32 socketId, uint32 typeId, bool local = false) const = 0;
	virtual void SetHelperPos(uint32 socketId, uint32 typeId, const QuatT& pos) = 0;
	virtual uint32 GetSocketParentSlot(uint32 socketId) const = 0;
	virtual bool IsHelperUsed(uint32 socketId, uint32 typeId) const = 0;
	virtual uint32 GetPinHelpersCount() const = 0;
	virtual const char* GetPinHelperName(uint32 index) const = 0;
	virtual const QuatT& GetPinHelperPos(uint32 index) const = 0;
	virtual void SetPinHelperPos(uint32 index, const QuatT& pos) = 0;*/

	//-------------------- ACCESSORIES --------------------
	//virtual void TakeAccessories(EntityId receiver) = 0;
	virtual void RemoveAllAccessories() = 0;
	virtual uint32 GetAccessoriesCount() const = 0;
	virtual EntityId GetAccessoryId(uint32 index) const = 0;
	virtual const char* GetAccessoryAttachHelper(const char* name) = 0;
	virtual const char* GetAccessoryName(uint32 index) const = 0;
	virtual void SwitchAccessory(const char* accessory) = 0;
	virtual bool AttachAccessory(const char* name, bool attach, bool noanim, bool force = false, bool initialSetup = false) = 0;
	virtual void AttachAccessoryPlaceHolder(const char* name, bool attach) = 0;
	virtual bool HasAccessoryParams(const char* name) const = 0;
	virtual void RemoveAccessory(const char* name) = 0;
	virtual void RemoveBonusAccessoryAmmo() = 0;
	virtual void AccessoriesChanged() = 0;
	virtual IItem *GetAccessory(const char* name) = 0;
	virtual IItem *GetAccessoryPlaceHolder(const char* name) = 0;

	virtual bool IsFP() const = 0;
	virtual int GetScopeAttachment() const = 0;

	virtual void Hide(bool hide) = 0;

	virtual void EnterWater(bool enter) = 0;

	virtual void Cloak(bool cloak, IMaterial *cloakMat = 0) = 0;
	virtual void CloakEnable(bool enable, bool fade) = 0;
	virtual void CloakSync(bool fade) = 0;

	virtual void Freeze(bool freeze) = 0;
	virtual void FrostSync(bool fade) = 0;

	virtual void WetSync(bool fade) = 0;	
};

struct IItemEdit
{
	struct SItemHelper
	{
		const char* name;
		const char* bone;
		int		slot;
	};
	
	virtual ~IItemEdit(){}
	virtual void BindToEditor(bool bind) = 0;
	virtual uint32 GetSocketsCount() const = 0;
	virtual const char* GetSocketName(uint32 socketId) const = 0;
	virtual const char* GetSocketBone(uint32 index) const = 0;
	virtual uint32 GetSocketTypesCount(uint32 socketId) const = 0;
	virtual const char* GetSocketType(uint32 socketId, uint32 typeId) const = 0;
	virtual const QuatT& GetHelperPos(uint32 socketId, uint32 typeId, bool local = false) const = 0;
	virtual void SetHelperPos(uint32 socketId, uint32 typeId, const QuatT& pos) = 0;
	virtual uint32 GetSocketParentSlot(uint32 socketId) const = 0;
	virtual bool IsHelperUsed(uint32 socketId, uint32 typeId) const = 0;
	virtual uint32 GetPinHelpersCount() const = 0;
	virtual const char* GetPinHelperName(uint32 index) const = 0;
	virtual const QuatT& GetPinHelperPos(uint32 index) const = 0;
	virtual void SetPinHelperPos(uint32 index, const QuatT& pos) = 0;
	virtual int GetBoneHelperCount() const = 0;
	virtual bool GetBoneHelper(int idx, SItemHelper& hlpInfo) const = 0;
	virtual bool SetBoneHelper(int idx, const SItemHelper& hlpInfo) = 0;
	virtual const char* GetSlotGeometry(int slot) const = 0;
	virtual void SetSlotGeometry(int slot, const char* name) = 0;
	virtual void SetViewMode(EViewMode mode) = 0;
};

struct IItemView 
{
	enum eGeometrySlot
	{
		eIGS_FirstPerson = 0,
		eIGS_ThirdPerson,
		eIGS_Arms,
		eIGS_Aux0,
		eIGS_Owner,
		eIGS_OwnerLooped,
		eIGS_OffHand,
		eIGS_Destroyed,
		eIGS_Aux1,
		eIGS_ThirdPersonAux,
		eIGS_Last,
	};

	enum ePlayActionFlags
	{
		eIPAF_FirstPerson				= 1 << eIGS_FirstPerson,
		eIPAF_ThirdPerson				= 1 << eIGS_ThirdPerson,
		eIPAF_Arms							= 1 << eIGS_Arms,
		eIPAF_Aux0							= 1 << eIGS_Aux0,
		eIPAF_Owner							= 1 << eIGS_Owner,
		eIPAF_OwnerLooped					= 1 << eIGS_OwnerLooped,
		eIPAF_Destroyed         = 1 << eIGS_Destroyed,
		eIPAF_ForceFirstPerson	= 1 << 15,
		eIPAF_ForceThirdPerson	= 1 << 16,
		eIPAF_NoBlend						= 1 << 17,
		eIPAF_CleanBlending			= 1 << 18,
		eIPAF_Animation					= 1 << 19,
		eIPAF_Sound							= 1 << 20,
		eIPAF_SoundLooped				= 1 << 21,
		eIPAF_SoundStartPaused	= 1 << 22,
		eIPAF_RestartAnimation	= 1 << 23,
		eIPAF_RepeatLastFrame	  = 1 << 24,
		eIPAF_Effect            = 1 << 25,
		eIPAF_Default						= eIPAF_FirstPerson|eIPAF_ThirdPerson|eIPAF_Owner|eIPAF_OwnerLooped|eIPAF_Sound|eIPAF_Animation|eIPAF_Effect,
	};
	virtual ~IItemView(){}

	//-------------------- SOUND --------------------
	virtual void EnableSound(bool enable) = 0;
	virtual bool IsSoundEnabled() const = 0;
	virtual IEntitySoundProxy *GetSoundProxy(bool create=false) = 0;
	virtual void StopSound(tSoundID id) = 0;
	virtual void Quiet() = 0;
	virtual ISound *GetISound(tSoundID id) = 0;

	//-------------------- ANIMATIONS --------------------
	virtual void SetActionSuffix(const char *suffix, const char *suffixAG = NULL) = 0;

	virtual void PlayLayer(const char* name, int flags = eIPAF_Default, bool record=true) = 0;
	virtual void RestoreLayers() = 0;
	virtual void SetDefaultIdleAnimation(int slot, const char* actionName) = 0;
	virtual void PlayAnimation(const char* animationName, int layer=0, bool loop=false, uint32 flags = eIPAF_Default) = 0;
	virtual void EnableAnimations(bool enable) = 0;
	virtual void StopLayer(const char* name, int flags = eIPAF_Default, bool record=true) = 0;
	virtual void ResetAnimation(int layer=0, uint32 flags = eIPAF_Default) = 0;

	//-------------------- VIEW --------------------
	virtual void UpdateFPView(float frameTime) = 0;
	virtual bool FilterView(struct SViewParams &viewParams) = 0;
	virtual void PostFilterView(struct SViewParams &viewParams) = 0;
	virtual Matrix34 GetEffectWorldTM(uint32 it) = 0;
	virtual void SetEffectWorldTM(uint32 id, const Matrix34 &tm) = 0;
	virtual IParticleEmitter *GetEffectEmitter(uint32 id) const = 0;
	virtual uint32 AttachEffect(int slot, uint32 id, bool attach, const char *effectName=0, const char *helper=0,
		const Vec3 &offset=Vec3Constants<float>::fVec3_Zero, const Vec3 &dir=Vec3Constants<float>::fVec3_OneY, float scale=1.0f, bool prime=true) = 0;
	virtual void SpawnEffect(int slot, const char *effectName, const char *helper, const Vec3 &offset=Vec3Constants<float>::fVec3_Zero,
		const Vec3 &dir=Vec3Constants<float>::fVec3_OneY, float scale=1.0f) = 0;
	virtual uint32 AttachLight(int slot, uint32 id, bool attach, float radius=5.0f, const Vec3 &color=Vec3Constants<float>::fVec3_One,
		const float fSpecularMult=1.0f, const char *projectTexture=0, float projectFov=0, const char *helper=0,
		const Vec3 &offset=Vec3Constants<float>::fVec3_Zero, const Vec3 &dir=Vec3Constants<float>::fVec3_OneY, 
		const char* material=0, float fHDRDynamic=0.f) = 0;
	virtual void EnableLight(bool enable, uint32 id) = 0;
	virtual uint32 AttachLightEx(int slot, uint32 id, bool attach, bool fakeLight = false , bool castShadows = false, IRenderNode* pCasterException = NULL, 
		float radius=5.0f, const Vec3 &color=Vec3Constants<float>::fVec3_One, const float fSpecularMult=1.0f, const char *projectTexture=0, 
		float projectFov=0, const char *helper=0, const Vec3 &offset=Vec3Constants<float>::fVec3_Zero, const Vec3 &dir=Vec3Constants<float>::fVec3_OneY, 
		const char* material=0, float fHDRDynamic=0.f) = 0;
	virtual void SetLightRadius(float radius, uint32 id) = 0;
	virtual Vec3 GetSlotHelperPos(int slot, const char *helper, bool worldSpace, bool relative=false) = 0;
	virtual const Matrix33 &GetSlotHelperRotation(int slot, const char *helper, bool worldSpace, bool relative=false) = 0;
	virtual void DrawSlot(int slot, bool draw, bool near=false) = 0;
	virtual void SetViewMode(int mode) = 0;
	virtual int GetViewMode() const = 0;
	virtual void ApplyViewLimit(EntityId userId, bool apply) = 0;
	virtual void ForceSkinning(bool always) = 0;
	virtual float GetNearFov() const = 0;
	virtual const char* GetDofMask() const = 0;
	virtual const char* GetBlurMask() const = 0;
};

// Summary
//   Interface to implement a new Item class
struct IItem : public IGameObjectExtension
{
	enum eItemBackAttachment
	{
		eIBA_Primary = 0,
		eIBA_Secondary,
		eIBA_Unknown
	};

	enum eItemHand
	{
		eIH_Right = 0, // indicates the right hand of the actor
		eIH_Left, // indicates the left hand of the actor
		eIH_Last,
	};

	// Summary
	//   Retrieves the ownwer id
	virtual EntityId GetOwnerId() const = 0;

	virtual void EnableUpdate(bool enable, int slot=-1) = 0;
	virtual void RequireUpdate(int slot) = 0;

	//-------------------- ACTIONS --------------------
	//virtual tSoundID PlayAction(const char* action, int layer=0, bool loop=false, uint32 flags = IItemView::eIPAF_Default, float speedOverride = -1.0f) = 0;
	//virtual uint32 GetActionDuration(const char* action) = 0;
	virtual void ForcePendingActions(uint8 blockedActions = 0) = 0;
	//virtual bool HasAction(const char* action) = 0;

	//-------------------- EVENTS --------------------
	virtual void OnAction(EntityId actorId, const ActionId& actionId, int activationMode, float value) = 0;
	virtual void OnParentSelect(bool select) = 0;
	virtual void OnAttach(bool attach) = 0;
	virtual void OnPickedUp(EntityId actorId, bool destroyed) = 0;
	virtual void OnHit(float damage, const char* damageType) = 0;

	//-------------------- HOLDING --------------------
	virtual void Select(bool select) = 0;
	//virtual void SetFirstSelection(bool firstSelection) = 0;
	//virtual bool IsFirstSelection() const = 0;
	virtual bool IsSelected() const = 0;
	virtual void EnableSelect(bool enable) = 0;
	virtual bool CanSelect() const = 0;
	virtual bool CanDeselect() const = 0;

	virtual void Physicalize(bool enable, bool rigid) = 0;
	virtual bool CanDrop() const = 0;
	virtual void Drop(float impulseScale=1.0f, bool selectNext=true, bool byDeath=false) = 0;
	virtual void UpdateFPView(float frameTime) = 0;
	virtual Vec3 GetMountedAngleLimits() const = 0;
	virtual void PickUp(EntityId picker, bool sound, bool select=true, bool keepHistory=true, const char *setup = NULL) = 0;
	virtual void MountAtEntity(EntityId entityId, const Vec3 &pos, const Ang3 &angles) = 0;
	virtual bool FilterView(struct SViewParams &viewParams) = 0;
	virtual void RemoveAllAccessories() = 0;
	//virtual void FillAmmo() = 0;//bad code, remove this!!!

	virtual void SetHand(int hand) = 0;

	virtual void StartUse(EntityId userId) = 0;
	virtual void StopUse(EntityId userId) = 0;
	virtual void SetBusy(bool busy) = 0;
	virtual bool IsBusy() const = 0;
	virtual void Cloak(bool cloak, IMaterial *cloakMat = 0) = 0;
	virtual bool CanUse(EntityId userId) const = 0;
	virtual bool IsUsed() const = 0;
	virtual void Use(EntityId userId) = 0;

	virtual bool AttachToHand(bool attach, bool checkAttachment = false) = 0;
	virtual bool AttachToBack(bool attach) = 0;

	//-------------------- DUELD WIELD --------------------
	virtual bool SupportsDualWield(const char *itemName) const = 0;
	virtual void ResetDualWield() = 0;
	virtual IItem *GetDualWieldSlave() const = 0;
	//virtual EntityId GetDualWieldSlaveId() const = 0;
	virtual IItem *GetDualWieldMaster() const = 0;
	virtual EntityId GetDualWieldMasterId() const = 0;
	virtual void SetDualWieldMaster(EntityId masterId) = 0;
	virtual void SetDualWieldSlave(EntityId slaveId) = 0;
	virtual bool IsDualWield() const = 0;
	virtual bool IsDualWieldMaster() const = 0;
	virtual bool IsDualWieldSlave() const = 0;
	//virtual bool IsTwoHand() const = 0;
	virtual void SetDualSlaveAccessory(bool noNetwork = false) = 0;

	//-------------------- SETTINGS --------------------
	//virtual bool IsProneUsable() const = 0;
	//virtual bool IsTacticalAllowed() const = 0;
	virtual bool IsModifying() const = 0;
	virtual int TwoHandMode() const = 0;
	//virtual float GetRiseDistance() const = 0;
	//virtual float GetMass() const = 0;
	//virtual int GetHitPoints() const = 0;
	//virtual float GetHealth() const = 0;
	virtual bool CheckAmmoRestrictions(EntityId pickerId) = 0;
	virtual void Reset() = 0;
	virtual bool ResetParams() = 0;
	virtual void PreResetParams() = 0;
	virtual bool GivesAmmo() = 0;
	virtual const char *GetDisplayName() const = 0;
	virtual void HideItem(bool hide) = 0;

	//-------------------- INTERFACES --------------------
	virtual IWeapon *GetIWeapon() = 0;
	virtual const IWeapon *GetIWeapon() const = 0;
	//virtual IItemBox *GetIItemBox() const = 0;
	//virtual IItemView *GetIItemView() const = 0;
	//virtual IItemEdit *GetIItemEdit() const = 0;

	// Summary
	// Returns
	//   true if the item is an accessory
	virtual bool IsAccessory() = 0;

	//used to serialize item attachment when loading next level
	virtual void SerializeLTL( TSerialize ser ) = 0;

	//virtual bool SupportItemProxy() const = 0;
	
	//-------------------- REPLAYES --------------------
	//virtual void LoadReplay(const char* path) = 0;
	//virtual void PlayReplay() = 0;

	// gets the original (not current) direction vector of a mounted weapon
	virtual Vec3 GetMountedDir() const = 0;
};

#endif // __IITEM_H__
