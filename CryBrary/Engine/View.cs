using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;

namespace CryEngine
{
	public class View
	{
		#region Externals
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static uint _GetView(uint linkedEntityId, bool forceCreate = false);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _RemoveView(uint viewId);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static uint _GetActiveView();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _SetActiveView(uint viewId);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static ViewParams _GetViewParams(uint viewId);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _SetViewParams(uint viewId, ViewParams cam);
		#endregion

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
			var viewId = _GetView(linkedEntity, forceCreate);
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

			_RemoveView(view.Id);
		}

		/// <summary>
		/// The currently active view.
		/// </summary>
		public static View Active
		{
			get
			{
				var viewId = _GetActiveView();
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
				_SetActiveView(value.Id);
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
		public float FoV { get { return ViewParams.FieldOfView; } set { var viewParams = ViewParams; viewParams.FieldOfView = value; ViewParams = viewParams; } }

		public EntityId TargetId { get { return new EntityId(ViewParams.TargetId); } set { var viewParams = ViewParams; viewParams.TargetId = (uint)value._value; ViewParams = viewParams; } }

		internal ViewParams ViewParams { get { return _GetViewParams(Id); } set { _SetViewParams(Id, value); } }
		
		internal uint Id;

		static List<View> Views = new List<View>();
	}
}