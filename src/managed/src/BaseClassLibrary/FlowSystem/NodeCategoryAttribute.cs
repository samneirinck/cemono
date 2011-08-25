using System;

namespace CryEngine.FlowSystem
{
    [AttributeUsage(AttributeTargets.Class)]
    public class NodeCategoryAttribute : System.Attribute
    {
        public NodeCategoryAttribute()
            : base()
        {

        }
        public NodeCategoryAttribute(string category)
            : this()
        {
            this.Category = category;
        }

        public string Category { get; set; }
    }
}
