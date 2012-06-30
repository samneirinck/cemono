using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using CryEngine.Native;

namespace CryEngine
{
	public class View
	{
        #region Statics
		/// <summary>
		/// Gets a view linked to a specific entity id.
		/// </summary>
		public static View Get(EntityBase linkedEntity, bool forceCreate = false)
		{
			return Get(linkedEntity.Id, forceCreate);
		}

		/// <summary>
		/// Gets a view linked to a specific entity id.
		/// </summary>
		public static View Get(EntityId linkedEntity, bool forceCreate = false)
		{
			var viewId = NativeMethods.View.GetView(linkedEntity, forceCreate);
			if(viewId == 0)
				return null;

			Views.Add(new View(viewId));

			return Views.Last();
		}

		/// <summary>
		/// Removes an existing view.
		/// </summary>
		/// <param name="view"></param>
		public static void Remove(View view)
		{
			Views.Remove(view);

            NativeMethods.View.RemoveView(view.Id);
		}

		/// <summary>
		/// The currently active view.
		/// </summary>
		public static View Active
		{
			get
			{
                var viewId = NativeMethods.View.GetActiveView();
				if(viewId == 0)
					return null;

				var view = Views.FirstOrDefault(x => x.Id == viewId);
				if(view != default(View))
					return view;

				Views.Add(new View(viewId));
				return Views.Last();
			}
			set
			{
                NativeMethods.View.SetActiveView(value.Id);
			}
		}
		#endregion

		internal View(uint viewId)
		{
			Id = viewId;
		}

		public Vec3 Position { get { return ViewParams.Position; } set { var viewParams = ViewParams; viewParams.Position = value; ViewParams = viewParams; } }
		public Quat Rotation { get { return ViewParams.Rotation; } set { var viewParams = ViewParams; viewParams.Rotation = value; ViewParams = viewParams; } }

		/// <summary>
		/// The current field of view.
		/// </summary>
		public float FieldOfView { get { return ViewParams.FieldOfView; } set { var viewParams = ViewParams; viewParams.FieldOfView = value; ViewParams = viewParams; } }

		public EntityId TargetId { get { return new EntityId(ViewParams.TargetId); } set { var viewParams = ViewParams; viewParams.TargetId = (uint)value._value; ViewParams = viewParams; } }

        internal ViewParams ViewParams { get { return NativeMethods.View.GetViewParams(Id); } set { NativeMethods.View.SetViewParams(Id, value); } }
		
		internal uint Id;

		static List<View> Views = new List<View>();
	}

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