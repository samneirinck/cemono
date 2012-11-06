using System;
using CryEngine.Lua;

namespace CryEngine.Native
{
    public static class NativeScriptTableExtensions
    {
        public static IntPtr GetHandle(this ScriptTable scriptTable)
        {
            return scriptTable.Handle;
        }

        public static void SetActorHandle(this ScriptTable scriptTable, IntPtr handle)
        {
            scriptTable.Handle = handle;
        }
    }
}
