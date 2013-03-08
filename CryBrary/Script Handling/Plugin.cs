using System;
using System.CodeDom.Compiler;
using System.Reflection;
using System.Collections.Generic;

namespace CryEngine.Initialization
{
    /// <summary>
    /// Represents a CryMono plugin.
    /// </summary>
    public interface ICryMonoPlugin
    {
        IEnumerable<Type> GetTypes(IEnumerable<Assembly> assemblies);

        IScriptRegistrationParams GetRegistrationParams(ScriptType scriptType, Type type);
    }

    [Serializable]
    public class ScriptCompilationException : Exception
    {
        public ScriptCompilationException() { }

        public ScriptCompilationException(string errorMessage)
            : base(errorMessage) { }

        public ScriptCompilationException(string errorMessage, Exception innerEx)
            : base(errorMessage, innerEx) { }
    }
}
