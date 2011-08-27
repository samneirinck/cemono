namespace CryEngine
{
    public class EntityProperty
    {
        public EntityProperty()
        {
            Name = "UnknownProperty";
            Type = 0;
            EditorType = "";
            Description = "No description";
            Flags = 0;
            MinValue = 0;
            MaxValue = 0;
        }

        public string Name { get; set; }
        public int Type { get; set; }
        public string EditorType { get; set; }
        public string Description { get; set; }
        public int Flags { get; set; }
        public float MinValue { get; set; }
        public float MaxValue { get; set; }
    }
}
