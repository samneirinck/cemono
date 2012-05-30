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
		extern internal static void _SetMaterial(IntPtr entityPtr, IntPtr materialPtr, int slot);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static IntPtr _CloneMaterial(IntPtr materialPtr, int subMtl);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static string _GetSurfaceTypeName(IntPtr ptr);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static bool _SetGetMaterialParamFloat(IntPtr ptr, string paramName, ref float v, bool get);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static bool _SetGetMaterialParamVec3(IntPtr ptr, string paramName, ref Vec3 v, bool get);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _SetShaderParam(IntPtr ptr, string paramName, float newVal);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _SetShaderParam(IntPtr ptr, string paramName, Color newVal);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static MaterialFlags _GetFlags(IntPtr ptr);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _SetFlags(IntPtr ptr, MaterialFlags flags);
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

		public static Material Get(EntityBase entity, int slot = 0)
		{
			if(entity == null)
				throw new ArgumentNullException("entity");

			var ptr = _GetMaterial(entity.EntityPointer, slot);
			return TryAdd(ptr);
		}

		public static void Set(EntityBase entity, Material mat, int slot = 0)
		{
			if(entity == null)
				throw new ArgumentNullException("entity");
			else if(mat == null)
				throw new ArgumentNullException("mat");

			_SetMaterial(entity.EntityPointer, mat.MaterialPointer, slot);
		}

		internal static Material TryAdd(IntPtr ptr)
		{
			if(ptr == null || ptr == IntPtr.Zero)
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

		/// <summary>
		/// Clones a material
		/// </summary>
		/// <param name="subMaterial">If negative, all sub materials are cloned, otherwise only the specified slot is</param>
		/// <returns></returns>
		public Material Clone(int subMaterial = -1)
		{
			var ptr = _CloneMaterial(MaterialPointer, subMaterial);

			return TryAdd(ptr);
		}

		public bool SetParam(string paramName, float value)
		{
			return _SetGetMaterialParamFloat(MaterialPointer, paramName, ref value, false);
		}

		public float GetParam(string paramName)
		{
			float value;
			TryGetParam(paramName, out value);

			return value;
		}

		public bool TryGetParam(string paramName, out float value)
		{
			value = 0;

			return _SetGetMaterialParamFloat(MaterialPointer, paramName, ref value, true);
		}

		public bool SetParam(string paramName, Vec3 value)
		{
			return _SetGetMaterialParamVec3(MaterialPointer, paramName, ref value, false);
		}

		public Vec3 GetParamVec3(string paramName)
		{
			Vec3 value;
			TryGetParam(paramName, out value);

			return value;
		}

		public bool TryGetParam(string paramName, out Vec3 value)
		{
			value = Vec3.Zero;

			return _SetGetMaterialParamVec3(MaterialPointer, paramName, ref value, true);
		}

		public void SetShaderParam(string paramName, float newVal)
		{
			_SetShaderParam(MaterialPointer, paramName, newVal);
		}

		public void SetShaderParam(ShaderFloatParameter param, float value)
		{
			SetShaderParam(param.GetEngineName(), value);
		}

		public void SetShaderParam(string paramName, Color newVal)
		{
			_SetShaderParam(MaterialPointer, paramName, newVal);
		}

		public void SetShaderParam(ShaderColorParameter param, Color value)
		{
			SetShaderParam(param.GetEngineName(), value);
		}

		public void SetShaderParam(ShaderColorParameter param, Vec3 value)
		{
			SetShaderParam(param.GetEngineName(), new Color(value.X, value.Y, value.Z));
		}

		#region Fields & Properties
		public float AlphaTest { get { return GetParam("alpha"); } set { SetParam("alpha", value); } }
		public float Opacity { get { return GetParam("opacity"); } set { SetParam("opacity", value); } }
		public float Glow { get { return GetParam("glow"); } set { SetParam("glow", value); } }
		public float Shininess { get { return GetParam("shininess"); } set { SetParam("shininess", value); } }

		public Vec3 DiffuseColor { get { return GetParamVec3("diffuse"); } set { SetParam("diffuse", value); } }
		public Vec3 EmissiveColor { get { return GetParamVec3("emissive"); } set { SetParam("emissive", value); } }
		public Vec3 SpecularColor { get { return GetParamVec3("specular"); } set { SetParam("specular", value); } }

		public string SurfaceType { get { return _GetSurfaceTypeName(MaterialPointer); } }

		internal IntPtr MaterialPointer { get; set; }
		#endregion
	}

	public enum ShaderFloatParameter
	{
		BlendFalloff,
		BendDetailLeafAmplitude,
		BackShadowBias,
		FresnelPower,
		DetailBendingFrequency,
		BendingBranchAmplitude,
		BlendLayer2Tiling,
		FresnelScale,
		FresnelBias,
		CapOpacityFalloff,
		BackViewDep,
		BackDiffuseColorScale,
		BlendFactor,
	}

	public enum ShaderColorParameter
	{
		BackDiffuseColor,
		IndirectBounceColor
	}

	public static class MaterialExtensions
	{
		public static string GetEngineName(this ShaderFloatParameter param)
		{
			switch(param)
			{
				case ShaderFloatParameter.BendDetailLeafAmplitude: return "bendDetailLeafAmplitude";
				case ShaderFloatParameter.DetailBendingFrequency: return "bendDetailFrequency";
				case ShaderFloatParameter.BendingBranchAmplitude: return "bendDetailBranchAmplitude";
				case ShaderFloatParameter.BackDiffuseColorScale: return "BackDiffuseMultiplier";
			}

			return param.ToString();
		}

		public static string GetEngineName(this ShaderColorParameter param)
		{
			switch(param)
			{
				case ShaderColorParameter.BackDiffuseColor: return "BackDiffuse";
				case ShaderColorParameter.IndirectBounceColor: return "IndirectColor";
			}

			return param.ToString();
		}
	}
}

[Flags]
public enum MaterialFlags
{
	Wire = 0x0001,   // Use wire frame rendering for this material.
	TWoSided = 0x0002,   // Use 2 Sided rendering for this material.
	Additive = 0x0004,   // Use Additive blending for this material.
	DetailDecal = 0x0008, // Massive decal technique
	Lighting = 0x0010,   // Should lighting be applied on this material.
	NoShadow = 0x0020,   // Material do not cast shadows.
	AlwaysUsed = 0x0040,   // When set forces material to be export even if not explicitly used.
	PureMaterial = 0x0080,   // Not shared sub material, sub material unique to his parent multi material.
	Multi_SubMaterial = 0x0100,   // This material is a multi sub material.
	NoPhysicalize = 0x0200, // Should not physicalize this material.
	NoDraw = 0x0400,   // Do not render this material.
	NoPreview = 0x0800,   // Cannot preview the material.
	NotInstanced = 0x1000,   // Do not instantiate this material.
	CollisionProxy = 0x2000,// This material is the collision proxy.
	Scatter = 0x4000,   // Use scattering for this material
	RequireForwardRendering = 0x8000,   // This material has to be rendered in foward rendering passes (alpha/additive blended)
	NonRemovable = 0x10000,  // Material with this flag once created are never removed from material manager (Used for decal materials, this flag should not be saved).
	HideOnBreak = 0x20000,   // Non-physicalized subsets with such materials will be removed after the object breaks
	UIMaterial = 0x40000,   // Used for UI in Editor. Don't need show it DB.	
	// Not CLS compliant (underscore): _64BitShadergenMask = 0x80000,   // ShaderGen mask is remapped
	RayCastProxy = 0x100000,
	RequireNearestCubemap = 0x200000,   // materials with alpha blending requires special processing for shadows
}