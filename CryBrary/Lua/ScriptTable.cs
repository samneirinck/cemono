using System;
using System.Collections.Generic;
using System.Linq;

using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using CryEngine.Native;

namespace CryEngine.Lua
{
	/// <summary>
	/// Represents a Lua ScriptTable.
	/// </summary>
	public class ScriptTable
	{
		#region Statics
		internal static ScriptTable Get(IntPtr entityPtr)
		{
			if (ScriptTables == null)
				ScriptTables = new List<ScriptTable>();

			var scriptPtr = NativeMethods.ScriptTable.GetScriptTable(entityPtr);
			if (scriptPtr != IntPtr.Zero)
			{
				var scriptTable = ScriptTables.FirstOrDefault(x => x.HandleRef.Handle == scriptPtr);
				if (scriptTable != default(ScriptTable))
					return scriptTable;

				scriptTable = new ScriptTable(scriptPtr);
				ScriptTables.Add(scriptTable);
				return scriptTable;
			}

			return null;
		}

		static List<ScriptTable> ScriptTables { get; set; }
		#endregion

		internal ScriptTable(IntPtr scriptPtr)
		{
			HandleRef = new HandleRef(this, scriptPtr);
		}

		/// <summary>
		/// Invokes a method on the script table
		/// </summary>
		/// <param name="methodName">Name of the method</param>
		/// <param name="args">Invocation arguments</param>
		/// <returns>Result or null</returns>
		public object CallMethod(string methodName, params object[] args)
		{
			return NativeMethods.ScriptTable.CallMethod(HandleRef.Handle, methodName, args);
		}

		/// <summary>
		/// Gets a value within the table.
		/// </summary>
		/// <param name="name">Name of the value</param>
		/// <returns>The value or null</returns>
		public object GetValue(string name)
		{
			return NativeMethods.ScriptTable.GetValue(HandleRef.Handle, name);
		}

		/// <summary>
		/// Gets a table within this table.
		/// </summary>
		/// <param name="name">Name of the table</param>
		/// <returns></returns>
		public ScriptTable GetTable(string name)
		{
			var scriptPtr = NativeMethods.ScriptTable.GetSubScriptTable(HandleRef.Handle, name);
			if (scriptPtr != IntPtr.Zero)
			{
				var scriptTable = ScriptTables.FirstOrDefault(x => x.HandleRef.Handle == scriptPtr);
				if (scriptTable != default(ScriptTable))
					return scriptTable;

				scriptTable = new ScriptTable(scriptPtr);
				ScriptTables.Add(scriptTable);
				return scriptTable;
			}

			return null;
		}

		/// <summary>
		/// Handle to the native IScriptTable object
		/// </summary>
		internal HandleRef HandleRef { get; set; }
	}

	enum LuaVariableType
	{
		Boolean,
		Integer,
		Float,
		String,

		None
	}
}
