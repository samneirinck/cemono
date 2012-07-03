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
}