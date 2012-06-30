using System.Runtime.CompilerServices;
using CryEngine.Native;

namespace CryEngine
{
	public static class Renderer
	{
        private static INativeRendererMethods _nativeRendererMethods;
        internal static INativeRendererMethods NativeRendererMethods
        {
            get { return _nativeRendererMethods ?? (_nativeRendererMethods = new NativeRendererMethods()); }
            set { _nativeRendererMethods = value; }
        }

		public static Vec3 ScreenToWorld(int x, int y)
		{
			return NativeRendererMethods.ScreenToWorld(x,y);
		}

		public static void DrawTextToScreen(float x, float y, float fontSize, Color color, string text, params object[] args)
		{
			DrawTextToScreen(x, y, fontSize, color, false, text, args);
		}

		public static void DrawTextToScreen(float x, float y, float fontSize, Color color, bool centered, string text, params object[] args)
		{
			NativeRendererMethods.DrawTextToScreen(x, y, fontSize, color, centered, string.Format(text, args));
		}

		/// <summary>
		/// The width of the screen in pixels.
		/// </summary>
		public static int ScreenHeight { get { return NativeRendererMethods.GetHeight(); } }

		/// <summary>
		/// The height of the screen in pixels.
		/// </summary>
		public static int ScreenWidth { get { return NativeRendererMethods.GetWidth(); } }

		/// <summary>
		/// Loads a texture into memory and generates its ID.
		/// </summary>
		/// <param name="texturePath">The path to the texture.</param>
		/// <returns></returns>
		public static int LoadTexture(string texturePath)
		{
			return NativeRendererMethods.LoadTexture(texturePath);
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
			NativeRendererMethods.DrawTextureToScreen(xPos, yPos, width, height, textureId);
		}

		public static int CreateRenderTarget(int width, int height, int flags = 0)
		{
			return NativeRendererMethods.CreateRenderTarget(width, height, flags);
		}

		public static void SetRenderTarget(int id)
		{
			NativeRendererMethods.SetRenderTarget(id);
		}

		public static void DestroyRenderTarget(int id)
		{
			NativeRendererMethods.DestroyRenderTarget(id);
		}
	}

	
}
