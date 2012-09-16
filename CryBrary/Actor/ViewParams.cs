using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine
{
	public struct ViewParams
	{
		public Vec3 Position;//view position
		public Quat Rotation;//view orientation
		public Quat LocalRotationLast;

		public float NearPlane;//custom near clipping plane, 0 means use engine defaults
		public float FieldOfView;

		public ushort ViewID;

		//view shake status
		public bool GroundOnly;
		public float ShakingRatio;//whats the amount of shake, from 0.0 to 1.0
		public Quat CurrentShakeQuat;//what the current angular shake
		public Vec3 CurrentShakeShift;//what is the current translational shake

		// For damping camera movement.
		public EntityId TargetId;  // Who we're watching. 0 == nobody.
		public Vec3 TargetPos;     // Where the target was.
		public float FrameTime;    // current dt.
		public float AngleVel;     // previous rate of change of angle.
		public float Vel;          // previous rate of change of dist between target and camera.
		public float Dist;         // previous dist of cam from target

		//blending
		public bool Blend;
		public float BlendPosSpeed;
		public float BlendRotSpeed;
		public float BlendFOVSpeed;
		public Vec3 BlendPosOffset;
		public Quat BlendRotOffset;
		public float BlendFOVOffset;
		public bool JustActivated;

		public ushort ViewIDLast;
		public Vec3 PositionLast;//last view position
		public Quat RotationLast;//last view orientation
		public float FOVLast;
	}
}
