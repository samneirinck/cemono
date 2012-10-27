using System;
using System.CodeDom.Compiler;
using System.Reflection;
using System.Collections.Generic;

namespace CryEngine.Initialization
{
    /// <summary>
    /// Represents a custom script compiler.
    /// </summary>
    public abstract class ScriptCompiler
    {
        #region Statics
        /// <summary>
        /// Validates that a compilation has been successful.
        /// </summary>
        /// <param name="results">The results of the compilation that you wish to validate</param>
        /// <returns>The resulting assembly, if no errors are found.</returns>
        public static Assembly ValidateCompilation(CompilerResults results)
        {
            if (!results.Errors.HasErrors && results.CompiledAssembly != null)
                return results.CompiledAssembly;

            string compilationError = string.Format("Compilation failed; {0} errors: ", results.Errors.Count);

            foreach (CompilerError error in results.Errors)
            {
                compilationError += Environment.NewLine;

                if (!error.ErrorText.Contains("(Location of the symbol related to previous error)"))
                    compilationError += string.Format("{0}({1},{2}): {3} {4}: {5}", error.FileName, error.Line, error.Column, error.IsWarning ? "warning" : "error", error.ErrorNumber, error.ErrorText);
                else
                    compilationError += "    " + error.ErrorText;
            }

            throw new ScriptCompilationException(compilationError);
        }
        #endregion

        public abstract IEnumerable<CryScript> Process(IEnumerable<Assembly> assemblies);
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
