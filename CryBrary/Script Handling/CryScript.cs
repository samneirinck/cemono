using System;
using System.Collections.Generic;

using CryEngine.Extensions;

namespace CryEngine.Initialization
{
	/// <summary>
	/// Represents a given class.
	/// </summary>
	public struct CryScript
	{
		internal CryScript(Type type)
			: this()
		{
			Type = type;
			ScriptName = type.Name;

			if(type.Implements(typeof(Actor)))
				ScriptType = ScriptType.Actor;
			else if(type.Implements(typeof(Entity)))
				ScriptType = ScriptType.Entity;
			else if(type.Implements(typeof(FlowNode)))
				ScriptType = ScriptType.FlowNode;
			else if(type.Implements(typeof(GameRules)))
				ScriptType = ScriptType.GameRules;
			else if(type.Implements(typeof(UIEventSystem)))
				ScriptType = ScriptType.UIEventSystem;
			else if(type.Implements(typeof(ScriptCompiler)))
				ScriptType = ScriptType.ScriptCompiler;
			else if(type.Implements(typeof(CryScriptInstance)))
				ScriptType = ScriptType.CryScriptInstance;
			else
				ScriptType = ScriptType.Unknown;
		}

		public ScriptType ScriptType { get; private set; }

		public Type Type { get; private set; }
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
			return script1.Type == script2.Type;
		}

		public static bool operator !=(CryScript script1, CryScript script2)
		{
			return !(script1 == script2);
		}

		public override bool Equals(object obj)
		{
			if(obj is CryScript)
				return (CryScript)obj == this;

			return false;
		}

		public override int GetHashCode()
		{
			return Type.GetHashCode();
		}

		#endregion
	}

	public enum ScriptType
	{
		/// <summary>
		/// Scripts not inheriting from CryScriptInstance will utilize this script type.
		/// </summary>
		Unknown = -1,
		/// <summary>
		/// Scripts inheriting from CryScriptInstance, but no other CryMono base script will be linked to this script type.
		/// </summary>
		CryScriptInstance,
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
		UIEventSystem,
		/// <summary>
		/// </summary>
		EditorForm,
		ScriptCompiler,
	}
}
