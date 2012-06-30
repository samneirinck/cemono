using System;
using System.Collections.Generic;
using System.Linq;

using System.Runtime.CompilerServices;
using CryEngine.Native;

namespace CryEngine.Lua
{
	/// <summary>
	/// Represents a Lua ScriptTable.
	/// </summary>
	public class ScriptTable
	{
		#region Statics
		public static ScriptTable Get(EntityId entityId)
		{
			var scriptTable = ScriptTables.Find(x => x.EntityId == entityId);
			if(scriptTable != default(ScriptTable))
				return scriptTable;

            var scriptPtr = NativeMethods.ScriptTable.GetScriptTable(entityId);
			if(scriptPtr != IntPtr.Zero)
			{
				ScriptTables.Add(new ScriptTable(scriptPtr));
				return ScriptTables.Last();
			}

			return null;
		}

		static List<ScriptTable> ScriptTables { get; set; }
		#endregion

		internal ScriptTable(IntPtr scriptPtr)
		{
			ScriptPointer = scriptPtr;

			IsSubtable = false;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <typeparam name="T">Boolean, Integer, Float or String.</typeparam>
		/// <param name="methodName"></param>
		/// <param name="args"></param>
		/// <returns></returns>
		public T CallMethod<T>(string methodName, object[] args = null)
		{
			Type retType = typeof(T);
			LuaVariableType variableType = LuaVariableType.None;

			if(retType.IsPrimitive)
			{
				if(retType == typeof(bool))
					variableType = LuaVariableType.Boolean;
				else if(retType == typeof(int))
					variableType = LuaVariableType.Integer;
				else if(retType == typeof(float))
					variableType = LuaVariableType.Float;
			}
			else if(retType == typeof(string))
				variableType = LuaVariableType.String;
			else
				throw new NotSupportedException("Lua methods can only return Boolean, Integer, Float, Vector or String.");

            return (T)NativeMethods.ScriptTable.CallMethod(ScriptPointer, methodName, variableType, args);
		}

		public void CallMethod(string methodName, object[] args = null)
		{
            NativeMethods.ScriptTable.CallMethodVoid(ScriptPointer, methodName, args);
		}

		/// <summary>
		/// Gets a value within the table.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="name"></param>
		/// <returns></returns>
		public T GetValue<T>(string name)
		{
			return default(T);
		}

		/// <summary>
		/// Gets a table within this table.
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		public ScriptTable GetTable(string name)
		{
			return null;
		}

		public EntityId EntityId { get; set; }

		/// <summary>
		/// Determines if this is a SmartScriptTable, retrieved from a ScriptTable.
		/// </summary>
		internal bool IsSubtable { get; set; }
		internal IntPtr ScriptPointer { get; set; }
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
