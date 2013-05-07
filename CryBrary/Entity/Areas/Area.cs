using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using CryEngine.Native;

namespace CryEngine
{
	public struct AreaQueryResult
	{
		IntPtr pArea;
		public Area Area
		{
			get
			{
				return Area.TryGet(pArea);
			}
		}

		float DistanceSquared { get; set; }
		Vec3 Position { get; set; }
		bool Inside { get; set; }
		bool Near { get; set; }
	}

	public class Area
	{
		#region Statics
		public static int AreaCount { get { return NativeEntityMethods.GetNumAreas(); } }

		public static Area GetArea(int areaId)
		{
			return TryGet(NativeEntityMethods.GetArea(areaId));
		}

		public static IEnumerable<AreaQueryResult> QueryAreas(Vec3 pos, int maxResults, bool forceCalculation)
		{
			var objAreas = NativeEntityMethods.QueryAreas(pos, maxResults, forceCalculation);

			return objAreas.Cast<AreaQueryResult>();
		}

		internal static Area TryGet(IntPtr ptr)
		{
			if (ptr == IntPtr.Zero)
				return null;

			var area = Areas.FirstOrDefault(x => x.Handle == ptr);
			if (area != null)
				return area;

			area = new Area(ptr);
			Areas.Add(area);

			return area;
		}

		static List<Area> Areas = new List<Area>();
		#endregion

		Area(IntPtr ptr)
		{
			Handle = ptr;
		}

		public void GetMinMax(ref Vec3 min, ref Vec3 max)
		{
			NativeEntityMethods.GetAreaMinMax(Handle, ref min, ref max);
		}

		public EntityId GetEntityIdByIndex(int index)
		{
			return NativeEntityMethods.GetAreaEntityByIdx(Handle, index);
		}

		public Vec3 Minimum
		{
			get
			{
				var min = Vec3.Zero;
				var max = Vec3.Zero;

				GetMinMax(ref min, ref max);

				return min;
			}
		}

		public Vec3 Maximum
		{
			get
			{
				var min = Vec3.Zero;
				var max = Vec3.Zero;

				GetMinMax(ref min, ref max);

				return max;
			}
		}

		public int EntityCount { get { return NativeEntityMethods.GetAreaEntityAmount(Handle); } }

		public int Priority { get { return NativeEntityMethods.GetAreaPriority(Handle); } }

		/// <summary>
		/// IArea pointer
		/// </summary>
		public IntPtr Handle { get; set; }
	}
}
