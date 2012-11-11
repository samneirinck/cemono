using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine.Serialization
{
    enum SerializationType
    {
        Null,
        Reference,
        Object,
        GenericEnumerable,
        Enumerable,
        Enum,
        Any,
        String,
        MemberInfo,
        Type,
        Delegate,
        IntPtr,
        UnusedMarker
    }
}
