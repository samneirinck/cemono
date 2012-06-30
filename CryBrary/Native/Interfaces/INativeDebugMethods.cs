namespace CryEngine.Native
{
    internal interface INativeDebugMethods
    {
       void AddPersistentSphere(Vec3 pos, float radius, Color color, float timeout);

       void AddDirection(Vec3 pos, float radius, Vec3 dir, Color color, float timeout);

       void AddPersistentText2D(string text, float size, Color color, float timeout);

       void AddPersistentLine(Vec3 pos, Vec3 end, Color color, float timeout);

    }
}