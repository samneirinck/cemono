using System;
using System.Runtime.CompilerServices;
using CryEngine.Native;

namespace CryEngine
{
	public class ParticleEffect
	{
		/// <summary>
		/// </summary>
		/// <param name="effectName"></param>
		/// <param name="loadResources">Load all required resources?</param>
		/// <returns></returns>
		public static ParticleEffect Get(string effectName, bool loadResources = true)
		{
            var ptr = NativeMethods.Particle.FindEffect(effectName, loadResources);
			if(ptr != IntPtr.Zero)
				return new ParticleEffect(ptr);

			return null;
		}

		internal ParticleEffect(IntPtr ptr)
		{
			Pointer = ptr;
		}

		/// <summary>
		/// Spawns this effect
		/// </summary>
		/// <param name="independent"></param>
		/// <param name="pos">World location to place emitter at.</param>
		/// <param name="dir">World rotation of emitter, set to Vec3.Up if null.</param>
		/// <param name="scale">Scale of the emitter.</param>
		public void Spawn(Vec3 pos, Vec3? dir = null, float scale = 1f, bool independent = true)
		{
            NativeMethods.Particle.Spawn(Pointer, independent, pos, dir ?? Vec3.Up, scale);
		}

		public void Remove()
		{
			NativeMethods.Particle.Remove(Pointer);
		}

		public void LoadResources()
		{
			NativeMethods.Particle.LoadResoruces(Pointer);
		}

		internal IntPtr Pointer { get; set; }
	}
}