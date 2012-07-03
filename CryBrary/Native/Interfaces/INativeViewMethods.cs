using System;

namespace CryEngine.Native
{
	[CLSCompliant(false)]
	public interface INativeViewMethods
	{
		uint GetView(uint linkedEntityId, bool forceCreate = false);
		void RemoveView(uint viewId);
		uint GetActiveView();
		void SetActiveView(uint viewId);
		ViewParams GetViewParams(uint viewId);
		void SetViewParams(uint viewId, ViewParams cam);
	}

	[CLSCompliant(false)]
	public struct ViewParams
	{
		/// <summary>
		/// view position
		/// </summary>
		public Vec3 Position;
		/// <summary>
		/// view orientation
		/// </summary>
		public Quat Rotation;
		public Quat LastLocalRotation;

		/// <summary>
		/// custom near clipping plane, 0 means use engine defaults
		/// </summary>
		public float Nearplane;
		public float FieldOfView;

		public byte ViewID;

		/// <summary>
		/// view shake status
		/// </summary>
		public bool GroundOnly;
		/// <summary>
		/// whats the amount of shake, from 0.0 to 1.0
		/// </summary>
		public float ShakingRatio;
		/// <summary>
		/// what the current angular shake
		/// </summary>
		public Quat CurrentShakeQuat;
		/// <summary>
		/// what is the current translational shake
		/// </summary>
		public Vec3 CurrentShakeShift;

		// For damping camera movement.
		/// <summary>
		/// Who we're watching. 0 == nobody.
		/// </summary>
		public uint TargetId;
		/// <summary>
		/// Where the target was.
		/// </summary>
		public Vec3 TargetPosition;
		/// <summary>
		/// current dt.
		/// </summary>
		public float FrameTime;
		/// <summary>
		/// previous rate of change of angle.
		/// </summary>
		public float AngleVelocity;
		/// <summary>
		/// previous rate of change of dist between target and camera.
		/// </summary>
		public float Velocity;
		/// <summary>
		/// previous dist of cam from target
		/// </summary>
		public float Distance;

		// blending
		public bool Blend;
		public float BlendPositionSpeed;
		public float BlendRotationSpeed;
		public float BlendFieldOfViewSpeed;
		public Vec3 BlendPositionOffset;
		public Quat BlendRotationOffset;
		public float BlendFieldOfViewOffset;
		public bool JustActivated;

		private byte LastViewID;
		private Vec3 LastPosition;
		private Quat LastRotation;
		private float LastFieldOfView;
	}
}