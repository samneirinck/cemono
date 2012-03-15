using System.Runtime.CompilerServices;

namespace CryEngine
{
	public class Material
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _CreateMaterial(string name);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _LoadMaterial(string name, bool makeIfNotFound = true, bool nonRemovable = false);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static string _GetSurfaceTypeName(int Id);

		public static Material Load(string name, bool makeIfNotFound = true, bool nonRemovable = false)
		{
			int id = _LoadMaterial(name, makeIfNotFound, nonRemovable);
			if(id != -1)
				return new Material(id);

			return null;
		}

		public static Material Create(string name, bool makeIfNotFound = true, bool nonRemovable = false)
		{
			int id = _CreateMaterial(name);
			if(id != -1)
				return new Material(id);

			return null;
		}

		internal Material(int id)
		{
			Id = id;
		}

		public string SurfaceType { get { return _GetSurfaceTypeName(Id); } }

		public int Id { get; private set; }
	}
}
