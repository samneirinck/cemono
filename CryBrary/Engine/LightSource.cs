using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using CryEngine.Native;

namespace CryEngine
{
	public class LightSource
	{
		public LightSource(Vec3 pos)
		{
			LightSourcePtr = NativeMethods.Engine3D.CreateLightSource();

			LightParams parameters = new LightParams();
			parameters.coronaScale = parameters.coronaDistSizeFactor = parameters.coronaDistIntensityFactor = 1;
			parameters.diffuseColor = new Color(1, 1, 1, 1);
			parameters.origin = pos;
			parameters.lightFrustumAngle = 45.0f;
			parameters.radius = 4.0f;
			parameters.flags = LightFlags.LightSource;
			parameters.specularMultiplier = 1.0f;
			parameters.specularCubemap = "";
			parameters.diffuseCubemap = "";

			Params = parameters;
		}

		internal IntPtr LightSourcePtr { get; set; }

		internal LightParams Params { get { return NativeMethods.Engine3D.GetLightSourceParams(LightSourcePtr); } set { NativeMethods.Engine3D.SetLightSourceParams(LightSourcePtr, value); } }

		public int LightStyle { get { return Params.lightStyle; } set { var parameters = Params; parameters.lightStyle = value; Params = parameters; } }
		public Vec3 Origin { get { return Params.origin; } set { var parameters = Params; parameters.origin = value; Params = parameters; } }
		public float LightFrustumAngle { get { return Params.lightFrustumAngle; } set { var parameters = Params; parameters.lightFrustumAngle = value; Params = parameters; } }
		public float Radius { get { return Params.radius; } set { var parameters = Params; parameters.radius = value; Params = parameters; } }

		public LightFlags Flags { get { return Params.flags; } set { var parameters = Params; parameters.flags = value; Params = parameters; } }

		public float CoronaScale { get { return Params.coronaScale; } set { var parameters = Params; parameters.coronaScale = value; Params = parameters; } }
		public float CoronaDistSizeFactor { get { return Params.coronaDistSizeFactor; } set { var parameters = Params; parameters.coronaDistSizeFactor = value; Params = parameters; } }
		public float CoronaDistIntensityFactor { get { return Params.coronaDistIntensityFactor; } set { var parameters = Params; parameters.coronaDistIntensityFactor = value; Params = parameters; } }

		// TODO: Change to Texture class when we've implemented it.
		public string SpecularCubemap { get { return Params.specularCubemap; } set { var parameters = Params; parameters.specularCubemap = value; Params = parameters; } }
		public string DiffuseCubemap { get { return Params.diffuseCubemap; } set { var parameters = Params; parameters.diffuseCubemap = value; Params = parameters; } }

		public Color Color { get { return Params.diffuseColor; } set { var parameters = Params; parameters.diffuseColor = value; Params = parameters; } }
		public float SpecularMultiplier { get { return Params.specularMultiplier; } set { var parameters = Params; parameters.specularMultiplier = value; Params = parameters; } }
		public int PostEffect { get { return Params.postEffect; } set { var parameters = Params; parameters.postEffect = value; Params = parameters; } }

		public float HDRDynamic { get { return Params.hdrDynamic; } set { var parameters = Params; parameters.hdrDynamic = value; Params = parameters; } }
		public float ProjectNearPlane { get { return Params.projectNearPlane; } set { var parameters = Params; parameters.projectNearPlane = value; Params = parameters; } }

		public Matrix34 Transform { get { return NativeMethods.Engine3D.GetLightSourceMatrix(LightSourcePtr); } set { NativeMethods.Engine3D.SetLightSourceMatrix(LightSourcePtr, value); } }
	}

	[Flags]
	public enum LightFlags : uint
	{
		Directional = 2,
		CastShadows = 0x10,
		Point = 0x20,
		Project = 0x40,
		HasCBuffer = 0x80,
		ReflectiveShadowmap = 0x100,
		IgnoreVisAreas = 0x200,
		DeferredCubemaps = 0x400,
		DeferredIndirectLight = 0x800,
		Disabled = 0x1000,
		HasClipBound = 0x4000,
		LightSource = 0x10000,
		Fake = 0x20000,
		Sun = 0x40000,
		Local = 0x100000,
		LM = 0x200000,
		ThisAreaOnly = 0x400000,
		AmbientLight = 0x800000,
		Negative = 0x1000000,
		IndoorOnly = 0x2000000,
		HighSpecOnly = 0x4000000,
		SpecularHighSpecOnly = 0x8000000,
		DeferredLight = 0x10000000,
		IraddianceVolumes = 0x20000000,
		SpecularOcclusion = 0x40000000,
		DiffuseOcclusion = 0x80000000
	}

	internal struct LightParams
	{
		public int lightStyle;
		public Vec3 origin;
		public float lightFrustumAngle;
		public float radius;
		public LightFlags flags;

		public float coronaScale;
		public float coronaDistSizeFactor;
		public float coronaDistIntensityFactor;

		public string specularCubemap;
		public string diffuseCubemap;

		public Color diffuseColor;
		public float specularMultiplier;
		public int postEffect;

		public float hdrDynamic;

		public float projectNearPlane;
	}
}
