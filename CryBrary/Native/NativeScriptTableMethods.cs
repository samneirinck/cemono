using System;
using System.Runtime.CompilerServices;
using CryEngine.Lua;

namespace CryEngine.Native
{
    internal class NativeScriptTableMethods : INativeScriptTableMethods
    {
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static IntPtr _GetScriptTable(IntPtr entityPtr);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static IntPtr _GetSubScriptTable(IntPtr scriptTablePtr, string tableName);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static object _CallMethod(IntPtr scriptTablePtr, string methodName, object[] parameters);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static object _GetValue(IntPtr scriptTablePtr, string keyName);

		public IntPtr GetScriptTable(IntPtr entityPtr)
		{
			return _GetScriptTable(entityPtr);
		}

		public IntPtr GetSubScriptTable(IntPtr scriptTablePtr, string tableName)
		{
			return _GetSubScriptTable(scriptTablePtr, tableName);
		}

		public object CallMethod(IntPtr scriptTablePtr, string methodName, object[] parameters)
		{
			return _CallMethod(scriptTablePtr, methodName, parameters);
		}

		public object GetValue(IntPtr scriptTablePtr, string keyName)
		{
			return _GetValue(scriptTablePtr, keyName);
		}
    }
}
