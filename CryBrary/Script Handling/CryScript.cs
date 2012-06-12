using System;
using System.Collections.Generic;

using CryEngine.Extensions;
using CryEngine.Initialization;

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

			ScriptType |= ScriptType.Any;
			if(type.Implements(typeof(CryScriptInstance)))
			{
				ScriptType |= ScriptType.CryScriptInstance;

				if(type.Implements(typeof(EntityBase)))
				{
					ScriptType |= ScriptType.Entity;

					if(type.Implements(typeof(Actor)))
						ScriptType |= ScriptType.Actor;
				}
				if(type.Implements(typeof(FlowNode)))
					ScriptType |= ScriptType.FlowNode;
				else if(type.Implements(typeof(GameRules)))
					ScriptType |= ScriptType.GameRules;
			}
			else if(type.Implements(typeof(UIEventSystem)))
				ScriptType |= ScriptType.UIEventSystem;
			else if(type.Implements(typeof(ScriptCompiler)))
				ScriptType |= ScriptType.ScriptCompiler;
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

	[Flags]
	public enum ScriptType
	{
		/// <summary>
		/// All scripts have this flag applied.
		/// </summary>
		Any = 1,
		/// <summary>
		/// Scripts deriving from CryScriptInstance.
		/// </summary>
		CryScriptInstance = 2,
		/// <summary>
		/// Scripts deriving from GameRules.
		/// </summary>
		GameRules = 4,
		/// <summary>
		/// Scripts deriving from FlowNode.
		/// </summary>
		FlowNode = 8,
		/// <summary>
		/// Scripts deriving from EntityBase.
		/// </summary>
		Entity = 16,
		/// <summary>
		/// Scripts deriving from Actor.
		/// </summary>
		Actor = 32,
		/// <summary>
		/// </summary>
		UIEventSystem = 64,
		ScriptCompiler = 128,
	}
}
