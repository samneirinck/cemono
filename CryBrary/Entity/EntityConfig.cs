using System;


namespace CryEngine
{
    /// <summary>
    /// Defines additional information used by the entity registration system.
    /// </summary>
    [AttributeUsage(AttributeTargets.Class)]
    public sealed class EntityAttribute : Attribute
    {
        public EntityAttribute()
        {
            Flags = EntityClassFlags.Default;
        }

        /// <summary>
        /// Gets or sets the Entity class name. Uses class name if not set.
        /// </summary>
        public string Name { get; set; }

        /// <summary>
        /// Gets or sets the helper mesh displayed inside Sandbox.
        /// </summary>
        public string EditorHelper { get; set; }

        /// <summary>
        /// Gets or sets the class flags for this entity.
        /// </summary>
        public EntityClassFlags Flags { get; set; }

        /// <summary>
        /// Gets or sets the category in which the entity will be placed.
        /// </summary>
        public string Category { get; set; }

        /// <summary>
        /// Gets or sets the helper graphic displayed inside Sandbox.
        /// </summary>
        public string Icon { get; set; }
    }

    /// <summary>
    /// Defines a property that is displayed and editable inside Sandbox.
    /// </summary>
    [AttributeUsage(AttributeTargets.Property | AttributeTargets.Field)]
    public sealed class EditorPropertyAttribute : Attribute
    {
        /// <summary>
        /// Gets or sets the minimum value
        /// </summary>
        public float Min { get; set; }

        /// <summary>
        /// Gets or sets the maximum value
        /// </summary>
        public float Max { get; set; }

        /// <summary>
        /// Gets or sets the default value
        /// </summary>
        public object DefaultValue { get; set; }

        /// <summary>
        /// Gets or sets the property type.
        /// Should be used for special types such as files.
        /// </summary>
        public EntityPropertyType Type { get; set; }

        public int Flags { get; set; }

        /// <summary>
        /// Gets or sets the name of the property, if not set the entity class name will be used.
        /// </summary>
        public string Name { get; set; }

        /// <summary>
        /// Gets or sets the description to display when the user hovers over this property inside Sandbox.
        /// </summary>
        public string Description { get; set; }

        /// <summary>
        /// Folder in which the entity property resides.
        /// If null, not contained in a folder.
        /// </summary>
        public string Folder { get; set; }
    }

    /// <summary>
    /// Defines the list of supported editor types.
    /// </summary>
    public enum EntityPropertyType
    {
        Bool,
        Int,
        Float,
        Vec3,
        String,
        Entity,
        Object,
        Texture,
        File,
        Sound,
        Dialogue,
        Color,
        Sequence
    }

    public struct EntityPropertyLimits
    {
        public EntityPropertyLimits(float min, float max)
            : this()
        {
            this.min = min;
            this.max = max;
        }

        public float min;
        public float max;
    }

    public struct EntityPropertyFolder
    {
        public string name;
        public object[] properties;
    }

    public struct EntityProperty
    {
        public EntityProperty(string Name, string Desc, EntityPropertyType Type, EntityPropertyLimits Limits, int Flags = 0)
            : this(Name, Desc, Type)
        {
            if (Limits.max == 0 && Limits.min == 0)
            {
                limits.max = Sandbox.UIConstants.MAX_SLIDER_VALUE;
            }
            else
            {
                limits.max = Limits.max;
                limits.min = Limits.min;
            }

            flags = Flags;
        }

        public EntityProperty(string Name, string Desc, EntityPropertyType Type)
            : this()
        {
            name = Name;
            description = Desc;

            type = Type;
        }

        public string name;

        public string description;

#pragma warning disable 414
        private string editType;
#pragma warning restore 414

        private EntityPropertyType _type;

        public EntityPropertyType type
        {
            get
            {
                return _type;
            }

            set
            {
                _type = value;

                switch (value)
                {
                    // VALUE TYPES
                    case EntityPropertyType.Bool:
                        {
                            editType = "b";
                        }
                        break;

                    case EntityPropertyType.Int:
                        {
                            editType = "i";
                        }
                        break;

                    case EntityPropertyType.Float:
                        {
                            editType = "f";
                        }
                        break;

                    // FILE SELECTORS
                    case EntityPropertyType.File:
                        {
                            editType = "file";
                            _type = EntityPropertyType.String;
                        }
                        break;

                    case EntityPropertyType.Object:
                        {
                            editType = "object";
                            _type = EntityPropertyType.String;
                        }
                        break;

                    case EntityPropertyType.Texture:
                        {
                            editType = "texture";
                            _type = EntityPropertyType.String;
                        }
                        break;

                    case EntityPropertyType.Sound:
                        {
                            editType = "sound";
                            _type = EntityPropertyType.String;
                        }
                        break;

                    case EntityPropertyType.Dialogue:
                        {
                            editType = "dialog";
                            _type = EntityPropertyType.String;
                        }
                        break;

                    // VECTORS
                    case EntityPropertyType.Color:
                        {
                            editType = "color";
                            _type = EntityPropertyType.Vec3;
                        }
                        break;

                    case EntityPropertyType.Vec3:
                        {
                            editType = "vector";
                        }
                        break;

                    // MISC
                    case EntityPropertyType.Sequence:
                        {
                            editType = "_seq";
                            _type = EntityPropertyType.String;
                        }
                        break;

                }
            }
        }

        public int flags;

        public EntityPropertyLimits limits;
    }
}