using System.Reflection;

namespace CryEngine.Initialization
{
	/// <summary>
	/// Represents a custom script compiler.
	/// </summary>
	public interface IScriptCompiler
	{
		/// <summary>
		/// Creates the assembly to be loaded in turn by the main compiler.
		/// </summary>
		/// <returns>The compiled assembly.</returns>
		Assembly Compile();
	}
}
