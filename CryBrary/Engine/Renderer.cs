using System.Runtime.CompilerServices;

namespace CryEngine
{
	public static class Renderer
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _GetWidth();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _GetHeight();

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static Vec3 _ScreenToWorld(int x, int y);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _DrawTextToScreen(float x, float y, float fontSize, Color color, bool center, string text);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _LoadTexture(string path);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _DrawTextureToScreen(float xpos, float ypos, float width, float height, int textureId, float s0 = 0, float t0 = 0, float s1 = 1, float t1 = 1, float angle = 0, float r = 1, float g = 1, float b = 1, float a = 1, float z = 1);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _CreateRenderTarget(int width, int height, int flags);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _DestroyRenderTarget(int id);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _SetRenderTarget(int id);

		public static Vec3 ScreenToWorld(int x, int y)
		{
			return _ScreenToWorld(x, y);
		}

		public static void DrawTextToScreen(float x, float y, float fontSize, Color color, string text, params object[] args)
		{
			DrawTextToScreen(x, y, fontSize, color, false, text, args);
		}

		public static void DrawTextToScreen(float x, float y, float fontSize, Color color, bool centered, string text, params object[] args)
		{
			_DrawTextToScreen(x, y, fontSize, color, centered, string.Format(text, args));
		}

		/// <summary>
		/// The width of the screen in pixels.
		/// </summary>
		public static int ScreenHeight { get { return _GetHeight(); } }

		/// <summary>
		/// The height of the screen in pixels.
		/// </summary>
		public static int ScreenWidth { get { return _GetWidth(); } }

		/// <summary>
		/// Loads a texture into memory and generates its ID.
		/// </summary>
		/// <param name="texturePath">The path to the texture.</param>
		/// <returns></returns>
		public static int LoadTexture(string texturePath)
		{
			return _LoadTexture(texturePath);
		}

		/// <summary>
		/// Draws a texture to the screen for one frame.
		/// </summary>
		/// <param name="xPos">The x position on the screen, relative to the top-left corner.</param>
		/// <param name="yPos">The y position on the screen, relative to the top-left corner.</param>
		/// <param name="width">The width to render the texture at.</param>
		/// <param name="height">The height to render the texture at.</param>
		/// <param name="textureId">The ID of the texture (obtained using Renderer.LoadTexture(path)).</param>
		public static void DrawTexture(int xPos, int yPos, int width, int height, int textureId)
		{
			_DrawTextureToScreen(xPos, yPos, width, height, textureId);
		}

		public static int CreateRenderTarget(int width, int height, int flags = 0)
		{
			return _CreateRenderTarget(width, height, flags);
		}

		public static void SetRenderTarget(int id)
		{
			_SetRenderTarget(id);
		}

		public static void DestroyRenderTarget(int id)
		{
			_DestroyRenderTarget(id);
		}
	}

	internal struct ViewParams
	{
		/// <summary>
		/// view position
		/// </summary>
		public Vec3 Position;
		/// <summary>
		/// view orientation
		/// </summary>
		public Quat Rotation;
		public Quat LastLocalRotation;

		/// <summary>
		/// custom near clipping plane, 0 means use engine defaults
		/// </summary>
		public float Nearplane;
		public float FieldOfView;

		public byte ViewID;

		/// <summary>
		/// view shake status
		/// </summary>
		public bool GroundOnly;
		/// <summary>
		/// whats the amount of shake, from 0.0 to 1.0
		/// </summary>
		public float ShakingRatio;
		/// <summary>
		/// what the current angular shake
		/// </summary>
		public Quat CurrentShakeQuat;
		/// <summary>
		/// what is the current translational shake
		/// </summary>
		public Vec3 CurrentShakeShift;

		// For damping camera movement.
		/// <summary>
		/// Who we're watching. 0 == nobody.
		/// </summary>
		public uint TargetId;
		/// <summary>
		/// Where the target was.
		/// </summary>
		public Vec3 TargetPosition;
		/// <summary>
		/// current dt.
		/// </summary>
		public float FrameTime;
		/// <summary>
		/// previous rate of change of angle.
		/// </summary>
		public float AngleVelocity;
		/// <summary>
		/// previous rate of change of dist between target and camera.
		/// </summary>
		public float Velocity;
		/// <summary>
		/// previous dist of cam from target
		/// </summary>
		public float Distance;

		// blending
		public bool Blend;
		public float BlendPositionSpeed;
		public float BlendRotationSpeed;
		public float BlendFieldOfViewSpeed;
		public Vec3 BlendPositionOffset;
		public Quat BlendRotationOffset;
		public float BlendFieldOfViewOffset;
		public bool JustActivated;

		private byte LastViewID;
		private Vec3 LastPosition;
		private Quat LastRotation;
		private float LastFieldOfView;
	}
}
