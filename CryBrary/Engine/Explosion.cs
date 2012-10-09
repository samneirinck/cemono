using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using CryEngine.Native;

namespace CryEngine
{
	public class Explosion
	{
		public Explosion()
		{
			explosion = new pe_explosion
			{
				rminOcc = 0.07f,
				explDir = new Vec3(0, 0, 1)
			};

		}

		public void Explode()
		{
			if (explosion.rmax == 0)
				explosion.rmax = 0.0001f;
			explosion.nOccRes = explosion.rmax > 50 ? 0 : 16;

			NativeMethods.Physics.SimulateExplosion(explosion);
		}

		public Vec3 Epicenter { get { return explosion.epicenter; } set { explosion.epicenter = value; } }
		public Vec3 EpicenterImpulse { get { return explosion.epicenterImp; } set { explosion.epicenterImp = value; } }

		public Vec3 Direction { get { return explosion.explDir; } set { explosion.explDir = value; } }

		public float Radius { get { return explosion.r; } set { explosion.r = value; } }
		public float MinRadius { get { return explosion.rmin; } set { explosion.rmin = value; } }
		public float MaxRadius { get { return explosion.rmax; } set { explosion.rmax = value; } }

		public float ImpulsePressure { get { return explosion.impulsivePressureAtR; } set { explosion.impulsivePressureAtR = value; } }

		public float HoleSize { get { return explosion.holeSize; } set { explosion.holeSize = value; } }
		public int HoleType { get { return explosion.iholeType; } set { explosion.iholeType = value; } }

		public bool ForceEntityDeformation { get { return explosion.forceDeformEntities; } set { explosion.forceDeformEntities = value; } }

		internal pe_explosion explosion;
	}
}
