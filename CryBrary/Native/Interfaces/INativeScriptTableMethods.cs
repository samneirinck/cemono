using System;
using CryEngine.Lua;

namespace CryEngine.Native
{
    internal interface INativeScriptTableMethods
    {
		IntPtr GetScriptTable(IntPtr entityPtr);
		IntPtr GetSubScriptTable(IntPtr scriptTablePtr, string tableName);

		object CallMethod(IntPtr scriptTablePtr, string methodName, object[] parameters);
		object GetValue(IntPtr scriptTablePtr, string keyName);

		bool ExecuteBuffer(string buffer);
    }
}