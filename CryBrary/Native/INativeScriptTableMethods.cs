using System;
using CryEngine.Lua;

namespace CryEngine.Native
{
    internal interface INativeScriptTableMethods
    {
        IntPtr GetScriptTable(EntityId entityId);

        object CallMethod(IntPtr scriptTable, string methodName, LuaVariableType returnType, object[] args);
        void CallMethodVoid(IntPtr scriptTable, string methodName, object[] args);
    }
}