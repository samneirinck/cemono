using System;

namespace CryEngine
{
    [AttributeUsage(AttributeTargets.Class)]
    public class EntityAttribute : Attribute
    {
        public EntityAttribute()
            : base()
        {

        }

        public string Name { get; set; }
        public string Category { get; set; }
    }
}
