using System.Runtime.CompilerServices;

namespace CryEngine.Native
{
    internal class NativeRendererMethods : INativeRendererMethods
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
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

        public int GetWidth()
        {
            return _GetWidth();
        }

        public int GetHeight()
        {
            return _GetHeight();
        }

        public Vec3 ScreenToWorld(int x, int y)
        {
            return _ScreenToWorld(x, y);
        }

        public void DrawTextToScreen(float x, float y, float fontSize, Color color, bool center, string text)
        {
            _DrawTextToScreen(x, y, fontSize, color, center, text);
        }

        public int LoadTexture(string path)
        {
            return _LoadTexture(path);
        }

        public void DrawTextureToScreen(float xpos, float ypos, float width, float height, int textureId, float s0 = 0, float t0 = 0, float s1 = 1, float t1 = 1, float angle = 0, float r = 1, float g = 1, float b = 1, float a = 1, float z = 1)
        {
            _DrawTextureToScreen(xpos, ypos, width, height, textureId, s0, t0, s1, t1, angle, r, g, b, a, z);
        }

        public int CreateRenderTarget(int width, int height, int flags)
        {
            return _CreateRenderTarget(width, height, flags);
        }

        public void DestroyRenderTarget(int id)
        {
            _DestroyRenderTarget(id);
        }

        public void SetRenderTarget(int id)
        {
            _SetRenderTarget(id);
        }
    }
}
