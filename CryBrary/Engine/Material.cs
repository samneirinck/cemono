using System.Runtime.CompilerServices;

namespace CryEngine
{
	public class Material
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static bool _MaterialExists(string name);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _CreateMaterial(string name);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _LoadMaterial(string name, bool makeIfNotFound = true, bool nonRemovable = false);
	}
}
