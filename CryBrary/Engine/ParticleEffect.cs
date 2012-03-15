using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine
{
	public class ParticleEffect
	{
		public ParticleEffect(int id)
		{
			Id = id;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="independent"></param>
		/// <param name="pos"></param>
		/// <param name="dir">Is set to Vec3.Up if null</param>
		/// <param name="scale"></param>
		public void Spawn(bool independent, Vec3 pos, Vec3? dir = null, float scale = 1f)
		{
			ParticleSystem._SpawnEffect(Id, independent, pos, dir ?? Vec3.Up, scale);
		}

		public int Id { get; private set; }
	}
}