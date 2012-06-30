namespace CryEngine.Native
{
    internal interface INativeRendererMethods
    {
        int GetWidth();
        int GetHeight();
        Vec3 ScreenToWorld(int x, int y);
        void DrawTextToScreen(float x, float y, float fontSize, Color color, bool center, string text);
        int LoadTexture(string path);
        void DrawTextureToScreen(float xpos, float ypos, float width, float height, int textureId, float s0 = 0, float t0 = 0, float s1 = 1, float t1 = 1, float angle = 0, float r = 1, float g = 1, float b = 1, float a = 1, float z = 1);
        int CreateRenderTarget(int width, int height, int flags);
        void DestroyRenderTarget(int id);
        void SetRenderTarget(int id);
    }
}