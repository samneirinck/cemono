using System;

namespace CryEngine
{
    [AttributeUsage(AttributeTargets.All, Inherited = false, AllowMultiple = true)]
    public class PropertyAttribute : Attribute
    {
        public PropertyAttribute()
            : base()
        {
            Name = "";
            Type = PropertyTypes.None;
            EditorType = EditorTypes.None;
            Description = "No description";
            Flags = 0;
            MinValue = Single.MinValue;
            MaxValue = Single.MaxValue;
        }

        public string Name { get; set; }
        public PropertyTypes Type { get; set; }
        public EditorTypes EditorType { get; set; }
        public string Description { get; set; }
        public int Flags { get; set; }
        public float MinValue { get; set; }
        public float MaxValue { get; set; }
    }
}
