using System;
using System.Runtime.CompilerServices;

using System.Linq;

namespace CryEngine
{
    public class PhysicalWorld
    {
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _RayWorldIntersection(Vec3 origin, Vec3 dir, EntityQueryFlags objFlags, RayWorldIntersectionFlags flags, ref RayHit rayHit, int maxHits, object[] skipEnts);

        internal struct RayHit
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
        };

		/// <summary>
		/// Steps through the entity grid and raytraces entities
		/// traces a finite ray from org along dir
		/// </summary>
		/// <param name="origin"></param>
		/// <param name="dir"></param>
		/// <param name="objectTypes"></param>
		/// <param name="flags"></param>
		/// <param name="hits"></param>
		/// <param name="maxHits"></param>
		/// <param name="skipEntities"></param>
		/// <returns>The total amount of hits detected (solid and pierceable)</returns>
		public static int RayWorldIntersection(Vec3 origin, Vec3 dir, EntityQueryFlags objectTypes, RayWorldIntersectionFlags flags, out RaycastHit hits, int maxHits = 1, EntityId[] skipEntities = null)
        {
			var internalRayHit = new RayHit();

			object[] skippedEntities = null;
			if(skipEntities != null && skipEntities.Count() > 0)
				skippedEntities = skipEntities.Cast<object>().ToArray(); 

			int rayResult = _RayWorldIntersection(origin, dir, objectTypes, flags, ref internalRayHit, maxHits, skippedEntities);

			hits = new RaycastHit(internalRayHit);

            return rayResult;
        }
    }

	public struct RaycastHit
	{
		PhysicalWorld.RayHit _info;

		internal RaycastHit(PhysicalWorld.RayHit hit)
		{
			_info = hit;
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
		Pierceabiltiy = 0,
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
}
