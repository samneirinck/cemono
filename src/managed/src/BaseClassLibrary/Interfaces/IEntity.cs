namespace CryEngine
{
    public interface IEntity
    {
        long Id { get; }
        string Name { get; set; }
        EntityFlags Flags { get; set; }
        Vec3 Position { get; set; }
    }
}
