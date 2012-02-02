using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.CompilerServices;

namespace CryEngine
{
    public class PhysicalWorld
    {
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _RayWorldIntersection(string gamemode);

        public struct RayHit
        {
            float dist;
	        uint colliderId;
	        int ipart;
	        int partid;
	        short surface_idx;
	        short idmatOrg;	// original material index, not mapped with material mapping
	        int foreignIdx;
	        int iNode; // BV tree node that had the intersection; can be used for "warm start" next time
	        Vec3 pt;
	        Vec3 n;	// surface normal
	        int bTerrain;	// global terrain hit
	        int iPrim; // hit triangle index
        };

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
        public static int RayWorldIntersection(Vec3 origin, Vec3 dir, int objectTypes, RayWorldIntersectionFlag flags, out RayHit hits, int maxHits, uint[] skipEntities)
        {
			hits = new RayHit();

            return -1;
        }
    }
}
