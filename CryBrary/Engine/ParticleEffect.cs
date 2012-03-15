using System.Runtime.CompilerServices;

namespace CryEngine
{
	public class ParticleEffect
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _FindEffect(string effectName, bool loadResources);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _Spawn(int id, bool independent, Vec3 pos, Vec3 dir, float scale);

		/// <summary>
		/// </summary>
		/// <param name="effectName"></param>
		/// <param name="loadResources">Load all required resources?</param>
		/// <returns></returns>
		public static ParticleEffect Get(string effectName, bool loadResources = true)
		{
			int id = _FindEffect(effectName, loadResources);
			if(id != -1)
				return new ParticleEffect(id);

			return null;
		}

		internal ParticleEffect(int id)
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
			_Spawn(Id, independent, pos, dir ?? Vec3.Up, scale);
		}

		public int Id { get; private set; }
	}
}