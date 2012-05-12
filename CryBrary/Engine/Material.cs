using System;
using System.Runtime.CompilerServices;

namespace CryEngine
{
	public class Material
	{
		#region Statics
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static IntPtr _CreateMaterial(string name);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static IntPtr _LoadMaterial(string name, bool makeIfNotFound = true, bool nonRemovable = false);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static string _GetSurfaceTypeName(IntPtr ptr);

		public static Material Load(string name, bool makeIfNotFound = true, bool nonRemovable = false)
		{
			var ptr = _LoadMaterial(name, makeIfNotFound, nonRemovable);
			if(ptr != null)
				return new Material(ptr);

			return null;
		}

		public static Material Create(string name, bool makeIfNotFound = true, bool nonRemovable = false)
		{
			var ptr = _CreateMaterial(name);
			if(ptr != null)
				return new Material(ptr);

			return null;
		}
		#endregion

		internal Material(IntPtr ptr)
		{
			Pointer = ptr;
		}

		public string SurfaceType { get { return _GetSurfaceTypeName(Pointer); } }

		internal IntPtr Pointer { get; set; }
	}
}
