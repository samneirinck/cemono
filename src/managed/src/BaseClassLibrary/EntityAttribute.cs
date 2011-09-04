using System;

namespace CryEngine
{
    [AttributeUsage(AttributeTargets.Class)]
    public class EntityAttribute : Attribute
    {
        public EntityAttribute()
            : base()
        {
            Name = "Unknown";
            Category = "/";
            Flags = EntityClassFlags.ECLF_DEFAULT;
            EditorHelperObjectName = "";
            EditorIconName = "";
        }

        public string Name { get; set; }
        private string _category;

        public string Category
        {
            get { return _category; }
            set
            {
                _category = value;
                if (!_category.EndsWith("/"))
                    _category = _category + "/";
            }
        }

        public EntityClassFlags Flags { get; set; }
        public string EditorHelperObjectName { get; set; }
        public string EditorIconName { get; set; }
    }
}
