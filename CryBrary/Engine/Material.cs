using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using System.Collections.Generic;
using System.Linq;

using CryEngine.Native;

namespace CryEngine
{
	public class Material
	{
		#region Statics
		public static Material Find(string name)
		{
			var ptr = NativeMethods.Material.FindMaterial(name);

			return TryAdd(ptr);
		}

		public static Material Create(string name, bool makeIfNotFound = true, bool nonRemovable = false)
		{
            var ptr = NativeMethods.Material.CreateMaterial(name);

			return TryAdd(ptr);
		}

		public static Material Load(string name, bool makeIfNotFound = true, bool nonRemovable = false)
		{
            var ptr = NativeMethods.Material.LoadMaterial(name, makeIfNotFound, nonRemovable);

			return TryAdd(ptr);
		}

		public static Material Get(EntityBase entity, int slot = 0)
		{
			if(entity == null)
				throw new ArgumentNullException("entity");

			var ptr = NativeMethods.Material.GetMaterial(entity.HandleRef.Handle, slot);
			return TryAdd(ptr);
		}

		public static void Set(EntityBase entity, Material mat, int slot = 0)
		{
			if(entity == null)
				throw new ArgumentNullException("entity");
			if(mat == null)
				throw new ArgumentNullException("mat");

			NativeMethods.Material.SetMaterial(entity.HandleRef.Handle, mat.HandleRef.Handle, slot);
		}

		internal static Material TryAdd(IntPtr ptr)
		{
			if(ptr == IntPtr.Zero)
				return null;

			var mat = Materials.FirstOrDefault(x => x.HandleRef.Handle == ptr);
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
			HandleRef = new HandleRef(this, ptr);
		}

		public Material GetSubmaterial(int slot)
		{
			var ptr = NativeMethods.Material.GetSubMaterial(HandleRef.Handle, slot);

			return TryAdd(ptr);
		}

		/// <summary>
		/// Clones a material
		/// </summary>
		/// <param name="subMaterial">If negative, all sub materials are cloned, otherwise only the specified slot is</param>
		/// <returns></returns>
		public Material Clone(int subMaterial = -1)
		{
			var ptr = NativeMethods.Material.CloneMaterial(HandleRef.Handle, subMaterial);

			return TryAdd(ptr);
		}

		public bool SetParam(string paramName, float value)
		{
			return NativeMethods.Material.SetGetMaterialParamFloat(HandleRef.Handle, paramName, ref value, false);
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

			return NativeMethods.Material.SetGetMaterialParamFloat(HandleRef.Handle, paramName, ref value, true);
		}

		public bool SetParam(string paramName, Color value)
		{
			Vec3 vecValue = new Vec3(value.R, value.G, value.B);
			var result = NativeMethods.Material.SetGetMaterialParamVec3(HandleRef.Handle, paramName, ref vecValue, false);

			Opacity = value.A;

			return result;
		}

		public Color GetParamColor(string paramName)
		{
			Color value;
			TryGetParam(paramName, out value);

			return value;
		}

		public bool TryGetParam(string paramName, out Color value)
		{
			Vec3 vecVal = Vec3.Zero;
			bool result = NativeMethods.Material.SetGetMaterialParamVec3(HandleRef.Handle, paramName, ref vecVal, true);

			value = new Color();
			value.R = vecVal.X;
			value.G = vecVal.Y;
			value.B = vecVal.Z;
			value.A = Opacity;

			return result;
		}

		public void SetShaderParam(string paramName, float newVal)
		{
			NativeMethods.Material.SetShaderParam(HandleRef.Handle, paramName, newVal);
		}

		public void SetShaderParam(ShaderFloatParameter param, float value)
		{
			SetShaderParam(param.GetEngineName(), value);
		}

		public void SetShaderParam(string paramName, Color newVal)
		{
			NativeMethods.Material.SetShaderParam(HandleRef.Handle, paramName, newVal);
		}

		public void SetShaderParam(ShaderColorParameter param, Color value)
		{
			SetShaderParam(param.GetEngineName(), value);
		}

		public void SetShaderParam(ShaderColorParameter param, Vec3 value)
		{
			SetShaderParam(param.GetEngineName(), new Color(value.X, value.Y, value.Z));
		}

		public string GetShaderParamName(int index)
		{
			return NativeMethods.Material.GetShaderParamName(HandleRef.Handle, index);
		}

		#region Fields & Properties
		public float AlphaTest { get { return GetParam("alpha"); } set { SetParam("alpha", value); } }
		public float Opacity { get { return GetParam("opacity"); } set { SetParam("opacity", value); } }
		public float Glow { get { return GetParam("glow"); } set { SetParam("glow", value); } }
		public float Shininess { get { return GetParam("shininess"); } set { SetParam("shininess", value); } }

		public Color DiffuseColor { get { return GetParamColor("diffuse"); } set { SetParam("diffuse", value); } }
		public Color EmissiveColor { get { return GetParamColor("emissive"); } set { SetParam("emissive", value); } }
		public Color SpecularColor { get { return GetParamColor("specular"); } set { SetParam("specular", value); } }

		public string SurfaceType { get { return NativeMethods.Material.GetSurfaceTypeName(HandleRef.Handle); } }

        public int ShaderParamCount { get { return NativeMethods.Material.GetShaderParamCount(HandleRef.Handle); } }

		public HandleRef HandleRef { get; set; }
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