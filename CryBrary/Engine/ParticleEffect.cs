using System;
using System.Runtime.CompilerServices;

namespace CryEngine
{
	public class ParticleEffect
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static IntPtr _FindEffect(string effectName, bool loadResources);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _Spawn(IntPtr ptr, bool independent, Vec3 pos, Vec3 dir, float scale);

		/// <summary>
		/// </summary>
		/// <param name="effectName"></param>
		/// <param name="loadResources">Load all required resources?</param>
		/// <returns></returns>
		public static ParticleEffect Get(string effectName, bool loadResources = true)
		{
			var ptr = _FindEffect(effectName, loadResources);
			if(ptr != null)
				return new ParticleEffect(ptr);

			return null;
		}

		internal ParticleEffect(IntPtr ptr)
		{
			Pointer = ptr;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="independent"></param>
		/// <param name="pos"></param>
		/// <param name="dir">Is set to Vec3.Up if null</param>
		/// <param name="scale"></param>
		public void Spawn(Vec3 pos, Vec3? dir = null, float scale = 1f, bool independent = true)
		{
			_Spawn(Pointer, independent, pos, dir ?? Vec3.Up, scale);
		}

		internal IntPtr Pointer { get; set; }
	}
}