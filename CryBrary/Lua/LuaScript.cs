using System.Collections.Generic;
using System.Linq;

using System.Runtime.CompilerServices;

namespace CryEngine.Lua
{
	/// <summary>
	/// Represents a Lua ScriptTable.
	/// </summary>
	public class ScriptTable
	{
		#region Externals
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _GetScriptTable(EntityId entityId);
		#endregion

		#region Statics
		public static ScriptTable Get(EntityId entityId)
		{
			if(!scripts.ContainsKey(entityId))
			{
				int scriptTableId = _GetScriptTable(entityId);
				if(scriptTableId == -1)
					return null;

				scripts.Add(entityId, new ScriptTable(scriptTableId));
			}

			return scripts[entityId];
		}

		public static Dictionary<EntityId, ScriptTable> scripts = new Dictionary<EntityId, ScriptTable>();
		#endregion

		internal ScriptTable(int scriptTableId)
		{
			Id = scriptTableId;
		}

		public int Id { get; set; }
	}
}
