#include "StdAfx.h"
#include <IActorSystem.h>
#include "Nodes/G2FlowBaseNode.h"
#include "IHardwareMouse.h"
#include "Cry_Camera.h"
#include "IVehicleSystem.h"
#include "Game.h"

class CFlowMouseInfoNode : public CFlowBaseNode, public IInputEventListener, public IHardwareMouseEventListener, public IGameFrameworkListener
{
private:

	SActivationInfo m_actInfo;
	bool m_bEnabled;
	bool m_bKeyPressed;
	bool m_bOnEnter;
	bool m_bOnLeave;
	bool m_bNoInput;
	bool m_bSelect;
	bool m_bAdd;
	Vec2i m_vSelectPosStart;
	Vec2i m_vSelectPosEnd;
	unsigned int m_iCursorCounter;
	EntityId m_EntityID;
	EntityId m_LastEntityID;
	string m_sKey;
	string m_sRaySelection;
	int m_iMouseX;
	int m_iMouseY;
	bool m_bFinished;
	std::vector<EntityId>::iterator m_CurrentID;
	std::vector<EntityId> m_Entities;
	IRenderer* m_pRenderer;

public:
	CFlowMouseInfoNode( SActivationInfo * pActInfo )
	{
		m_bEnabled = true;
		m_bKeyPressed = false;
		m_bOnEnter = false;
		m_bOnLeave = false;
		m_bNoInput = false;
		m_bSelect = false;
		m_bFinished = false;
		m_bAdd = false;
		m_iCursorCounter = 0;
		m_EntityID = 0;
		m_LastEntityID = 0;
		m_iMouseX = 0;
		m_iMouseY = 0;
		m_sKey = "NUM0";
		m_sRaySelection = "All";
		m_pRenderer = gEnv->pRenderer;
	}

	~CFlowMouseInfoNode()
	{
		if(gEnv->pHardwareMouse)
			gEnv->pHardwareMouse->RemoveListener(this);

		if(GetISystem() && GetISystem()->GetIInput())
			GetISystem()->GetIInput()->RemoveEventListener(this);
	}

	IFlowNodePtr Clone( SActivationInfo * pActInfo )
	{
		return new CFlowMouseInfoNode(pActInfo);
	}

	void Serialize(SActivationInfo* pActInfo, TSerialize ser)
	{
		if (ser.IsReading())
		{
			int counter = m_iCursorCounter;

			while(counter > 0)
			{
				gEnv->pHardwareMouse->DecrementCounter();
				--counter;
			}
		}

		ser.Value("m_bEnabled", m_bEnabled);
		ser.Value("m_iCursorCounter", m_iCursorCounter);
		ser.Value("m_bKeyPressed", m_bKeyPressed);
		ser.Value("m_bOnEnter", m_bOnEnter);
		ser.Value("m_bOnLeave", m_bOnLeave);	
		ser.Value("m_EntityID", m_EntityID);
		ser.Value("m_LastEntityID", m_LastEntityID);
		ser.Value("m_sKey", m_sKey);
		ser.Value("m_sRaySelection", m_sRaySelection);

		if (ser.IsReading())
		{
			m_actInfo = *pActInfo;

			if(gEnv->pHardwareMouse)
				gEnv->pHardwareMouse->AddListener(this);

			if(GetISystem() && GetISystem()->GetIInput())
				GetISystem()->GetIInput()->AddEventListener(this);

			if(GetPortBool(&m_actInfo, EIP_EnableModalMode))
			{
				m_bKeyPressed = true;
				m_bNoInput = true;

				if(GetPortBool(&m_actInfo, EIP_DisableMovement))
				{
					Movement(false);
				}

				MouseCursor(true);
			}
			else
			{
				Movement(true);
			}

			if (m_bEnabled && m_bKeyPressed)
			{
				int counter = m_iCursorCounter;

				while (counter > 0)
				{
					gEnv->pHardwareMouse->IncrementCounter();
					--counter;
				}

				Movement(false);
			}
		}
	}

	enum EInputPorts
	{
		EIP_Enable,
		EIP_Disable,
		EIP_IgnoreLinkedVehicle,
		EIP_EveryHit,
		EIP_DisableMovement,
		EIP_EnableModalMode,
		EIP_DisableModalMode,
		EIP_ShowBoundaries,
		EIP_UpdateAlways,
		EIP_RaySelection,
		EIP_Key
	};

	enum EOutputPorts
	{
		EOP_EntityID = 0,
		EOP_LastEntityID,
		EOP_OnEnter,
		EOP_OnLeave,
		EOP_OnMousePressed,
		EOP_OnMouseReleased,
		EOP_MouseX,
		EOP_MouseY,
		EOP_MouseHitPos,
		EOP_MouseHitDir,
		EOP_SelectedEntity
	};

	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		static const SInputPortConfig inputs[] = {
			InputPortConfig<bool>("Enable", false, _HELP("Enable ME")),
			InputPortConfig<bool>("Disable", false, _HELP("Disable ME")),
			InputPortConfig<bool>("IgnoreLinked", true, _HELP("If enabled it will add the linked vehicle to the raycast ignorelist")),
			InputPortConfig<bool>("EveryHit", false, _HELP("If enabled it will return the world position coordinates of every rayhit, not just if an entity exists under the cursor")),
			InputPortConfig<bool>("DisableMovement", true, _HELP("If enabled it will disable player movement while modal mode is enabled")),
			InputPortConfig<bool>("EnableModalMode", false, _HELP("If enabled it will enable modal mode without pressing a key")),
			InputPortConfig<bool>("DisableModalMode", false, _HELP("If enabled it will disable modal mode without pressing a key")),
			InputPortConfig<bool>("ShowBoundaries", true, _HELP("If enabled it will show the selection boundaries")),
			InputPortConfig<bool>("UpdateAlways", false, _HELP("If enabled it will always update the flownode")),
			InputPortConfig<string>("RaySelection", "All", _HELP("Sets what should cause a ray hit"), "Ray Selection", _UICONFIG("enum_string: All=All, Terrain=Terrain, Rigid=Rigid, Static=Static, Water=Water, Living=Living")),
			InputPortConfig<string>("Key", "NUM0",  _HELP("Sets the modal mode activation key"), "Modal mode key", _UICONFIG("enum_string: A=A,B=B,C=C,D=D,E=E,F=F,G=G,H=H,I=I,J=J,K=K,L=L,M=M,N=N,O=O,P=P,Q=Q,R=R,S=S,T=T,U=U,V=V,X=X,Y=Y,Z=Z,LEFT=LEFT,RIGHT=RIGHT,UP=UP,DOWN=DOWN,0=0,1=1,2=2,3=3,4=4,5=5,6=6,7=7,8=8,9=9,MOUSE1=MOUSE1, MOUSE2=MOUSE2, MOUSE3=MOUSE3, MOUSE4=MOUSE4, MOUSE5=MOUSE5, MOUSE6=MOUSE6, MOUSE7=MOUSE7, MOUSE8=MOUSE8,NUM0=NUM0,NUM1=NUM1,NUM2=NUM2,NUM3=NUM3,NUM4=NUM4,NUM5=NUM5,NUM6=NUM6,NUM7=NUM7,NUM8=NUM8,NUM9=NUM9,NUMENTER=NUMENTER,F1=F1,F2=F2,F3=F3,F4=F4,F5=F5,F6=F6,F7=F7,F8=F8,F9=F9,F10=F10")),
			{0}
		};

		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<EntityId>("EntityID", _HELP("EntityID of the current entity under the mouse cursor")),
			OutputPortConfig<EntityId>("LastEntityID", _HELP("EntityID of the last entity under the mouse cursor")),
			OutputPortConfig<bool>("OnEnter", _HELP("Triggers if the mouse cursor enters an entity shape")),
			OutputPortConfig<bool>("OnLeave", _HELP("Triggers if the mouse cursor leaves an entity shape")),
			OutputPortConfig<int>("OnPressed", _HELP("Returns an integer which represents the pressed mouse key")),
			OutputPortConfig<int>("OnReleased", _HELP("Returns an integer which represents the released mouse key")),
			OutputPortConfig<int>("X", _HELP("Mouse cursor x position")),
			OutputPortConfig<int>("Y", _HELP("Mouse cursor y position")),
			OutputPortConfig<Vec3>("HitPos", _HELP("Hit position of the mouse cursor in world coordinates")),
			OutputPortConfig<Vec3>("HitDir", _HELP("Direction from the camera position to the mouse cursor position")),
			OutputPortConfig<EntityId>("SelectedEntityIDs", _HELP("EntityID's of the selected entities")),
			{0}
		};

		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("Returns several mouse information.");
		config.SetCategory(EFLN_APPROVED);
	}

	virtual void ProcessEvent( EFlowEvent event, SActivationInfo *pActInfo )
	{
		switch (event)
		{
		case eFE_Initialize:
			{
				m_actInfo = *pActInfo;

				if(gEnv->pHardwareMouse)
					gEnv->pHardwareMouse->AddListener(this);

				if(GetISystem() && GetISystem()->GetIInput())
					GetISystem()->GetIInput()->AddEventListener(this);

				m_bEnabled = false;
				m_bNoInput = false;
				m_bKeyPressed = false;

				MouseCursor(false);

				m_sKey = GetPortString(pActInfo, EIP_Key);

				if(GetPortBool(&m_actInfo, EIP_EnableModalMode))
				{
					m_bKeyPressed = true;
					m_bNoInput = true;

					if(GetPortBool(&m_actInfo, EIP_DisableMovement))
					{
						Movement(false);
					}

					MouseCursor(true);
				}
				else
				{
					Movement(true);
				}
				break;
			}		
		case eFE_Activate:
			{
				if (IsPortActive(pActInfo, EIP_Enable))
				{
					if (IsPortActive(pActInfo, EIP_Enable) && !m_bEnabled)
					{
						m_bEnabled = true;

						if(gEnv->pHardwareMouse)
							gEnv->pHardwareMouse->AddListener(this);

						if(GetISystem() && GetISystem()->GetIInput())
							GetISystem()->GetIInput()->AddEventListener(this);

						m_sKey = GetPortString(pActInfo, EIP_Key);
					}
					if (IsPortActive(pActInfo, EIP_Disable) && m_bEnabled)
					{
						m_bEnabled = false;

						if(GetISystem() && GetISystem()->GetIInput())
							GetISystem()->GetIInput()->RemoveEventListener(this);

						if(gEnv->pHardwareMouse)
							gEnv->pHardwareMouse->RemoveListener(this);
					}	
				}

				if (IsPortActive(pActInfo, EIP_EnableModalMode) && m_bEnabled)
				{
					m_bKeyPressed = true;
					m_bNoInput = true;

					if(GetPortBool(&m_actInfo, EIP_DisableMovement))
					{
						Movement(false);
					}

					MouseCursor(true);
				}
				else if (IsPortActive(pActInfo, EIP_DisableModalMode) && m_bEnabled)
				{
					m_bKeyPressed = false;
					m_bNoInput = false;

					MouseCursor(false);

					if(GetPortBool(&m_actInfo, EIP_DisableMovement))
					{
						Movement(true);
					}
				}

				if (IsPortActive(pActInfo, EIP_RaySelection) && m_bEnabled)
				{
					m_sRaySelection = GetPortString(pActInfo, EIP_RaySelection);
				}

				if (GetPortBool(pActInfo, EIP_UpdateAlways))
				{
					if (g_pGame && g_pGame->GetIGameFramework())
						g_pGame->GetIGameFramework()->RegisterListener(this, "FlowNode_MouseInfo", FRAMEWORKLISTENERPRIORITY_DEFAULT);
				} 
				else
				{
					if (g_pGame && g_pGame->GetIGameFramework())
						g_pGame->GetIGameFramework()->UnregisterListener(this);
				}

				break;
			}
		case eFE_Update:
			{
				//render selection boundaries if needed
				if(m_bSelect && GetPortBool(pActInfo, EIP_ShowBoundaries))
				{
					if(IRenderAuxGeom* pGeom = m_pRenderer->GetIRenderAuxGeom())
					{
						//calculate the four selection boundary points
						Vec3 vTopLeft((float)m_vSelectPosStart.x, (float)m_vSelectPosStart.y, 0.0f);
						Vec3 vTopRight((float)m_iMouseX, (float)m_vSelectPosStart.y,0.0f);
						Vec3 vBottomLeft((float)m_vSelectPosStart.x, (float)m_iMouseY,0.0f);
						Vec3 vBottomRight((float)m_iMouseX, (float)m_iMouseY,0.0f);

						m_pRenderer->Set2DMode(true, m_pRenderer->GetWidth(), m_pRenderer->GetHeight());

						//set boundary color: white
						ColorB col(255,255,255,255); 

						pGeom->DrawLine(vTopLeft,col,vTopRight,col);
						pGeom->DrawLine(vTopRight,col,vBottomRight,col);
						pGeom->DrawLine(vTopLeft,col,vBottomLeft,col);
						pGeom->DrawLine(vBottomLeft,col,vBottomRight,col);

						m_pRenderer->Set2DMode(false,0,0);
					}
				}

				if(m_bFinished && !m_bSelect && !GetPortBool(pActInfo, EIP_UpdateAlways))
				{
					//if we have sent every stored entity id to the output port, disable the updates for each frame
					m_actInfo.pGraph->SetRegularlyUpdated(m_actInfo.myID,false);
				}
				else if(!m_bFinished && !m_bSelect)
				{
					if(m_CurrentID == m_Entities.end())
					{
						//we reached the end of the stored entity id's vector
						m_bFinished = true;
					}
					else
					{
						//send the current entity id to the output port
						ActivateOutput(&m_actInfo,EOP_SelectedEntity, *m_CurrentID);
						//increase to next id, so on the next update we will send the new entity id
						++m_CurrentID;
					}	
				}
				break;
			}
		}
	}

	virtual void GetMemoryUsage(ICrySizer * s) const
	{
		s->Add(*this);
	}

	// IInputEventListener
	virtual bool OnInputEventUI( const SInputEvent &event ) {return false;}
	virtual bool OnInputEvent( const SInputEvent &rInputEvent)
	{
		if(!m_bEnabled)
			return false;

		if(rInputEvent.keyId == GetKeyID(m_sKey) && rInputEvent.state == eIS_Pressed && !m_bNoInput)
		{
			m_bKeyPressed = !m_bKeyPressed;

			if(m_bKeyPressed)
			{
				if(GetPortBool(&m_actInfo, EIP_DisableMovement))
				{
					Movement(false);
				}

				MouseCursor(true);
			}
			else
			{
				MouseCursor(false);

				if(GetPortBool(&m_actInfo, EIP_DisableMovement))
				{
					Movement(true);
				}
			}
		}
		else if(rInputEvent.keyId == eKI_Mouse1 && rInputEvent.state == eIS_Pressed && m_bKeyPressed)
		{
			ActivateOutput(&m_actInfo, EOP_OnMousePressed, 1);
		}
		else if(rInputEvent.keyId == eKI_Mouse1 && rInputEvent.state == eIS_Released && m_bKeyPressed)
		{
			ActivateOutput(&m_actInfo, EOP_OnMouseReleased, 1);
		}
		else if(rInputEvent.keyId == eKI_Mouse2 && rInputEvent.state == eIS_Pressed && m_bKeyPressed)
		{
			ActivateOutput(&m_actInfo, EOP_OnMousePressed, 2);
		}
		else if(rInputEvent.keyId == eKI_Mouse2 && rInputEvent.state == eIS_Released && m_bKeyPressed)
		{
			ActivateOutput(&m_actInfo, EOP_OnMouseReleased, 2);
		}
		else if(rInputEvent.keyId == eKI_Mouse3 && rInputEvent.state == eIS_Pressed && m_bKeyPressed)
		{
			ActivateOutput(&m_actInfo, EOP_OnMousePressed, 3);
		}
		else if(rInputEvent.keyId == eKI_Mouse3 && rInputEvent.state == eIS_Released && m_bKeyPressed)
		{
			ActivateOutput(&m_actInfo, EOP_OnMouseReleased, 3);
		}
		else if(rInputEvent.keyId == eKI_Mouse4 && rInputEvent.state == eIS_Pressed && m_bKeyPressed)
		{
			ActivateOutput(&m_actInfo, EOP_OnMousePressed, 4);
		}
		else if(rInputEvent.keyId == eKI_Mouse4 && rInputEvent.state == eIS_Released && m_bKeyPressed)
		{
			ActivateOutput(&m_actInfo, EOP_OnMouseReleased, 4);
		}
		else if(rInputEvent.keyId == eKI_Mouse5 && rInputEvent.state == eIS_Pressed && m_bKeyPressed)
		{
			ActivateOutput(&m_actInfo, EOP_OnMousePressed, 5);
		}
		else if(rInputEvent.keyId == eKI_Mouse5 && rInputEvent.state == eIS_Released && m_bKeyPressed)
		{
			ActivateOutput(&m_actInfo, EOP_OnMouseReleased, 5);
		}
		else if(rInputEvent.keyId == eKI_Mouse6 && rInputEvent.state == eIS_Pressed && m_bKeyPressed)
		{
			ActivateOutput(&m_actInfo, EOP_OnMousePressed, 6);
		}
		else if(rInputEvent.keyId == eKI_Mouse6 && rInputEvent.state == eIS_Released && m_bKeyPressed)
		{
			ActivateOutput(&m_actInfo, EOP_OnMouseReleased, 6);
		}
		else if(rInputEvent.keyId == eKI_Mouse7 && rInputEvent.state == eIS_Pressed && m_bKeyPressed)
		{
			ActivateOutput(&m_actInfo, EOP_OnMousePressed, 7);
		}
		else if(rInputEvent.keyId == eKI_Mouse7 && rInputEvent.state == eIS_Released && m_bKeyPressed)
		{
			ActivateOutput(&m_actInfo, EOP_OnMouseReleased, 7);
		}
		else if(rInputEvent.keyId == eKI_Mouse8 && rInputEvent.state == eIS_Pressed && m_bKeyPressed)
		{
			ActivateOutput(&m_actInfo, EOP_OnMousePressed, 8);
		}
		else if(rInputEvent.keyId == eKI_Mouse8 && rInputEvent.state == eIS_Released && m_bKeyPressed)
		{
			ActivateOutput(&m_actInfo, EOP_OnMouseReleased, 8);
		}

		if((rInputEvent.keyId == eKI_LCtrl || rInputEvent.keyId == eKI_RCtrl) && rInputEvent.state == eIS_Pressed && m_bKeyPressed)
		{
			m_bAdd = true;
		}
		else if((rInputEvent.keyId == eKI_LCtrl || rInputEvent.keyId == eKI_RCtrl) && rInputEvent.state == eIS_Released && m_bKeyPressed)
		{
			m_bAdd = false;
		}

		return false;	
	}
	// ~IInputEventListener

	// IHardwareMouseEventListener
	virtual void OnHardwareMouseEvent(int iX,int iY,EHARDWAREMOUSEEVENT eHardwareMouseEvent, int wheelDelta = 0)
	{
		ActivateOutput(&m_actInfo, EOP_MouseX, iX);
		ActivateOutput(&m_actInfo, EOP_MouseY, iY);

		m_iMouseX = iX;
		m_iMouseY = iY;

		if(m_bKeyPressed && m_bEnabled)
		{
			if(eHardwareMouseEvent == HARDWAREMOUSEEVENT_LBUTTONDOWN)
			{
				m_bSelect = true;
				m_vSelectPosStart = Vec2i(iX, iY);
				m_actInfo.pGraph->SetRegularlyUpdated(m_actInfo.myID,true);
			}
			else if(eHardwareMouseEvent == HARDWAREMOUSEEVENT_LBUTTONUP)
			{
				m_bSelect = false;
				m_vSelectPosEnd = Vec2i(iX, iY);
				GetSelectedEntities();
				m_CurrentID = m_Entities.begin();
			}

			if(!GetPortBool(&m_actInfo, EIP_UpdateAlways))
				CheckEntitySelection();
		}
		else
		{
			if(m_bOnEnter)
			{
				m_LastEntityID = m_EntityID;

				ActivateOutput(&m_actInfo, EOP_EntityID, 0);
				ActivateOutput(&m_actInfo, EOP_LastEntityID, m_LastEntityID);
				ActivateOutput(&m_actInfo, EOP_OnLeave, true);

				m_bOnEnter = false;
				m_bOnLeave = true;
			}
		}

	}
	// ~IHardwareMouseEventListener

private:

	void CFlowMouseInfoNode::CheckEntitySelection()
	{
		EntityId id = GetMouseEntityID(m_iMouseX, m_iMouseY);

		if(id != 0 && !m_bOnEnter)
		{
			m_EntityID = id;

			ActivateOutput(&m_actInfo, EOP_EntityID, m_EntityID);
			ActivateOutput(&m_actInfo, EOP_OnEnter, true);

			m_bOnEnter = true;
			m_bOnLeave = false;	
		}
		else if(id != 0 && id != m_EntityID)
		{
			ActivateOutput(&m_actInfo, EOP_LastEntityID, m_EntityID);
			ActivateOutput(&m_actInfo, EOP_EntityID, id);
			ActivateOutput(&m_actInfo, EOP_OnLeave, true);

			m_EntityID = id;
			m_LastEntityID = m_EntityID;

			m_bOnEnter = false;
			m_bOnLeave = true;
		}
		else if(id == 0 && !m_bOnLeave && m_bOnEnter)
		{
			m_LastEntityID = m_EntityID;

			ActivateOutput(&m_actInfo, EOP_EntityID, 0);
			ActivateOutput(&m_actInfo, EOP_LastEntityID, m_LastEntityID);
			ActivateOutput(&m_actInfo, EOP_OnLeave, true);

			m_bOnEnter = false;
			m_bOnLeave = true;		
		}
	}

	void CFlowMouseInfoNode::Movement(bool enable)
	{	
		IActionMapManager *pActionMapManager = gEnv->pGame->GetIGameFramework()->GetIActionMapManager();

		if(!pActionMapManager)
			return;

		if(enable)
		{				
			pActionMapManager->EnableFilter("no_move", false);
			pActionMapManager->EnableFilter("no_mouse", false);
		}
		else
		{
			pActionMapManager->EnableFilter("no_move", true);
			pActionMapManager->EnableFilter("no_mouse", true);
		}	
	}

	void CFlowMouseInfoNode::MouseCursor(bool enable)
	{
		if(enable)
		{
			gEnv->pHardwareMouse->IncrementCounter();
			++m_iCursorCounter;
		}
		else
		{
			while(m_iCursorCounter>0)
			{
				gEnv->pHardwareMouse->DecrementCounter();
				m_iCursorCounter--;
			}
		}
	}

	void CFlowMouseInfoNode::GetSelectedEntities()
	{
		//clear previously stored entity id's if left or right CTRL is not pressed
		if(!m_bAdd)
			m_Entities.clear();

		m_bFinished = false;

		IEntityItPtr pIt = gEnv->pEntitySystem->GetEntityIterator();

		while (!pIt->IsEnd())
		{
			if (IEntity *pEntity = pIt->Next())
			{
				//skip useless entities (gamerules, fists etc.)
				if(IPhysicalEntity *physEnt = pEntity->GetPhysics())
				{
					IActor *pClientActor = g_pGame->GetIGameFramework()->GetClientActor();

					if (!pClientActor)
						return;

					//skip the client actor entity
					if(physEnt == pClientActor->GetEntity()->GetPhysics())
						continue;

					//skip linked vehicle entity if we want to ignore it
					if(GetPortBool(&m_actInfo, EIP_IgnoreLinkedVehicle) && pClientActor->GetLinkedVehicle() && physEnt == pClientActor->GetLinkedVehicle()->GetEntity()->GetPhysics())
						continue;

					AABB worldBounds;
					pEntity->GetWorldBounds(worldBounds);

					//skip further calculations if the entity is not visible at all...
					if(gEnv->pSystem->GetViewCamera().IsAABBVisible_F(worldBounds) == CULL_EXCLUSION)
						continue;

					Vec3 wpos = pEntity->GetWorldPos();
					Quat rot = pEntity->GetWorldRotation();
					AABB localBounds;

					pEntity->GetLocalBounds(localBounds);

					//get min and max values of the entity bounding box (local positions)
					static Vec3 points[2];
					points[0] = wpos + rot * localBounds.min;
					points[1] = wpos + rot * localBounds.max;

					static Vec3 pointsProjected[2];

					//project the bounding box min max values to screen positions
					for (int i=0; i<2; ++i)
					{
						m_pRenderer->ProjectToScreen(points[i].x, points[i].y, points[i].z, &pointsProjected[i].x, &pointsProjected[i].y, &pointsProjected[i].z); 
						const float fWidth = (float)m_pRenderer->GetWidth();
						const float fHeight = (float)m_pRenderer->GetHeight();

						//scale projected values to the actual screen resolution
						pointsProjected[i].x *= 0.01f * fWidth;
						pointsProjected[i].y *= 0.01f * fHeight;
					}	

					//check if the projected bounding box min max values are fully or partly inside the screen selection 
					if((m_vSelectPosStart.x <= pointsProjected[0].x && pointsProjected[0].x <= m_vSelectPosEnd.x) || 
						(m_vSelectPosStart.x >= pointsProjected[0].x && m_vSelectPosEnd.x <= pointsProjected[1].x) || 
						(m_vSelectPosStart.x <= pointsProjected[1].x && m_vSelectPosEnd.x >= pointsProjected[1].x) ||
						(m_vSelectPosStart.x <= pointsProjected[0].x && m_vSelectPosEnd.x >= pointsProjected[1].x))
					{
						if((m_vSelectPosStart.y <= pointsProjected[0].y && m_vSelectPosEnd.y >= pointsProjected[0].y) ||
							(m_vSelectPosStart.y <= pointsProjected[1].y && m_vSelectPosEnd.y >= pointsProjected[0].y) ||
							(m_vSelectPosStart.y <= pointsProjected[1].y && m_vSelectPosEnd.y >= pointsProjected[1].y))
						{
							//finally we have an entity id
							//if left or right CTRL is not pressed we can directly add every entity id, old entity id's are already deleted
							if(!m_bAdd)
							{
								m_Entities.push_back(pEntity->GetId());
							}
							else
							{
								//check for previously added entity id's first
								stl::push_back_unique(m_Entities, pEntity->GetId());
							}
						}
					}
				}

			}
		}
	}

	EntityId CFlowMouseInfoNode::GetMouseEntityID(int xMouse, int yMouse)
	{		
		if(!gEnv->pHardwareMouse || !m_pRenderer || !gEnv->p3DEngine || !gEnv->pSystem || !gEnv->pEntitySystem || !g_pGame->GetIGameFramework())
			return 0;

		IActor *pClientActor = g_pGame->GetIGameFramework()->GetClientActor();

		if (!pClientActor)
			return 0;

		yMouse = m_pRenderer->GetHeight() - yMouse;

		float xWorldMouse, yWorldMouse, zWorldMouse = 0.0f;
		m_pRenderer->UnProjectFromScreen((float)xMouse, (float)yMouse, 0.0f, &xWorldMouse, &yWorldMouse, &zWorldMouse);

		Vec3  vCamPos = gEnv->pSystem->GetViewCamera().GetPosition();
		Vec3     vDir = (Vec3(xWorldMouse, yWorldMouse, zWorldMouse) - vCamPos).GetNormalizedSafe();

		ActivateOutput(&m_actInfo, EOP_MouseHitDir, vDir);

		IPhysicalEntity *pPhysicalEnt = pClientActor->GetEntity() ? pClientActor->GetEntity()->GetPhysics() : NULL;

		if(!pPhysicalEnt)
			return 0;

		static IPhysicalEntity* pSkipEnts[2];
		pSkipEnts[0] = pPhysicalEnt;
		int numSkipped = 1;

		if(GetPortBool(&m_actInfo, EIP_IgnoreLinkedVehicle) && pClientActor->GetLinkedVehicle())
		{
			if(IPhysicalEntity *pPhysicalEntVehicle = pClientActor->GetLinkedVehicle()->GetEntity() ? pClientActor->GetLinkedVehicle()->GetEntity()->GetPhysics() : NULL)
				pSkipEnts[1] = pPhysicalEntVehicle;

			numSkipped = 2;
		}

		entity_query_flags queryFlags;

		if(GetPortBool(&m_actInfo,EIP_EveryHit))
		{
			string sRaySelection = GetPortString(&m_actInfo, EIP_RaySelection);

			if(!strcmpi("Terrain",sRaySelection))
				queryFlags = ent_terrain;
			else if(!strcmpi("Rigid",sRaySelection))
				queryFlags = ent_rigid;
			else if(!strcmpi("All",sRaySelection))
				queryFlags = ent_all;
			else if(!strcmpi("Static",sRaySelection))
				queryFlags = ent_static;
			else if(!strcmpi("Living",sRaySelection))
				queryFlags = ent_living;
			else if(!strcmpi("Water",sRaySelection))
				queryFlags = ent_water;
			else
				queryFlags = ent_all;
		}
		else
			queryFlags = ent_all;

		static ray_hit hit;
		static const unsigned int flags = rwi_stop_at_pierceable|rwi_colltype_any;
		float  fRange = gEnv->p3DEngine->GetMaxViewDistance();

		if (gEnv->pPhysicalWorld && gEnv->pPhysicalWorld->RayWorldIntersection(vCamPos, vDir * fRange, queryFlags, flags, &hit, 1, pSkipEnts, numSkipped))
		{
			if(GetPortBool(&m_actInfo, EIP_EveryHit))
				ActivateOutput(&m_actInfo, EOP_MouseHitPos, hit.pt);

			if (IEntity *pEntity = gEnv->pEntitySystem->GetEntityFromPhysics(hit.pCollider))
			{
				if(!GetPortBool(&m_actInfo, EIP_EveryHit))
					ActivateOutput(&m_actInfo, EOP_MouseHitPos, hit.pt);

				return pEntity->GetId();
			}
		}	

		return 0;
	}

	EKeyId CFlowMouseInfoNode::GetKeyID(string sKey)
	{
		if(!strcmpi(sKey, "0"))
			return eKI_0;
		else if(!strcmpi(sKey, "1"))
			return eKI_1;
		else if(!strcmpi(sKey, "2"))
			return eKI_2;
		else if(!strcmpi(sKey, "3"))
			return eKI_3;
		else if(!strcmpi(sKey, "4"))
			return eKI_4;
		else if(!strcmpi(sKey, "5"))
			return eKI_5;
		else if(!strcmpi(sKey, "6"))
			return eKI_6;
		else if(!strcmpi(sKey, "7"))
			return eKI_7;
		else if(!strcmpi(sKey, "8"))
			return eKI_8;
		else if(!strcmpi(sKey, "9"))
			return eKI_9;

		else if(!strcmpi(sKey, "F1"))
			return eKI_F1;
		else if(!strcmpi(sKey, "F2"))
			return eKI_F2;
		else if(!strcmpi(sKey, "F3"))
			return eKI_F3;
		else if(!strcmpi(sKey, "F4"))
			return eKI_F4;
		else if(!strcmpi(sKey, "F5"))
			return eKI_F5;
		else if(!strcmpi(sKey, "F6"))
			return eKI_F6;
		else if(!strcmpi(sKey, "F7"))
			return eKI_F7;
		else if(!strcmpi(sKey, "F8"))
			return eKI_F8;
		else if(!strcmpi(sKey, "F9"))
			return eKI_F9;
		else if(!strcmpi(sKey, "F10"))
			return eKI_F10;

		else if(!strcmpi(sKey, "NUM0"))
			return eKI_NP_0;
		else if(!strcmpi(sKey, "NUM1"))
			return eKI_NP_1;
		else if(!strcmpi(sKey, "NUM2"))
			return eKI_NP_2;
		else if(!strcmpi(sKey, "NUM3"))
			return eKI_NP_3;
		else if(!strcmpi(sKey, "NUM4"))
			return eKI_NP_4;
		else if(!strcmpi(sKey, "NUM5"))
			return eKI_NP_5;
		else if(!strcmpi(sKey, "NUM6"))
			return eKI_NP_6;
		else if(!strcmpi(sKey, "NUM7"))
			return eKI_NP_7;
		else if(!strcmpi(sKey, "NUM8"))
			return eKI_NP_8;
		else if(!strcmpi(sKey, "NUM9"))
			return eKI_NP_9;
		else if(!strcmpi(sKey, "NUMENTER"))
			return eKI_NP_Enter;

		else if(!strcmpi(sKey, "MOUSE1"))
			return eKI_Mouse1;
		else if(!strcmpi(sKey, "MOUSE2"))
			return eKI_Mouse2;
		else if(!strcmpi(sKey, "MOUSE3"))
			return eKI_Mouse3;
		else if(!strcmpi(sKey, "MOUSE4"))
			return eKI_Mouse4;
		else if(!strcmpi(sKey, "MOUSE5"))
			return eKI_Mouse5;
		else if(!strcmpi(sKey, "MOUSE6"))
			return eKI_Mouse6;
		else if(!strcmpi(sKey, "MOUSE7"))
			return eKI_Mouse7;
		else if(!strcmpi(sKey, "MOUSE8"))
			return eKI_Mouse8;

		else if(!strcmpi(sKey, "DOWN"))
			return eKI_Down;
		else if(!strcmpi(sKey, "UP"))
			return eKI_Up;
		else if(!strcmpi(sKey, "LEFT"))
			return eKI_Left;
		else if(!strcmpi(sKey, "RIGHT"))
			return eKI_Right;

		else if(!strcmpi(sKey, "A"))
			return eKI_A;
		else if(!strcmpi(sKey, "B"))
			return eKI_B;
		else if(!strcmpi(sKey, "C"))
			return eKI_C;
		else if(!strcmpi(sKey, "D"))
			return eKI_D;
		else if(!strcmpi(sKey, "E"))
			return eKI_E;
		else if(!strcmpi(sKey, "F"))
			return eKI_F;
		else if(!strcmpi(sKey, "G"))
			return eKI_G;
		else if(!strcmpi(sKey, "H"))
			return eKI_H;
		else if(!strcmpi(sKey, "I"))
			return eKI_I;
		else if(!strcmpi(sKey, "J"))
			return eKI_J;
		else if(!strcmpi(sKey, "K"))
			return eKI_K;
		else if(!strcmpi(sKey, "L"))
			return eKI_L;
		else if(!strcmpi(sKey, "M"))
			return eKI_M;
		else if(!strcmpi(sKey, "N"))
			return eKI_N;
		else if(!strcmpi(sKey, "O"))
			return eKI_O;
		else if(!strcmpi(sKey, "P"))
			return eKI_P;
		else if(!strcmpi(sKey, "Q"))
			return eKI_Q;
		else if(!strcmpi(sKey, "R"))
			return eKI_R;
		else if(!strcmpi(sKey, "S"))
			return eKI_S;
		else if(!strcmpi(sKey, "T"))
			return eKI_T;
		else if(!strcmpi(sKey, "U"))
			return eKI_U;
		else if(!strcmpi(sKey, "V"))
			return eKI_V;
		else if(!strcmpi(sKey, "W"))
			return eKI_W;
		else if(!strcmpi(sKey, "X"))
			return eKI_X;
		else if(!strcmpi(sKey, "Y"))
			return eKI_Y;
		else if(!strcmpi(sKey, "Z"))
			return eKI_Z;

		else
			return eKI_NP_0;
	}

	virtual void OnSaveGame(ISaveGame* pSaveGame) {}
	virtual void OnLoadGame(ILoadGame* pLoadGame) {}
	virtual void OnLevelEnd(const char* nextLevel) {}
	virtual void OnActionEvent(const SActionEvent& event) {}
	virtual void OnPostUpdate(float fDeltaTime)
	{
		if(!m_bEnabled)
			return;

		if(GetPortBool(&m_actInfo, EIP_UpdateAlways))
			CheckEntitySelection();	
	}
};

REGISTER_FLOW_NODE("Input:Mouse", CFlowMouseInfoNode);