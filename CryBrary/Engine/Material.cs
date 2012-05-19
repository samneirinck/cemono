using System;
using System.Runtime.CompilerServices;

using System.Collections.Generic;
using System.Linq;

namespace CryEngine
{
	public class Material
	{
		#region Externals
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static IntPtr _CreateMaterial(string name);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static IntPtr _LoadMaterial(string name, bool makeIfNotFound = true, bool nonRemovable = false);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static IntPtr _GetSubMaterial(IntPtr materialPtr, int slot);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static IntPtr _GetMaterial(IntPtr entityPtr);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _SetMaterial(IntPtr entityPtr, IntPtr materialPtr);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static string _GetSurfaceTypeName(IntPtr ptr);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static bool _SetGetMaterialParamFloat(IntPtr ptr, string paramName, ref float v, bool get);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static bool _SetGetMaterialParamVec3(IntPtr ptr, string paramName, ref Vec3 v, bool get);
		#endregion

		#region Statics
		public static Material Create(string name, bool makeIfNotFound = true, bool nonRemovable = false)
		{
			var ptr = _CreateMaterial(name);

			return TryAdd(ptr);
		}

		public static Material Load(string name, bool makeIfNotFound = true, bool nonRemovable = false)
		{
			var ptr = _LoadMaterial(name, makeIfNotFound, nonRemovable);

			return TryAdd(ptr);
		}

		public static Material Get(EntityBase entity)
		{
			return TryAdd(_GetMaterial(entity.EntityPointer));
		}

		public static void Set(EntityBase entity, Material mat)
		{
			_SetMaterial(entity.EntityPointer, mat.Pointer);
		}

		internal static Material TryAdd(IntPtr ptr)
		{
			if(ptr != null)
			{
				var mat = Materials.First(x => x.Pointer == ptr);
				if(mat != null)
					return mat;
				else
				{
					mat = new Material(ptr);
					Materials.Add(mat);

					return mat;
				}
			}

			return null;
		}

		static List<Material> Materials = new List<Material>();
		#endregion

		internal Material(IntPtr ptr)
		{
			Pointer = ptr;
		}

		public Material GetSubmaterial(int slot)
		{
			var ptr = _GetSubMaterial(Pointer, slot);

			return TryAdd(ptr);
		}

		public void SetParam(string paramName, float value)
		{
			_SetGetMaterialParamFloat(Pointer, paramName, ref value, false);
		}

		public float GetParam(string paramName)
		{
			float value = 0;
			_SetGetMaterialParamFloat(Pointer, paramName, ref value, true);

			return value;
		}

		public void SetParamVec3(string paramName, Vec3 value)
		{
			_SetGetMaterialParamVec3(Pointer, paramName, ref value, false);
		}

		public Vec3 GetParamVec3(string paramName)
		{
			Vec3 value = Vec3.Zero;
			_SetGetMaterialParamVec3(Pointer, paramName, ref value, true);

			return value;
		}

		public string SurfaceType { get { return _GetSurfaceTypeName(Pointer); } }

		internal IntPtr Pointer { get; set; }
	}
}
