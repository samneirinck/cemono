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

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern public static int _GetWidth();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern public static int _GetHeight();

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern public static int _LoadTexture(string path);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern public static void _DrawTextureToScreen(float xpos, float ypos, float width, float height, int textureId);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern public static int _CreateRenderTarget(int width, int height, int flags);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern public static void _DestroyRenderTarget(int id);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern public static void _SetRenderTarget(int id);

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
