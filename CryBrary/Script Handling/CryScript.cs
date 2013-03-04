using System;
using System.Collections.Generic;

using CryEngine.Extensions;

using CryEngine.FlowSystem;

namespace CryEngine.Initialization
{
    /// <summary>
    /// Represents a given class.
    /// </summary>
    public class CryScript
    {
        private CryScript(Type type, ScriptType scriptType)
        {
            Type = type;
            ScriptName = type.Name;

            ScriptType = scriptType;
        }

        public static bool TryCreate(Type type, out CryScript script)
        {
            if (type.IsAbstract || type.IsEnum)
            {
                script = null;
                return false;
            }

            var scriptType = ScriptType.Any;
            if (type.Implements<CryScriptInstance>())
            {
                scriptType |= ScriptType.CryScriptInstance;

                if (type.Implements<EntityBase>())
                {
                    scriptType |= ScriptType.Entity;

                    if (type.Implements<ActorBase>())
                        scriptType |= ScriptType.Actor;
                    else if (type.Implements<GameRules>())
                        scriptType |= ScriptType.GameRules;
                }
                else if (type.Implements<FlowNode>())
                {
                    if (type.ImplementsGeneric(typeof(EntityFlowNode<>)))
                        scriptType |= ScriptType.EntityFlowNode;

                    scriptType |= ScriptType.FlowNode;
                }
            }

            if ((scriptType & (scriptType - 1)) == 0) // only had Any set.
            {
                script = null;
                return false;
            }

            script = new CryScript(type, scriptType);
            return true;
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

        public IScriptRegistrationParams RegistrationParams { get; set; }

        #region Operators
        public static bool operator ==(CryScript script1, CryScript script2)
        {
            if (object.ReferenceEquals(script1, null) || object.ReferenceEquals(script2, null))
                return false;

            return script1.Type == script2.Type;
        }

        public static bool operator !=(CryScript script1, CryScript script2)
        {
            return !(script1 == script2);
        }

        public override bool Equals(object obj)
        {
            if (obj == null)
                return false;

            return GetHashCode() == obj.GetHashCode();
        }

        public override int GetHashCode()
        {
            unchecked // Overflow is fine, just wrap
            {
                int hash = 17;

                hash = hash * 23 + ScriptType.GetHashCode();
                hash = hash * 23 + Type.GetHashCode();

                return hash;
            }
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
        /// Scripts deriving from EntityFlowNode.
        /// </summary>
        EntityFlowNode = 64,
    }
}
