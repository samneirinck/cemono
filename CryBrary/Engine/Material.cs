using System.Runtime.CompilerServices;

namespace CryEngine
{
	public class Material
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _CreateMaterial(string name);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _LoadMaterial(string name, bool makeIfNotFound = true, bool nonRemovable = false);
	}
}
