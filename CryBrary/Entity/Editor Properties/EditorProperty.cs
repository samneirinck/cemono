using System;


namespace CryEngine
{
    public struct EditorProperty
    {
        public EditorProperty(string Name, string Desc, EditorPropertyType Type, EditorPropertyLimits Limits, int Flags = 0)
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

        public EditorProperty(string Name, string Desc, EditorPropertyType Type)
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

        private EditorPropertyType _type;

        public EditorPropertyType type
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
                    case EditorPropertyType.Bool:
                        {
                            editType = "b";
                        }
                        break;

                    case EditorPropertyType.Int:
                        {
                            editType = "i";
                        }
                        break;

                    case EditorPropertyType.Float:
                        {
                            editType = "f";
                        }
                        break;

                    // FILE SELECTORS
                    case EditorPropertyType.File:
                        {
                            editType = "file";
                            _type = EditorPropertyType.String;
                        }
                        break;

                    case EditorPropertyType.Object:
                        {
                            editType = "object";
                            _type = EditorPropertyType.String;
                        }
                        break;

                    case EditorPropertyType.Texture:
                        {
                            editType = "texture";
                            _type = EditorPropertyType.String;
                        }
                        break;

                    case EditorPropertyType.Sound:
                        {
                            editType = "sound";
                            _type = EditorPropertyType.String;
                        }
                        break;

                    case EditorPropertyType.Dialogue:
                        {
                            editType = "dialog";
                            _type = EditorPropertyType.String;
                        }
                        break;

                    // VECTORS
                    case EditorPropertyType.Color:
                        {
                            editType = "color";
                            _type = EditorPropertyType.Vec3;
                        }
                        break;

                    case EditorPropertyType.Vec3:
                        {
                            editType = "vector";
                        }
                        break;

                    // MISC
                    case EditorPropertyType.Sequence:
                        {
                            editType = "_seq";
                            _type = EditorPropertyType.String;
                        }
                        break;

                }
            }
        }

        public int flags;

        public EditorPropertyLimits limits;

    }
}