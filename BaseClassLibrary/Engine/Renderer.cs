using System.Runtime.CompilerServices;

namespace CryEngine
{
	/// <summary>
	/// Renderer functionality; TODO
	/// </summary>
	public class Renderer
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static Camera _GetViewCamera();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _SetViewCamera(Camera cam);

		public static Camera Camera { get { return _GetViewCamera(); } set { _SetViewCamera(value); } }
	}

	public class Camera
	{
		public Camera() { }

		public Vec3 ViewDir;
		public Vec3 Position;

		public float FieldOfView;
	}
}
