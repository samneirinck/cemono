#include "StdAfx.h"
#include "G2FlowBaseNode.h"
#include "IRenderAuxGeom.h"

class CAutoFocusDofNode
	: public CFlowBaseNode
{
	typedef enum
	{
		PORT_IN_ENABLE,
		PORT_IN_DISABLE,
		PORT_IN_MAX_LENGTH,
		PORT_IN_POSITION_OFFSET_LOCAL,
		PORT_IN_SMOOTH_TIME,
		PORT_IN_FOCUS_RANGE_FACTOR,
		PORT_IN_DEBUG_ENABLED,
	} InputPorts;

	typedef enum
	{
		PORT_OUT_FOCUS_DISTANCE,
		PORT_OUT_FOCUS_RANGE,
	} OutputPorts;

public:
	CAutoFocusDofNode( SActivationInfo* pActInfo )
		: m_smoothedHitDistance( 0 )
		, m_hitDistanceChangeRate( 0 )
	{
	}

	virtual void GetConfiguration( SFlowNodeConfig& config )
	{
		static const SInputPortConfig inputs[] = 
		{
			InputPortConfig< SFlowSystemVoid >( "Enable", _HELP( "" ) ),
			InputPortConfig< SFlowSystemVoid >( "Disable", _HELP( "" ) ),
			InputPortConfig< float >( "MaxDistance", 1000, _HELP( "" ) ),
			InputPortConfig< Vec3 >( "PositionOffsetLocal", Vec3( ZERO ), _HELP( "" ) ),
			InputPortConfig< float >( "SmoothTime", 1, _HELP( "" ) ),
			InputPortConfig< float >( "FocusRangeFactor", 4, _HELP( "" ) ),
			InputPortConfig< bool >( "DebugEnabled", false, _HELP( "" ) ),
			{ 0 }
		};

		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig< float >( "FocusDistance", _HELP( "" ) ),
			OutputPortConfig< float >( "FocusRange", _HELP( "" ) ),
			{ 0 }
		};
		
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP( "" );
		config.SetCategory( EFLN_ADVANCED );
	}

	virtual void ProcessEvent( EFlowEvent event, SActivationInfo* pActInfo )
	{
		switch ( event )
		{
		case eFE_Update:
			OnUpdate( pActInfo );
			break;

		case eFE_Activate:
			OnActivate( pActInfo );
			break;
		}
	}

	void OnActivate( SActivationInfo* pActInfo )
	{
		IFlowGraph* pGraph = pActInfo->pGraph;
		TFlowNodeId nodeId = pActInfo->myID;

		if ( IsPortActive( pActInfo, PORT_IN_ENABLE ) )
		{
			pGraph->SetRegularlyUpdated( nodeId, true );
			return;
		}

		if ( IsPortActive( pActInfo, PORT_IN_DISABLE ) )
		{
			pGraph->SetRegularlyUpdated( nodeId, false );
			return;
		}
	}

	void OnUpdate( SActivationInfo* pActInfo )
	{
		const Vec3 positionOffsetLocal = GetPortVec3( pActInfo, PORT_IN_POSITION_OFFSET_LOCAL );
		const float maxDistance = max( 0.f, GetPortFloat( pActInfo, PORT_IN_MAX_LENGTH ) );

		const CCamera& camera = GetISystem()->GetViewCamera();
		
		const Vec3 cameraDirection = camera.GetViewdir();
		const Vec3 cameraPositionWorld = camera.GetPosition();
		const Matrix33 cameraOrientation = Matrix33::CreateRotationVDir( cameraDirection );
		const Vec3 positionOffsetWorld = cameraOrientation * positionOffsetLocal;

		const Vec3 rayOriginWorld = cameraPositionWorld + positionOffsetWorld;
		const Vec3 raySegment = cameraDirection * maxDistance;


		IPhysicalWorld* pWorld = gEnv->pPhysicalWorld;

		const int objectTypes = ent_all;
		const unsigned int raycastFlags = rwi_stop_at_pierceable | rwi_colltype_any;

		ray_hit hit;
		const int hitCount = pWorld->RayWorldIntersection( rayOriginWorld, raySegment, objectTypes, raycastFlags, &hit, 1 );
		
		float hitDistance = maxDistance;
		if ( 0 < hitCount )
		{
			hitDistance = hit.dist;
		}

		const float timeDelta = 0.1f;
		const float smoothTime = max( 0.f, GetPortFloat( pActInfo, PORT_IN_SMOOTH_TIME ) );
		SmoothCD( m_smoothedHitDistance, m_hitDistanceChangeRate, timeDelta, hitDistance, smoothTime );
		
		ActivateOutput( pActInfo, PORT_OUT_FOCUS_DISTANCE, m_smoothedHitDistance );

		const float focusRangeFactor = max( 0.f, GetPortFloat( pActInfo, PORT_IN_FOCUS_RANGE_FACTOR ) );
		const float focusRange = focusRangeFactor * m_smoothedHitDistance;

		ActivateOutput( pActInfo, PORT_OUT_FOCUS_RANGE, focusRange );


		const bool drawDebugInfo = GetPortBool( pActInfo, PORT_IN_DEBUG_ENABLED );
		if ( ! drawDebugInfo )
		{
			return;
		}

		IRenderer* pRenderer = gEnv->pRenderer;
		IRenderAuxGeom* pRenderAuxGeom = pRenderer->GetIRenderAuxGeom();

		ColorB rayColor = ( 0 < hitCount ) ? ColorB( 255, 255, 0 ) : ColorB( 255, 0, 0 );
		pRenderAuxGeom->DrawSphere( hit.pt, 0.1f, rayColor );
		pRenderAuxGeom->DrawLine( rayOriginWorld, rayColor, hit.pt, rayColor );

	}

	virtual void GetMemoryUsage( ICrySizer* s ) const
	{
		s->Add( *this );
	}

private:

	float m_smoothedHitDistance;
	float m_hitDistanceChangeRate;
};

REGISTER_FLOW_NODE_SINGLETON( "Camera:AutoFocusDOF", CAutoFocusDofNode );