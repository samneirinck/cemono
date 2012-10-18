using System;
using CryEngine.Native;

namespace CryEngine
{
	public struct Explosion
	{
		public void Explode()
		{
			rminOcc = 0.07f;
			if(Direction.IsZero)
				Direction = new Vec3(0, 0, 1);

			if (MaxRadius == 0)
				MaxRadius = 0.0001f;

			nOccRes = MaxRadius > 50 ? 0 : 16;

			NativeMethods.Physics.SimulateExplosion(this);
		}

		/// <summary>
		/// epicenter for the occlusion computation
		/// </summary>
		public Vec3 Epicenter { get; set; }

		/// <summary>
		/// epicenter for impulse computation
		/// </summary>
		public Vec3 EpicenterImpulse { get; set; }

		public float MinRadius { get; set; }
		public float MaxRadius { get; set; }
		public float Radius { get; set; }

		/// <summary>
		/// Pressure at r
		/// </summary>
		public float ImpulsePressure { get; set; }

		int nOccRes { get; set; } // resolution of the occlusion map (0 disables)
		int nGrow { get; set; } // grow occlusion projections by this amount of cells to allow explosion to reach around corners a bit
		float rminOcc { get; set; } // ignores geometry closer than this for occlusion computations

		/// <summary>
		/// explosion shape for iholeType will be scaled by this holeSize / shape's declared size
		/// </summary>
		public float HoleSize { get; set; }

		/// <summary>
		/// hit direction, for aligning the explosion boolean shape
		/// </summary>
		public Vec3 Direction { get; set; }
		/// <summary>
		/// breakability index for the explosion (less than 0 disables)
		/// </summary>
		/// 
		public int iHoleType { get; set; }

		/// <summary>
		/// force deformation even if breakImpulseScale is zero
		/// </summary>
		public bool ForceEntityDeformation { get; set; }

		// filled as results
		IntPtr pAffectedEnts { get; set; }

		IntPtr pAffectedEntsExposure { get; set; }	// 0..1 exposure, computed from the occlusion map

		internal int nAffectedEnts { get; set; }
	}
}
