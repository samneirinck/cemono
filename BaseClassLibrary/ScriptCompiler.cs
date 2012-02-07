using System;
using System.Linq;
using System.IO;
using System.Collections.Generic;

using System.CodeDom.Compiler;
using Microsoft.CSharp;
using System.Reflection;

using CryEngine.Extensions;

using System.Xml.Linq;
using System.ComponentModel;
using System.Threading.Tasks;

/// <summary>
/// The main engine namespace, otherwise known as the CryENGINE3 Base Class Library.
/// </summary>
namespace CryEngine
{
	/// <summary>
	/// The script compiler is responsible for all CryScript compilation.
	/// </summary>
	public class ScriptCompiler : _ScriptCompiler
	{
		public ScriptCompiler()
		{
#if !RELEASE
			Pdb2Mdb.Driver.Convert(Assembly.GetExecutingAssembly());
#endif

			m_compiledScripts = new List<CryScript>();
			m_flowNodes = new List<StoredNode>();
			referencedAssemblies = new List<string>();

			m_numInstances = 0;
		}

		public void Initialize()
		{
			//GenerateScriptbindAssembly(scriptBinds.ToArray());

			referencedAssemblies.AddRange(AppDomain.CurrentDomain.GetAssemblies().Select(a => a.Location).ToArray());

			Reload();
		}

		public void PostInit()
		{
			// These have to be registered later on due to the flow system being initialized late.
			RegisterFlownodes();
		}

		public void Reload()
		{
			LoadPrecompiledAssemblies();

			List<string> scriptFolders = new List<string>();
			scriptFolders.Add(PathUtils.GetScriptFolder(MonoScriptType.Entity));
			scriptFolders.Add(PathUtils.GetScriptFolder(MonoScriptType.GameRules));
			scriptFolders.Add(PathUtils.GetScriptFolder(MonoScriptType.FlowNode));
			// if(isEditor)
			scriptFolders.Add(PathUtils.GetScriptFolder(MonoScriptType.EditorForm));

			AddScripts(CompileScriptsInFolders(scriptFolders.ToArray()));
		}

		private void LoadPrecompiledAssemblies()
		{
			//Add pre-compiled assemblies / plugins
			AddScripts(ScriptCompiler.LoadLibrariesInFolder(Path.Combine(PathUtils.GetScriptsFolder(), "Plugins")));
		}

		private void AddScripts(CryScript[] scripts)
		{
			if(scripts == null || scripts.Length < 1)
				return;

			m_compiledScripts.AddRange(scripts);
		}

	    /// <summary>
	    /// Instantiates a script using its name and interface.
	    /// </summary>
	    /// <param name="scriptName"></param>
	    /// <param name="constructorParams"></param>
	    /// <returns>New instance scriptId or -1 if instantiation failed.</returns>
	    [EditorBrowsable(EditorBrowsableState.Never)]
		public int InstantiateScript(string scriptName, object[] constructorParams = null)
		{
			if(scriptName.Length < 1)
			{
				Console.LogAlways("Empty script passed to InstantiateClass");

				return -1;
			}

			// I can play with sexy lambdas too!
			int index = m_compiledScripts.FindIndex(x => x.className.Equals(scriptName));
			if(index == -1)
			{
				Console.LogAlways("Failed to instantiate {0}, compiled script could not be found.", scriptName);

				return -1;
			}

			CryScript script = m_compiledScripts.ElementAt(index);
			if(!script.Type.Implements(typeof(CryScriptInstance)))
				return -1;


			m_numInstances++;
			//ScriptId

			script.ScriptInstances.Add(Activator.CreateInstance(script.Type, constructorParams) as CryScriptInstance);
			script.ScriptInstances.Last().ScriptId = m_numInstances;

			m_compiledScripts[index] = script;

			return m_numInstances;
		}

		[EditorBrowsable(EditorBrowsableState.Never)]
		public void RemoveInstance(int scriptId, string scriptName)
		{
			int index = m_compiledScripts.FindIndex(x => x.className.Equals(scriptName));
			if(index == -1)
				return;

			CryScript script = m_compiledScripts[index];

			int instanceIndex = script.ScriptInstances.FindIndex(x => x.ScriptId == scriptId);
			script.ScriptInstances.RemoveAt(instanceIndex);

			m_compiledScripts[index] = script;

			// Allow re-use of Id's.
			if(m_numInstances == scriptId)
				m_numInstances--;
		}

		[EditorBrowsable(EditorBrowsableState.Never)]
		public object InvokeScriptFunctionById(int id, string func, object[] args = null)
		{
			CryScriptInstance scriptInstance = GetScriptInstanceById(id);
			if(scriptInstance == default(CryScriptInstance))
			{
				Console.LogAlways("Failed to invoke method, script instance was invalid");
				return null;
			}

			return InvokeScriptFunction(scriptInstance, func, args);
		}

		public CryScriptInstance GetScriptInstanceById(int id)
		{
			for(int i = 0; i < m_compiledScripts.Count; i++)
			{
				if(m_compiledScripts[i].ScriptInstances != null)
				{
					CryScriptInstance tempInstance = m_compiledScripts[i].ScriptInstances.FirstOrDefault(instance => instance.ScriptId == id);

					if(tempInstance != default(CryScriptInstance))
						return tempInstance;
				}
			}

			return null;
		}

		/// <summary>
		/// Automagically registers scriptbind methods to rid us of having to add them in both C# and C++.
		/// </summary>
		[EditorBrowsable(EditorBrowsableState.Never)]
		public void RegisterScriptbind(string namespaceName, string className, object[] methods)
		{
			if(scriptBinds == null)
				scriptBinds = new List<Scriptbind>();

			scriptBinds.Add(new Scriptbind(namespaceName, className, methods));
		}

        public void OnFileChange(string filePath)
        {
            string file = filePath.Split('/').Last();

            if (file.Contains(".cs"))
            {
                file = file.Split('.').First();
				// TODO: Script reloading
            }
            else if (file.Contains(".dll"))
            {
                file = file.Split('.').First();
				// TODO: Dll reloading
            }
        }

		/// <summary>
		/// Called once per frame.
		/// </summary>
		public void OnUpdate(float frameTime)
		{
			Time.DeltaTime = frameTime;

			Parallel.ForEach(m_compiledScripts, script =>
			{
				script.ScriptInstances.Where(i => i.ReceiveUpdates).ToList().ForEach(i => i.OnUpdate());
			});
		}

		List<Scriptbind> scriptBinds;

		List<CryScript> m_compiledScripts;
		int m_numInstances;
	}

	[EditorBrowsable(EditorBrowsableState.Never)]
	public class _ScriptCompiler : MarshalByRefObject
	{
		public static void GenerateScriptbindAssembly(Scriptbind[] scriptBinds)
		{
			List<string> sourceCode = new List<string>();
			sourceCode.Add("using System.Runtime.CompilerServices;");

			foreach(var scriptBind in scriptBinds)
			{
				sourceCode.Add(String.Format("namespace {0}", scriptBind.namespaceName) + "{");

				sourceCode.Add(String.Format("    public partial class {0}", scriptBind.className) + "    {");

				foreach(InternalCallMethod method in scriptBind.methods)
				{
					string parameters = method.parameters;
					string returnType = method.returnType;

					ConvertToCSharp(ref returnType);

					// Convert C++ types to C# ones
					string fixedParams = "";
					string[] splitParams = parameters.Split(',');
					for(int i = 0; i < splitParams.Length; i++)
					{
						string param = splitParams[i];
						ConvertToCSharp(ref param);
						fixedParams += param;
						if(param.Last() != ' ')
							fixedParams += ' ';

						string varName = param;

						if(varName.First() == ' ')
							varName = varName.Remove(0, 1);
						if(varName.Last() == ' ')
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
			if(cplusplusTypes.EndsWith("&"))
			{
				cplusplusTypes = cplusplusTypes.Replace("&", "");

				cplusplusTypes = cplusplusTypes.Insert(0, "ref ");
				// Remove annoying extra space.
				if(cplusplusTypes.ElementAt(4) == ' ')
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

		/// <summary>
		/// This function will automatically scan for C# dll (*.dll) files and load the types contained within them.
		/// </summary>
		public static CryScript[] LoadLibrariesInFolder(string directory)
		{
			if(!Directory.Exists(directory))
			{
				Console.LogAlways("Libraries failed to load; Folder {0} does not exist.", directory);
				return null;
			}

			var plugins = Directory.GetFiles(directory, "*.dll", SearchOption.AllDirectories);

			if(plugins != null && plugins.Length != 0)
			{
				List<CryScript> compiledScripts = new List<CryScript>();

				foreach(var plugin in plugins)
				{
					try
					{
#if !RELEASE
						Pdb2Mdb.Driver.Convert(plugin);
#endif

						AssemblyName assemblyName = AssemblyName.GetAssemblyName(plugin);

						//Process it, in case it contains types/gamerules
						Assembly assembly = Assembly.LoadFrom(plugin);

						referencedAssemblies.Add(plugin);

						compiledScripts.AddRange(LoadAssembly(assembly));
					}
					//This exception tells us that the assembly isn't a valid .NET assembly for whatever reason
					catch(BadImageFormatException)
					{
						Console.LogAlways("Plugin loading failed for {0}; dll is not valid.", plugin);
					}
				}

				return compiledScripts.ToArray();
			}
			else
				Console.LogAlways("No plugins detected.");

			return null;
		}

		/// <summary>
		/// This function will automatically scan for C# (*.cs) files and compile them using CompileScripts.
		/// </summary>
		public static CryScript[] CompileScriptsInFolder(string directory)
		{
			if(!Directory.GetParent(directory).Exists)
			{
				Console.LogAlways("Aborting script compilation; script directory parent could not be located.");
				return null;
			}

			if(!Directory.Exists(directory))
			{
				Console.LogAlways("Script compilation failed; Folder {0} does not exist.", directory);
				return null;
			}

			string[] scriptsInFolder = Directory.GetFiles(directory, "*.cs", SearchOption.AllDirectories);
			if(scriptsInFolder == null || scriptsInFolder.Length < 1)
			{
				Console.LogAlways("No scripts were found in {0}.", directory);
				return null;
			}

			return CompileScripts(scriptsInFolder, ".cs");
		}

		public static CryScript[] CompileScriptsInFolders(string[] scriptFolders)
		{
			List<string> scripts = new List<string>();
			foreach(var directory in scriptFolders)
			{
				if(Directory.Exists(directory))
					scripts.AddRange(Directory.GetFiles(directory, "*.cs", SearchOption.AllDirectories));
				else
					Console.LogAlways("Could not compile scripts in {0}; directory not found", directory);
			}

			if(scripts.Count > 0)
				return CompileScripts(scripts.ToArray(), ".cs");
			else
				return null;
		}

		/// <summary>
		/// Compiles the scripts and compiles them into an assembly.
		/// </summary>
		/// <param name="scripts">A string array containing full paths to scripts to be compiled.</param>
		/// <returns></returns>
		public static CryScript[] CompileScripts(string[] scripts, string scriptExtension)
		{
			if(scripts.Length < 1)
				return null;

			CodeDomProvider provider;
			switch (scriptExtension) // TODO enum
			{
				case ".vb":
					provider = CodeDomProvider.CreateProvider("VisualBasic");
					break;
				case ".js":
					provider = CodeDomProvider.CreateProvider("JScript");
					break;
				case ".cs":
				default:
					provider = CodeDomProvider.CreateProvider("CSharp");
					break;
			}

			CompilerParameters compilerParameters = new CompilerParameters();

			compilerParameters.GenerateExecutable = false;

			compilerParameters.GenerateInMemory = false;

			//Add additional assemblies as needed by gamecode to referencedAssemblies
			foreach(var script in scripts)
				GetScriptReferences(script);

			compilerParameters.ReferencedAssemblies.AddRange(referencedAssemblies.ToArray());

#if RELEASE
			compilerParameters.IncludeDebugInformation = false;
#else
			// Necessary for stack trace line numbers etc
           compilerParameters.IncludeDebugInformation = true;
#endif

			// We've got to get that assembly reference generator working. (Slap me if I accidentally commit this)
			// Consider yourself slapped. Here's a mildly less fugly (read: hardcoded) solution.
			// TODO: That ref generator.
			compilerParameters.ReferencedAssemblies.Add(PathUtils.GetGacFolder() + @"\System.Windows.Forms\4.0.0.0__b77a5c561934e089\System.Windows.Forms.dll");
			compilerParameters.ReferencedAssemblies.Add(PathUtils.GetGacFolder() + @"\System.Drawing\4.0.0.0__b03f5f7f11d50a3a\System.Drawing.dll");

			try
			{
				CompilerResults results = provider.CompileAssemblyFromFile(compilerParameters, scripts);

				provider = null;
				compilerParameters = null;

				if (results.CompiledAssembly != null) // success
					return LoadAssembly(results.CompiledAssembly);
				else if (results.Errors.HasErrors)
				{
					Console.LogAlways("Compilation failed; {0} errors:", results.Errors.Count);

					foreach (CompilerError error in results.Errors)
						Console.LogAlways(error.ErrorText);
				}
				else
					throw new ArgumentNullException("Tried loading a NULL assembly");
			}
			catch(Exception ex)
			{
				Console.LogException(ex);
			}

			return null;
		}

		//Fugly hack for referenced assemblies
		static void GetScriptReferences(string script)
		{
			if(String.IsNullOrEmpty(script))
				return;

			using(var stream = new FileStream(script, FileMode.Open))
			{
				using(var reader = new StreamReader(stream))
				{
					string line;
					// Just in case the file starts with an empty line.
					bool started = false;

					while((line = reader.ReadLine()) != null)
					{
						//Filter for using statements
						if (line.StartsWith("using") && line.EndsWith(";"))
						{
							ProcessNamespace(line.Replace("using ", "").Replace(";", ""));

							started = true;
						}
						else if(started)
							return;
					}

					reader.Close();
				}

				stream.Close();
			}
		}

		static void ProcessNamespace(string name)
		{
			if (name.StartsWith("CryEngine"))
				return;

			XDocument assemblyLookup = XDocument.Load(Path.Combine(PathUtils.GetEngineFolder(), "Mono", "assemblylookup.xml"));
			foreach(var node in assemblyLookup.Descendants())
			{
				if (node.Name.LocalName == "Namespace" && node.Attribute("name").Value==name)
				{
					string fullName = node.Parent.Attribute("name").Value;

					string[] assemblies = Directory.GetFiles(Path.Combine(PathUtils.GetEngineFolder(), "Mono", "lib", "mono", "gac"), "*.dll", SearchOption.AllDirectories);
					foreach (var assembly in assemblies)
					{
						if (assembly == fullName)
						{
							fullName = assembly;
							break;
						}
					}

					if (!referencedAssemblies.Contains(fullName))
					{
						Console.LogAlways("Adding an additional assembly, {0}", fullName);
						referencedAssemblies.Add(fullName);
					}
					else
					{
						Console.LogAlways("Skipping additional assembly, {0}, already queued", fullName);
					}
				}
			}
		}

		/// <summary>
		/// Loads an C# assembly and return encapulsated script Type.
		/// </summary>
		public static CryScript[] LoadAssembly(Assembly assembly)
		{
			var assemblyTypes = assembly.GetTypes().Where(type => type.Implements(typeof(CryScriptInstance)));

			List<CryScript> scripts = new List<CryScript>();

			Parallel.For(0, assemblyTypes.Count(), i =>
			{
				var type = assemblyTypes.ElementAt(i);

				if(!type.ContainsAttribute<ExcludeFromCompilationAttribute>())
				{
					var scriptType = MonoScriptType.Null;

					if (type.Implements(typeof(BaseGameRules)))
						scriptType = MonoScriptType.GameRules;
					else if (type.Implements(typeof(BasePlayer)))
						scriptType = MonoScriptType.Actor;
					else if (type.Implements(typeof(Entity)))
						scriptType = MonoScriptType.Entity;
					else if (type.Implements(typeof(StaticEntity)))
						scriptType = MonoScriptType.StaticEntity;
					else if (type.Implements(typeof(FlowNode)))
						scriptType = MonoScriptType.FlowNode;
					else if (type.Implements(typeof(CryScriptInstance)))
						scriptType = MonoScriptType.Other;
					else if (type.Implements(typeof(Sandbox.EditorForm)))
					{
						Sandbox.EditorForm form = Activator.CreateInstance(type) as Sandbox.EditorForm;
						form.Activate();
						form.Show();

						scriptType = MonoScriptType.EditorForm;
					}

					if (type != null)
					{
						scripts.Add(new CryScript(type, scriptType));

						// This is done after CryScript construction to avoid calling Type.name several times
						if (scriptType == MonoScriptType.GameRules)
						{
							GameRulesSystem._RegisterGameMode(scripts.Last().className);

							if (type.ContainsAttribute<DefaultGamemodeAttribute>())
								GameRulesSystem._SetDefaultGameMode(scripts.Last().className);
						}
						else if (scriptType == MonoScriptType.Actor)
							ActorSystem._RegisterActorClass(scripts.Last().className, false);
						else if (scriptType == MonoScriptType.Entity || scriptType == MonoScriptType.StaticEntity)
							LoadEntity(type, scripts.Last(), scriptType == MonoScriptType.StaticEntity);
						else if (scriptType == MonoScriptType.FlowNode)
							LoadFlowNode(type, scripts.Last().className);
					}
				}
			});

			assemblyTypes = null;

			return scripts.ToArray();
		}

		internal struct StoredNode
		{
			public StoredNode(string Class, string Category)
				: this()
			{
				className = Class;
				category = Category;
			}

			public string className;
			public string category;
		}

		internal static List<StoredNode> m_flowNodes;

		internal static void RegisterFlownodes()
		{
			foreach(var node in m_flowNodes)
				FlowSystem.RegisterNode(node.className, node.category, node.category.Equals("entity", StringComparison.Ordinal));
		}

		private static void LoadEntity(Type type, CryScript script, bool staticEntity)
		{
			EntityConfig config = default(EntityConfig);
			StaticEntity entity = null;

			if(staticEntity)
				entity = Activator.CreateInstance(type) as StaticEntity;
			else
				entity = Activator.CreateInstance(type) as Entity;

			config = entity.GetEntityConfig();

			entity = null;

			if(config.registerParams.Name.Length <= 0)
				config.registerParams.Name = script.className;
			if(config.registerParams.Category.Length <= 0)
				config.registerParams.Category = ""; // TODO: Use the folder structure in Scripts/Entities. (For example if the entity is in Scripts/Entities/Multiplayer, the category should become "Multiplayer")

			EntitySystem.RegisterEntityClass(config);

			LoadFlowNode(type, config.registerParams.Name, true);
		}

		private static void LoadFlowNode(Type type, string nodeName, bool entityNode = false)
		{
			string category = null;

			if(!entityNode)
			{
				category = type.Namespace;

				FlowNodeAttribute nodeInfo;
				if(type.TryGetAttribute<FlowNodeAttribute>(out nodeInfo))
				{
					if(nodeInfo.UICategory != null)
						category = nodeInfo.UICategory;

					if(nodeInfo.Name != null)
						nodeName = nodeInfo.Name;
				}
			}
			else
				category = "entity";

			m_flowNodes.Add(new StoredNode(nodeName, category));
		}

		public static object InvokeScriptFunction(object scriptInstance, string func, object[] args = null)
		{
			if(scriptInstance == null)
			{
				Console.LogAlways("Attempted to invoke method {0} with an invalid instance.", func);
				return null;
			}

			// TODO: Solve the problem with multiple function definitions.
			MethodInfo methodInfo = scriptInstance.GetType().GetMethod(func);
			if(methodInfo == null)
			{
				Console.LogAlways("Could not find method {0} in type {1}", func, scriptInstance.GetType().ToString());
				return null;
			}

			// Sort out optional parameters
			ParameterInfo[] info = methodInfo.GetParameters();

			if(info.Length > 0)
			{
				object[] tempArgs;
				tempArgs = new object[info.Length];
				int argIndexLength = args.Length - 1;

				for(int i = 0; i < info.Length; i++)
				{
					if(i <= argIndexLength)
						tempArgs.SetValue(args[i], i);
					else if(i > argIndexLength && info[i].IsOptional)
						tempArgs[i] = info[i].DefaultValue;
				}

				args = null;
				args = tempArgs;
				tempArgs = null;
			}
			else
				args = null;

			object result = methodInfo.Invoke(scriptInstance, args);

			args = null;
			methodInfo = null;
			info = null;

			return result;
		}

		/// <summary>
		/// All libraries passed through LoadLibrariesInFolder will be automatically added to this list.
		/// </summary>
		public static List<string> referencedAssemblies;
	}

	public enum MonoScriptType
	{
		Null = -1,
		/// <summary>
		/// Scripts directly inheriting from BaseGameRules will utilize this script type.
		/// </summary>
		GameRules,
		/// <summary>
		/// Scripts directly inheriting from FlowNode will utilize this script type.
		/// </summary>
		FlowNode,
		/// <summary>
		/// Scripts directly inheriting from StaticEntity will utilize this script type.
		/// </summary>
		StaticEntity,
		/// <summary>
		/// Scripts directly inheriting from Entity will utilize this script type.
		/// </summary>
		Entity,
		/// <summary>
		/// Scripts directly inheriting from Actor will utilize this script type.
		/// </summary>
		Actor,
		/// <summary>
		/// 
		/// </summary>
		EditorForm,
		/// <summary>
		/// Scripts will be linked to this type if they inherit from CryScriptInstance, but not any other script base.
		/// </summary>
		Other
	}

	/// <summary>
	/// Represents a given class.
	/// </summary>
	public struct CryScript
	{
		public CryScript(Type _type, MonoScriptType type)
			: this()
		{
			Type = _type;
			ScriptType = type;
			ScriptInstances = new List<CryScriptInstance>();
			className = Type.Name;
		}

		public Type Type { get; private set; }
		public MonoScriptType ScriptType { get; private set; }

		// Type.Name is costly to call
		public string className { get; private set; }

		/// <summary>
		/// Stores all instances of this class.
		/// </summary>
		public List<CryScriptInstance> ScriptInstances { get; private set; }
	}

	public struct InternalCallMethod
	{
		public string name;
		public string returnType;

		public string parameters;
	}

	public struct Scriptbind
	{
		public Scriptbind(string Namespace, string Class, object[] Methods)
			: this()
		{
			namespaceName = Namespace;
			className = Class;

			methods = Methods;
		}

		public string namespaceName;
		public string className;

		/// <summary>
		/// Array of InternalCallMethod
		/// </summary>
		public object[] methods;
	}
}