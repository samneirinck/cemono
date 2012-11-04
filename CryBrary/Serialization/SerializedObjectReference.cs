using System;
using System.Collections;
using System.Reflection;

using CryEngine.Extensions;

namespace CryEngine.Serialization
{
    class ObjectReference
    {
        public ObjectReference()
        {
        }

        public ObjectReference(string name, object value)
        {
            Name = name;
            Value = value;

            Type valueType = Value != null ? Value.GetType() : null;
            if (valueType == null || valueType.IsPointer)
                SerializationType = SerializationType.Null;
            else if (valueType == typeof(IntPtr))
                SerializationType = SerializationType.IntPtr;
            else if (valueType.IsPrimitive)
                SerializationType = SerializationType.Any;
            else if (valueType == typeof(string))
                SerializationType = SerializationType.String;
            else if (valueType.Implements<IEnumerable>())
            {
                if (valueType.IsGenericType)
                    SerializationType = SerializationType.GenericEnumerable;
                else
                    SerializationType = SerializationType.Enumerable;
            }
            else if (valueType.IsEnum)
                SerializationType = SerializationType.Enum;
            else
            {
                if (Value is Type)
                    SerializationType = SerializationType.Type;
                else if (valueType.Implements<Delegate>())
                    SerializationType = SerializationType.Delegate;
                else if (valueType.Implements<MemberInfo>())
                    SerializationType = SerializationType.MemberInfo;
                else
                    SerializationType = SerializationType.Object;
            }
        }

        public string Name { get; set; }
        public object Value { get; set; }
        public SerializationType SerializationType { get; set; }

        public bool AllowNull { get; set; }
    }
}
