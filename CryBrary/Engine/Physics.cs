using System;
using System.Runtime.CompilerServices;
using CryEngine.Native;

namespace CryEngine
{
	public struct RayHit
	{
		internal float dist;
		internal int colliderId;
		internal int ipart;
		internal int partid;
		internal short surface_idx;
		internal short idmatOrg;	// original material index, not mapped with material mapping
		internal int foreignIdx;
		internal int iNode; // BV tree node that had the intersection; can be used for "warm start" next time
		internal Vec3 pt;
		internal Vec3 n;	// surface normal
		internal int bTerrain;	// global terrain hit
		internal int iPrim; // hit triangle index
	}

	public struct RaycastHit
	{
		RayHit _info;

		internal RaycastHit(RayHit hit)
		{
			_info = hit;
		}

		public override bool Equals(object obj)
		{
			if(obj is RaycastHit)
				return obj.GetHashCode() == GetHashCode();

			return false;
		}

        public static bool operator ==(RaycastHit a, RaycastHit b)
        {
            return a.Equals(b);
        }

        public static bool operator !=(RaycastHit a, RaycastHit b)
        {
            return !(a == b);
        }

		public override int GetHashCode()
		{
			int hash = 17;

			hash = hash * 29 + Distance.GetHashCode();
			hash = hash * 29 + ColliderId.GetHashCode();
			hash = hash * 29 + Point.GetHashCode();
			hash = hash * 29 + Normal.GetHashCode();

			return hash;
		}

		public float Distance { get { return _info.dist; } }
		/// <summary>
		/// PhysicalEntityId, not yet implemented.
		/// </summary>
		public int ColliderId { get { return _info.colliderId; } }
		public Vec3 Point { get { return _info.pt; } }
		public Vec3 Normal { get { return _info.n; } }
	}

	[Flags]
	public enum SurfaceFlags
	{
		PierceableMask = 0x0F,
		MaxPierceable = 0x0F,
		Important = 0x200,
		ManuallyBreakable = 0x400,
		MaterialBreakableBit = 16
	}

	[Flags]
	public enum RayWorldIntersectionFlags
	{
		IgnoreTerrainHole = 0x20,
		IgnoreNonColliding = 0x40,
		IgnoreBackfaces = 0x80,
		IgnoreSolidBackfaces = 0x100,
		PierceabilityMask = 0x0F,
		Pierceability = 0,
		StopAtPierceable = 0x0F,
		/// <summary>
		/// among pierceble hits, materials with sf_important will have priority
		/// </summary>
		SeperateImportantHits = SurfaceFlags.Important,
		/// <summary>
		/// used to manually specify collision geometry types (default is geom_colltype_ray)
		/// </summary>
		CollissionTypeBit = 16,
		/// <summary>
		/// if several colltype flag are specified, switches between requiring all or any of them in a geometry
		/// </summary>
		CollissionTypeAny = 0x400,
		/// <summary>
		/// queues the RWI request, when done it'll generate EventPhysRWIResult
		/// </summary>
		Queue = 0x800,
		/// <summary>
		/// non-colliding geometries will be treated as pierceable regardless of the actual material
		/// </summary>
		ForcePiercableNonCollidable = 0x1000,
		/// <summary>
		/// marks the rwi to be a debug rwi (used for spu debugging, only valid in non-release builds)
		/// </summary>
		DebugTrace = 0x2000,
		/// <summary>
		/// update phitLast with the current hit results (should be set if the last hit should be reused for a "warm" start)
		/// </summary>
		UpdateLastHit = 0x4000,
		/// <summary>
		/// returns the first found hit for meshes, not necessarily the closest
		/// </summary>
		AnyHit = 0x8000
	}

	/// <summary>
	/// Used for GetEntitiesInBox and RayWorldIntersection
	/// </summary>
	[Flags]
	public enum EntityQueryFlags
	{
		Static = 1, SleepingRigid = 2, Rigid = 4, Living = 8, Independent = 16, Deleted = 128, Terrain = 0x100,
		All = Static | SleepingRigid | Rigid | Living | Independent | Terrain,
		FlaggedOnly = 0x800, SkipFlagged = FlaggedOnly * 2, // "flagged" meas has pef_update set
		Areas = 32, Triggers = 64,
		IgnoreNonColliding = 0x10000,
		/// <summary>
		/// sort by mass in ascending order
		/// </summary>
		SortByMass = 0x20000,
		/// <summary>
		/// if not set, the function will return an internal pointer
		/// </summary>
		AllocateList = 0x40000,
		/// <summary>
		/// will call AddRef on each entity in the list (expecting the caller call Release)
		/// </summary>
		AddRefResults = 0x100000,
		/// <summary>
		/// can only be used in RayWorldIntersection
		/// </summary>
		Water = 0x200,
		/// <summary>
		/// can only be used in RayWorldIntersection
		/// </summary>
		NoOnDemandActivation = 0x80000,
		/// <summary>
		/// queues procedural breakage requests; can only be used in SimulateExplosion
		/// </summary>
		DelayedDeformations = 0x80000
	}

	internal struct pe_explosion
	{
		public Vec3 epicenter;	// epicenter for the occlusion computation
		public Vec3 epicenterImp; // epicenter for impulse computation
		// the impulse a surface fragment with area dS and normal n gets is: dS*k*n*max(0,n*dir_to_epicenter)/max(rmin, dist_to_epicenter)^2
		// k is selected in such way that at impulsivePressureAtR = k/r^2
		public float rmin, rmax, r;
		public float impulsivePressureAtR;
		public int nOccRes; // resolution of the occlusion map (0 disables)
		public int nGrow; // grow occlusion projections by this amount of cells to allow explosion to reach around corners a bit
		public float rminOcc; // ignores geometry closer than this for occlusion computations
		public float holeSize;	// explosion shape for iholeType will be scaled by this holeSize / shape's declared size
		public Vec3 explDir;	// hit direction, for aligning the explosion boolean shape
		public int iholeType; // breakability index for the explosion (<0 disables)
		public bool forceDeformEntities; // force deformation even if breakImpulseScale is zero
		// filled as results
		IntPtr pAffectedEnts;
		IntPtr pAffectedEntsExposure;	// 0..1 exposure, computed from the occlusion map
		public int nAffectedEnts;
	}
}
