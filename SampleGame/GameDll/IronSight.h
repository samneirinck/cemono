/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2004.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Iron Sight

-------------------------------------------------------------------------
History:
- 28:10:2005   16:00 : Created by Márcio Martins

*************************************************************************/
#ifndef __IRONSIGHT_H__
#define __IRONSIGHT_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include <IViewSystem.h>
#include "Weapon.h"
#include "ItemParamReader.h"


#define ResetValue(name, defaultValue) if (defaultInit) name=defaultValue; reader.Read(#name, name)
#define ResetValueEx(name, var, defaultValue) if (defaultInit) var=defaultValue; reader.Read(name, var)

//Spread Modifiers (let modify spread per value when zoomed)
typedef struct SSpreadModParams
{
	SSpreadModParams() { Reset(); };
	void Reset(const IItemParamsNode *params=0, bool defaultInit=true)
	{
		CItemParamReader reader(params);
		ResetValue(min_mod,						1.0f);
		ResetValue(max_mod,						1.0f);
		ResetValue(attack_mod,				1.0f);
		ResetValue(decay_mod,					1.0f);
		ResetValue(speed_m_mod,				1.0f);
		ResetValue(rotation_m_mod,			1.0f);

		ResetValue(spread_crouch_m_mod, 1.0f);
		ResetValue(spread_prone_m_mod,  1.0f);
		ResetValue(spread_jump_m_mod,  1.0f);
		ResetValue(spread_zeroG_m_mod, 1.0f);

	}

	float	min_mod;
	float max_mod;
	float attack_mod;
	float decay_mod;
	float speed_m_mod;
	float rotation_m_mod;

	//Stance modifiers
	float								spread_crouch_m_mod;
	float								spread_prone_m_mod;
	float								spread_jump_m_mod;
	float								spread_zeroG_m_mod;

	void GetMemoryUsage(ICrySizer * s) const
	{
	}

} SSpreadModParams;

//Recoil Modifiers (let modify recoil per value when zoomed)
typedef struct SRecoilModParams
{
	SRecoilModParams() { Reset(); };
	void Reset(const IItemParamsNode *params=0, bool defaultInit=true)
	{
		CItemParamReader reader(params);
		ResetValue(max_recoil_mod,	1.0f);
		ResetValue(attack_mod,			1.0f);
		ResetValue(decay_mod,				1.0f);
		ResetValueEx("maxx_mod", max_mod.x,1.0f);
		ResetValueEx("maxy_mod", max_mod.y,1.0f);
		ResetValue(impulse_mod, 1.0f);
		ResetValue(angular_impulse_mod, 1.0f);
		ResetValue(back_impulse_mod, 1.0f);

		ResetValue(recoil_crouch_m_mod, 1.0f);
		ResetValue(recoil_prone_m_mod, 1.0f);
		ResetValue(recoil_jump_m_mod, 1.0f);

		ResetValue(recoil_strMode_m_mod, 1.0f);
		ResetValue(recoil_zeroG_m_mod, 1.0f);

	}

	float								max_recoil_mod;
	float								attack_mod;
	float								decay_mod;
	Vec2								max_mod;
	float               impulse_mod;
	float								angular_impulse_mod;
	float								back_impulse_mod;

	//Stance modifiers
	float								recoil_crouch_m_mod;
	float								recoil_prone_m_mod;
	float								recoil_jump_m_mod;
	float								recoil_zeroG_m_mod;

	//Nano suit modifiers
	float								recoil_strMode_m_mod;

	void GetMemoryUsage(ICrySizer * s) const
	{
	}

} SRecoilModParams;

class CIronSightSharedData;

class CIronSight : public IZoomMode
{
public:
	struct EnterZoomAction;
	struct LeaveZoomAction;
	struct DisableTurnOffAction;
	struct EnableTurnOffAction;

	typedef struct SZoomParams
	{
		SZoomParams(){ Reset(); };
		void Reset(const IItemParamsNode *params=0, bool defaultInit=true)
		{
			CItemParamReader reader(params);

			ResetValue(layer, "zoom_layer");
			ResetValue(suffix, "ironsight");
			ResetValue(suffix_FC, "fc");
			ResetValue(support_FC_IronSight,false);
			ResetValue(alternate_dof_mask, "");
			ResetValue(dof_mask, "");
			ResetValue(blur_amount, 0.0f);
			ResetValue(blur_mask, "");
			ResetValue(sensitivity_ratio, 1.2f);
			ResetValue(hbob_ratio, 1.75f);
			ResetValue(recoil_ratio, 1.0f);

			ResetValue(zoom_in_time, 0.35f);
			ResetValue(zoom_out_time, 0.35f);
			ResetValue(stage_time, 0.055f);
			ResetValue(scope_mode, false);
			ResetValue(scope_nearFov, 60.0f);
			ResetValue(scope_offset,Vec3(0,0,0));
			
			dof = true;
			if (dof_mask.empty() && alternate_dof_mask.empty())
				dof = false;

			if (dof && alternate_dof_mask.empty())
				alternate_dof_mask = dof_mask;

			if (defaultInit)
			{
				stages.resize(0);
				stages.push_back(1.5f);
			}

			if (params)
			{
				const IItemParamsNode *pstages=params->GetChild("stages");
				if (pstages)
				{
					stages.resize(0); 
					int n=pstages->GetChildCount();

					for (int i=0; i<n; i++)
					{
						const IItemParamsNode *stage = pstages->GetChild(i);
						float v = 1.0f; stage->GetAttribute("value", v);
						stages.push_back(v);
					}
				}
			}
		}
		void GetMemoryUsage(ICrySizer * s) const
		{
			s->AddContainer(stages);
			s->Add(layer);
			s->Add(suffix);
			s->Add(suffix_FC);
			s->Add(alternate_dof_mask);
			s->Add(dof_mask);
			s->Add(blur_mask);
		}

		std::vector<float>	stages;
		ItemString					layer;
		ItemString					suffix;
		ItemString					suffix_FC;	//Secondary suffix for Alternative ironsight
		bool								support_FC_IronSight;
		bool								dof;
		ItemString					dof_mask;
		ItemString					alternate_dof_mask;
		float								blur_amount;
		ItemString					blur_mask;
		float								sensitivity_ratio;
		float								hbob_ratio;
		float								recoil_ratio;
		float								zoom_in_time;
		float								zoom_out_time;
		float								stage_time;

		bool								scope_mode;
		Vec3								scope_offset;		//Hard code offset since we don't have "proper" animation
		float								scope_nearFov;
	} SZoomParams;

	typedef struct SZoomActions
	{

		SZoomActions() { Reset(); };
		void Reset(const IItemParamsNode *params=0, bool defaultInit=true)
		{
			CItemParamReader reader(params);
			ResetValue(zoom_in,			"zoom_in");
			ResetValue(zoom_out,		"zoom_out");
			ResetValue(idle,				"idle");
		}

		ItemString	zoom_in;
		ItemString	zoom_out;
		ItemString	idle;

		void GetMemoryUsage(ICrySizer * s) const
		{
			s->Add(zoom_in);
			s->Add(zoom_out);
			s->Add(idle);
		}
	} SZoomActions;

	typedef struct SZoomSway
	{

		SZoomSway() { Reset(); };
		void Reset(const IItemParamsNode *params=0, bool defaultInit=true)
		{
			CItemParamReader reader(params);
			ResetValue(maxX,			0.0f);
			ResetValue(maxY,		  0.0f);
			ResetValue(stabilizeTime,				3.0f);
			ResetValue(strengthScale, 0.6f);
			ResetValue(strengthScaleTime, 0.75f);
			ResetValue(minScale , 0.15f);
			ResetValue(scaleAfterFiring, 0.5f);
			ResetValue(crouchScale, 0.8f);
			ResetValue(proneScale, 0.6f);
		}

		float maxX;
		float maxY;
		float stabilizeTime;
		float strengthScale;
		float strengthScaleTime;
		float minScale;
		float scaleAfterFiring;

		//Stance modifiers
		float								crouchScale;
		float								proneScale;

		void GetMemoryUsage(ICrySizer * s) const
		{
		}
	} SZoomSway;

	CIronSight();
	virtual ~CIronSight();
	virtual void GetMemoryUsage(ICrySizer * s) const;

	// IZoomMode
	virtual void Init(IWeapon *pWeapon, const struct IItemParamsNode *params, uint32 id);
	virtual void Update(float frameTime, uint32 frameId);
	virtual void Release();

	virtual void ResetParams(const struct IItemParamsNode *params);
	virtual void PatchParams(const struct IItemParamsNode *patch);
	virtual void ModifyParams(bool modify, bool modified = true);

	virtual void Activate(bool activate);

	virtual bool CanZoom() const;
	virtual bool StartZoom(bool stayZoomed = false, bool fullZoomout = true, int zoomStep = 1);
	virtual void StopZoom();
	virtual void ExitZoom(bool force=false);

	virtual int GetCurrentStep() const {return m_currentStep;}
	virtual float GetZoomFoVScale(int step) const;

	virtual void ZoomIn();
	virtual bool ZoomOut();

	virtual bool IsZoomed() const;
	virtual bool IsZoomingInOrOut() const;
	inline bool IsZoomingIn() const { return IsZoomingInOrOut() && m_zoomingIn; }
	inline bool IsZoomingOut() const { return IsZoomingInOrOut() && !m_zoomingIn; }
	virtual EZoomState GetZoomState() const;
	virtual float GetZoomInTime() const { return 1.0f; }
	virtual float GetZoomTransition() const;
	
	virtual bool AllowsZoomSnap() const { return false; }

	virtual void Enable(bool enable);
	virtual bool IsEnabled() const;

	virtual void Serialize(TSerialize ser);

	virtual void GetFPOffset(QuatT &offset) const
	{
		offset.SetIdentity();
	}

	virtual void UpdateFPView(float frameTime){}

	virtual int  GetMaxZoomSteps() const;

	virtual void ApplyZoomMod(IFireMode* pFM);
	virtual void ResetZoomMod(IFireMode *pFM) {}

	virtual bool IsToggle();

	virtual void FilterView(SViewParams &viewparams);
	virtual void PostFilterView(SViewParams & viewparams);
	// ~IZoomMode

	virtual void ResetTurnOff();
	virtual void TurnOff(bool enable, bool smooth=true, bool anim=true);

	virtual bool IsScope() const { return false; }

	virtual void InitSharedParams ();
	virtual void CacheSharedParamsPtr();

	virtual void StartStabilize();
	virtual void EndStabilize();
	virtual bool IsStable();

protected:
	virtual void EnterZoom(float time, const char *zoom_layer=0, bool smooth=true, int zoomStep = 1);
	virtual void LeaveZoom(float time, bool smooth=true);

	virtual void ZoomIn(float time, float from, float to, bool smooth);
	virtual void ZoomOut(float time, float from, float to, bool smooth);

	virtual void OnEnterZoom();
	virtual void OnZoomedIn();

	virtual void OnLeaveZoom();
	virtual void OnZoomedOut();

	virtual void OnZoomStep(bool zoomingIn, float t);

	virtual void UpdateDepthOfField(CActor* pActor, float frameTime, float t);

	virtual void SetActorFoVScale(float fov, bool sens,bool recoil, bool hbob);
	virtual float GetActorFoVScale() const;

	virtual void SetActorSpeedScale(float scale);
	virtual float GetActorSpeedScale() const;

	virtual float GetSensitivityFromFoVScale(float scale) const;
	virtual float GetHBobFromFoVScale(float scale) const;
	virtual float GetRecoilFromFoVScale(float scale) const;
  
	virtual float GetMagFromFoVScale(float scale) const;
	virtual float GetFoVScaleFromMag(float mag) const;

	void ClearDoF();
	void ClearBlur();

	bool UseAlternativeIronSight() const;

	void AdjustScopePosition(float time, bool zoomIn);
	void AdjustNearFov(float time, bool zoomIn);
	void ResetFovAndPosition();

	void ZoomSway(float time, float &x, float&y);

	CWeapon				*m_pWeapon;

	_smart_ptr<IWeaponSharedData>		m_zoomParams;  
	uint32												m_zmIdx;
	bool												m_useCustomParams;

	//Beni - TODO: Many of the var members below could be static
  float					m_savedFoVScale;
	
	bool					m_zoomed;
	bool					m_zoomingIn;
	float					m_zoomTimer;
	float					m_zoomTime;
	float					m_focus;
	float					m_minDoF;
	float					m_maxDoF;
	float					m_averageDoF;

	float					m_startFoV;
	float					m_endFoV;
	bool					m_smooth;
	int						m_currentStep;

	float					m_initialNearFov;

	bool					m_enabled;

	float         m_swayTime;
	float         m_swayCycle;

	float         m_lastRecoil;

private:

	CIronSightSharedData*		m_pShared;
};

//Add data that you think it might be shared here.
//This data is shared between zoom modes created by
//the same weapon class
class CIronSightSharedData: public IWeaponSharedData
{
public:

	CIronSightSharedData():m_refs(0),m_valid(false){};
	virtual ~CIronSightSharedData(){};

	virtual void SetValid(bool valid) { m_valid = valid; }
	virtual bool Valid() const { return m_valid; }

	virtual const char* GetDataType() const { return "IronSightData"; };

	virtual void AddRef() const { ++m_refs; };
	virtual uint32 GetRefCount() const { return m_refs; };
	virtual void Release() const { 
		if (--m_refs <= 0)
			delete this;
	}

	virtual void GetMemoryUsage(ICrySizer* s) const
	{
		s->Add(*this);
		zoomParams.GetMemoryUsage(s);
		actions.GetMemoryUsage(s);
		zoomSway.GetMemoryUsage(s);
		spreadModParams.GetMemoryUsage(s);
		recoilModParams.GetMemoryUsage(s);
	}

	CIronSight::SZoomParams		zoomParams;
	CIronSight::SZoomActions	actions;
	CIronSight::SZoomSway			zoomSway;
	SSpreadModParams					spreadModParams;
	SRecoilModParams					recoilModParams;

protected:
	mutable uint32	m_refs;
	bool					m_valid;
};

#endif // __IRONSIGHT_H__
