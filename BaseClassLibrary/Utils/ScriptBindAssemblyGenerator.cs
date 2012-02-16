using System.IO;
using System.Linq;
using System.Collections.Generic;

namespace CryEngine.Utils
{
	/// <summary>
	/// Generates an assembly containing the scriptbind externals passed to it.
	/// </summary>
	public static class ScriptBindAssemblyGenerator
	{
		public static void GenerateScriptbindAssembly(Scriptbind[] scriptBinds)
		{
			List<string> sourceCode = new List<string>();
			sourceCode.Add("using System.Runtime.CompilerServices;");

			foreach (var scriptBind in scriptBinds)
			{
				sourceCode.Add(string.Format("namespace {0}", scriptBind.namespaceName) + "{");

				sourceCode.Add(string.Format("    public partial class {0}", scriptBind.className) + "    {");

				foreach (InternalCallMethod method in scriptBind.methods)
				{
					string parameters = method.parameters;
					string returnType = method.returnType;

					ConvertToCSharp(ref returnType);

					// Convert C++ types to C# ones
					string fixedParams = "";
					string[] splitParams = parameters.Split(',');
					for (int i = 0; i < splitParams.Length; i++)
					{
						string param = splitParams[i];
						ConvertToCSharp(ref param);
						fixedParams += param;
						if (param.Last() != ' ')
							fixedParams += ' ';

						string varName = param;

						if (varName.First() == ' ')
							varName = varName.Remove(0, 1);
						if (varName.Last() == ' ')
							varName = varName.Remove(varName.Count() - 1, 1);

						varName = varName.Replace("ref ", "").Replace("[]", "");

						varName += i.ToString();

						fixedParams += varName;
						fixedParams += ",";
					}
					// Remove the extra ','.
					fixedParams = fixedParams.Remove(fixedParams.Count() - 1);

					sourceCode.Add("        [MethodImplAttribute(MethodImplOptions.InternalCall)]");
					sourceCode.Add("        extern public static " + returnType + " " + method.name + "(" + fixedParams + ");");
				}

				sourceCode.Add("    }");

				sourceCode.Add("}");
			}

			string generatedFile = Path.Combine(PathUtils.GetScriptsFolder(), "GeneratedScriptbinds.cs");
			File.WriteAllLines(generatedFile, sourceCode);

			/*
			CodeDomProvider provider = new CSharpCodeProvider();
			CompilerParameters compilerParameters = new CompilerParameters();

			compilerParameters.OutputAssembly = Path.Combine(CryPath.GetScriptsFolder(), "Plugins", "CryScriptbinds.dll");

			compilerParameters.CompilerOptions = "/target:library /optimize";
			compilerParameters.GenerateExecutable = false;
			compilerParameters.GenerateInMemory = false;

#if DEBUG
			compilerParameters.IncludeDebugInformation = true;
#else
			compilerParameters.IncludeDebugInformation = false;
#endif

			var assemblies = AppDomain.CurrentDomain.GetAssemblies().Select(a => a.Location);
			foreach (var assemblyPath in assemblies)
				compilerParameters.ReferencedAssemblies.Add(assemblyPath);

			try
			{
				CompilerResults results = provider.CompileAssemblyFromSource(compilerParameters, sourceCode.ToArray());
				if (results.Errors.HasErrors)
				{
					CryConsole.LogAlways("CryScriptBinds.dll compilation failed; {0} errors:", results.Errors.Count);

					foreach (CompilerError error in results.Errors)
						CryConsole.LogAlways(error.ErrorText);
				}
			}
			catch (Exception ex)
			{
				CryConsole.LogException(ex);
			}*/
		}

		/// <summary>
		/// Finds C++-specific types in the provided string and substitutes them for C# types.
		/// </summary>
		/// <param name="cplusplusTypes"></param>
		private static void ConvertToCSharp(ref string cplusplusTypes)
		{
			cplusplusTypes = cplusplusTypes.Replace("mono::string", "string");
			cplusplusTypes = cplusplusTypes.Replace("mono::array", "object[]");
			cplusplusTypes = cplusplusTypes.Replace("MonoObject *", "object");
			cplusplusTypes = cplusplusTypes.Replace("EntityId", "uint");

			cplusplusTypes = cplusplusTypes.Replace(" &", "&");
			if (cplusplusTypes.EndsWith("&"))
			{
				cplusplusTypes = cplusplusTypes.Replace("&", "");

				cplusplusTypes = cplusplusTypes.Insert(0, "ref ");
				// Remove annoying extra space.
				if (cplusplusTypes.ElementAt(4) == ' ')
					cplusplusTypes = cplusplusTypes.Remove(4, 1);
			}

			// Fugly workaround; Replace types not known to this assembly with 'object'.
			// TODO: Generate <summary> stuff and add the original type to the description?
			/*if (!cplusplusTypes.Contains("int") && !cplusplusTypes.Contains("string")
				&& !cplusplusTypes.Contains("float") && !cplusplusTypes.Contains("uint")
				&& !cplusplusTypes.Contains("object") && !cplusplusTypes.Contains("bool")
				&& !cplusplusTypes.Contains("Vec3"))
			{
				if (cplusplusTypes.Contains("ref"))
					cplusplusTypes = "ref object";
				else
					cplusplusTypes = "object";
			}*/
		}
	}
}
