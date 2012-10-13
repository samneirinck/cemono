using System.Runtime.InteropServices;
using CryEngine.Lua;

namespace CryEngine.Native
{
	public static class NativeScriptTableExtensions
	{
		public static HandleRef GetHandle(this ScriptTable scriptTable)
		{
			return scriptTable.HandleRef;
		}

		public static void SetActorHandle(this ScriptTable scriptTable, HandleRef handleRef)
		{
			scriptTable.HandleRef = handleRef;
		}
	}
}
