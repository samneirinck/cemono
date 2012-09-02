using System;
using System.Runtime.CompilerServices;
using CryEngine.Lua;

namespace CryEngine.Native
{
    internal class NativeScriptTableMethods : INativeScriptTableMethods
    {
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static IntPtr _GetScriptTable(IntPtr entityPtr);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static object _CallMethod(IntPtr scriptTable, string methodName, LuaVariableType returnType, object[] args);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _CallMethodVoid(IntPtr scriptTable, string methodName, object[] args);

		public IntPtr GetScriptTable(IntPtr entityPtr)
		{
			return _GetScriptTable(entityPtr);
		}

        public object CallMethod(IntPtr scriptTable, string methodName, Lua.LuaVariableType returnType, object[] args)
        {
            return _CallMethod(scriptTable, methodName, returnType, args);
        }

        public void CallMethodVoid(IntPtr scriptTable, string methodName, object[] args)
        {
            _CallMethodVoid(scriptTable, methodName, args);
        }
    }
}
