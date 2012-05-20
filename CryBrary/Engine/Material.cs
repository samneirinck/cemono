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
		extern internal static IntPtr _GetMaterial(IntPtr entityPtr, int slot);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _SetMaterial(IntPtr entityPtr, IntPtr materialPtr);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static string _GetSurfaceTypeName(IntPtr ptr);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static bool _SetGetMaterialParamFloat(IntPtr ptr, string paramName, ref float v, bool get);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static bool _SetGetMaterialParamVec3(IntPtr ptr, string paramName, ref Vec3 v, bool get);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _SetShaderParam(IntPtr ptr, string paramName, float newVal);
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
			if(entity == null)
				throw new ArgumentNullException("entity");

			var ptr = _GetMaterial(entity.EntityPointer, 0);
			return TryAdd(ptr);
		}

		public static void Set(EntityBase entity, Material mat)
		{
			_SetMaterial(entity.EntityPointer, mat.MaterialPointer);
		}

		internal static Material TryAdd(IntPtr ptr)
		{
			if(ptr == null)
				return null;

			var mat = Materials.FirstOrDefault(x => x.MaterialPointer == ptr);
			if(mat != default(Material))
				return mat;

			mat = new Material(ptr);
			Materials.Add(mat);

			return mat;
		}

		static List<Material> Materials = new List<Material>();
		#endregion

		internal Material(IntPtr ptr)
		{
			MaterialPointer = ptr;
		}

		public Material GetSubmaterial(int slot)
		{
			var ptr = _GetSubMaterial(MaterialPointer, slot);

			return TryAdd(ptr);
		}

		public void SetParam(string paramName, float value)
		{
			_SetGetMaterialParamFloat(MaterialPointer, paramName, ref value, false);
		}

		public float GetParam(string paramName)
		{
			float value = 0;
			_SetGetMaterialParamFloat(MaterialPointer, paramName, ref value, true);

			return value;
		}

		public void SetParamVec3(string paramName, Vec3 value)
		{
			_SetGetMaterialParamVec3(MaterialPointer, paramName, ref value, false);
		}

		public Vec3 GetParamVec3(string paramName)
		{
			Vec3 value = Vec3.Zero;
			_SetGetMaterialParamVec3(MaterialPointer, paramName, ref value, true);

			return value;
		}

		public void SetShaderParam(string paramName, float newVal)
		{
			_SetShaderParam(MaterialPointer, paramName, newVal);
		}

		#region Fields & Properties
		public float AlphaTest { get { return GetParam("alphatest"); } set { SetParam("alphatest", value); } }
		public Vec3 DiffuseColor { get { return GetParamVec3("diffuse"); } set { SetParamVec3("diffuse", value); } }

		public string SurfaceType { get { return _GetSurfaceTypeName(MaterialPointer); } }

		internal IntPtr MaterialPointer { get; set; }
		#endregion
	}
}
