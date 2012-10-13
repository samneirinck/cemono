using System;
using System.Collections.Generic;
using System.Linq;
using CryEngine.Native;

namespace CryEngine
{
	[Obsolete]
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

		public Vec3 Position { get { return NativeMethods.View.GetViewPosition(Id); } set { NativeMethods.View.SetViewPosition(Id, value); } }
		public Quat Rotation { get { return NativeMethods.View.GetViewRotation(Id); } set { NativeMethods.View.SetViewRotation(Id, value); } }

		/// <summary>
		/// The current field of view.
		/// </summary>
		public float FieldOfView { get { return NativeMethods.View.GetViewFieldOfView(Id); } set { NativeMethods.View.SetViewFieldOfView(Id, value); } }

		internal uint Id;

		static List<View> Views = new List<View>();
	}
}