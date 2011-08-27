using System;

namespace CryEngine
{
    [AttributeUsage(AttributeTargets.All, Inherited = false, AllowMultiple = true)]
    public class PropertyAttribute : Attribute
    {
        public PropertyAttribute()
            : base()
        {
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
