using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using CryEngine.Native;

namespace CryEngine
{
	public class LightSource
	{
		public LightSource()
		{
			LightSourcePtr = NativeMethods.Engine3D.CreateLightSource();
		}

		internal IntPtr LightSourcePtr { get; set; }

		internal LightParams Params { get { return NativeMethods.Engine3D.GetLightSourceParams(LightSourcePtr); } set { NativeMethods.Engine3D.SetLightSourceParams(LightSourcePtr, value); } }
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
