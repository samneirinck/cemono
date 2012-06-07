using System;
using System.Runtime.CompilerServices;


namespace CryEngine
{
	public class GlobalPhysics
	{
		#region Externals
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static IntPtr _GetPhysicalEntity(IntPtr entityPointer);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _RayWorldIntersection(Vec3 origin, Vec3 dir, EntityQueryFlags objFlags, RayWorldIntersectionFlags flags, ref RayHit rayHit, int maxHits, object[] skipEnts);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _Physicalize(IntPtr entPtr, PhysicalizationParams physicalizationParams);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _Sleep(IntPtr entPtr, bool sleep);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _AddImpulse(IntPtr entPtr, ActionImpulse actionImpulse);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static Vec3 _GetVelocity(IntPtr entPtr);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _SetVelocity(IntPtr entPtr, Vec3 velocity);
		#endregion

		const string gravityCVar = "p_gravity_z";
		public static float GravityZ
		{
			get { return CVar.Get(gravityCVar).FVal; }
			set { CVar.Get(gravityCVar).FVal = value; }
		}
	}

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
}
