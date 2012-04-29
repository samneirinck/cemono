using System;
using System.Collections.Generic;

namespace CryEngine.Initialization
{
	/// <summary>
	/// Represents a given class.
	/// </summary>
	public struct CryScript
	{
		public CryScript(Type type)
			: this()
		{
			ScriptType = type;
			ScriptName = type.Name;
		}

		public Type ScriptType { get; private set; }
		/// <summary>
		/// The script's name, not always type name!
		/// </summary>
		public string ScriptName { get; set; }

		/// <summary>
		/// Stores all instances of this class.
		/// </summary>
		public List<CryScriptInstance> ScriptInstances { get; internal set; }

		#region Operators
		public static bool operator ==(CryScript script1, CryScript script2)
		{
			return script1.ScriptType == script2.ScriptType;
		}

		public static bool operator !=(CryScript script1, CryScript script2)
		{
			return script1.ScriptType != script2.ScriptType;
		}

		public override bool Equals(object obj)
		{
			if(obj is CryScript)
				return (CryScript)obj == this;

			return false;
		}

		public override int GetHashCode()
		{
			return ScriptType.GetHashCode();
		}

		#endregion
	}

	public enum ScriptType
	{
		/// <summary>
		/// Scripts will be linked to this type if they inherit from CryScriptInstance, but not any other script base.
		/// </summary>
		Unknown = 0,
		/// <summary>
		/// Scripts directly inheriting from BaseGameRules will utilize this script type.
		/// </summary>
		GameRules,
		/// <summary>
		/// Scripts directly inheriting from FlowNode will utilize this script type.
		/// </summary>
		FlowNode,
		/// <summary>
		/// Scripts directly inheriting from Entity will utilize this script type.
		/// </summary>
		Entity,
		/// <summary>
		/// Scripts directly inheriting from Actor will utilize this script type.
		/// </summary>
		Actor,
		/// <summary>
		/// </summary>
		UIEvent,
		/// <summary>
		/// </summary>
		EditorForm,
	}
}
