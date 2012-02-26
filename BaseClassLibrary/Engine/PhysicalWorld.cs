using System;
using System.Runtime.CompilerServices;

namespace CryEngine
{
    public class PhysicalWorld
    {
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _RayWorldIntersection(string gamemode);

		public struct RaycastHit
		{
			RayHit _info;

			internal RaycastHit(RayHit hit)
			{
				_info = hit;
			}

			public float Distance { get { return _info.dist; } }
			public EntityId Collider { get { return _info.colliderId; } }
			public Vec3 Point { get { return _info.pt; } }
			public Vec3 Normal { get { return _info.n; } }
		}

        internal struct RayHit
        {
            internal float dist;
	        internal uint colliderId;
	        internal int ipart;
	        internal int partid;
	        internal  short surface_idx;
	        internal  short idmatOrg;	// original material index, not mapped with material mapping
	        internal int foreignIdx;
	        internal int iNode; // BV tree node that had the intersection; can be used for "warm start" next time
	        internal Vec3 pt;
	        internal Vec3 n;	// surface normal
	        internal int bTerrain;	// global terrain hit
	        internal int iPrim; // hit triangle index
        };

        [Flags]
        public enum RayWorldIntersectionFlag
        {
            IgnoreTerrainHole = 0x20,
            IgnoreNonColliding = 0x40,
            IgnoreBackfaces = 0x80,
            IgnoreSolidBackfaces = 0x100,
            StopAtPierceable = 0x0F,
            /// <summary>
            /// among pierceble hits, materials with sf_important will have priority
            /// </summary>
            SeperateImportantHits = 0x200,
            /// <summary>
            /// if several colltype flag are specified, switches between requiring all or any of them in a geometry
            /// </summary>
            CollissionTypeAny = 0x400,
            ForcePiercableNonCollidable = 0x1000, // non-colliding geometries will be treated as pierceable regardless of the actual material
            AnyHit = 0x8000 // returns the first found hit for meshes, not necessarily the closets
        }

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
		public static int RayWorldIntersection(Vec3 origin, Vec3 dir, int objectTypes, RayWorldIntersectionFlag flags, out RaycastHit hits, int maxHits, EntityId[] skipEntities)
        {
			hits = new RaycastHit();

            return -1;
        }
    }
}
