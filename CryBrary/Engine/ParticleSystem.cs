using System.Runtime.CompilerServices;

namespace CryEngine
{
	public static class ParticleSystem
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _FindEffect(string effectName, bool loadResources);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _SpawnEffect(int id, bool independent, Vec3 pos, Vec3 dir, float scale);

		/// <summary>
		/// </summary>
		/// <param name="effectName"></param>
		/// <param name="loadResources">Load all required resources?</param>
		/// <returns></returns>
		public static ParticleEffect GetEffect(string effectName, bool loadResources = true)
		{
			int id = _FindEffect(effectName, loadResources);
			if(id != -1)
				return new ParticleEffect(id);

			return null;
		}
	}
}