using System;
using System.Linq;
using System.IO;
using System.Collections.Generic;

using System.CodeDom.Compiler;
using Microsoft.CSharp;
using System.Reflection;

using CryEngine.Extensions;

using System.Xml;
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
			m_referencedAssemblies = new List<string>();

			m_numInstances = 0;
		}

		public void Initialize()
		{
			//GenerateScriptbindAssembly(scriptBinds.ToArray());

			Reload();
		}

		public void PostInit()
		{
			// These have to be registered later on due to the flow system being initialized late.
			RegisterFlownodes();
		}

		public void Reload()
		{
			//CreateNamespaceMaps();
			LoadPrecompiledAssemblies();

			AddScripts(CompileScriptsInFolders(new string[] { PathUtils.GetScriptFolder(MonoScriptType.Entity), PathUtils.GetScriptFolder(MonoScriptType.GameRules), PathUtils.GetScriptFolder(MonoScriptType.FlowNode) }));
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

			foreach(var tempScript in scripts)
			{
				if(tempScript.Type != null)
					m_compiledScripts.Add(tempScript);
			}
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


			if(script.ScriptInstances == null)
				script.ScriptInstances = new List<CryScriptInstance>();

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

            }
            else if (file.Contains(".dll"))
            {
                file = file.Split('.').First();
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
		static Dictionary<string, List<string>> assemblyNamespaceMapping;

		public static void CreateNamespaceMaps()
		{
			//Start with a fresh copy each time
			assemblyNamespaceMapping = new Dictionary<string, List<string>>();
			string currentAssembly = string.Empty;

			Console.LogAlways("Mapping .NET namespaces to libraries...");
			using(var stream = new FileStream(Path.Combine(PathUtils.GetEngineFolder(), "Mono", "assemblylookup.xml"),
				FileMode.Open))
			{
				using(var reader = new XmlTextReader(stream))
				{
					while(reader.Read())
					{
						switch(reader.NodeType)
						{
							case XmlNodeType.Element:
								switch(reader.Name)
								{
									//In the case of assemblies, create a new mapping entry
									case "Assembly":
										currentAssembly = reader.GetAttribute("name");
										assemblyNamespaceMapping.Add(currentAssembly, new List<string>());
										Console.LogAlways("Creating entry for assembly {0}", currentAssembly);
										break;

									//When we have a namespace, add it to the currently active mapping entry
									case "Namespace":
										var namespaceName = reader.GetAttribute("name");
										assemblyNamespaceMapping[currentAssembly].Add(namespaceName);
										Console.LogAlways("Associating namespace {0} with assembly {1}",
											namespaceName, currentAssembly);
										break;
								}
								break;
						}
					}
				}
			}
		}

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

						m_referencedAssemblies.Add(plugin);

						foreach(var script in LoadAssembly(assembly))
							compiledScripts.Add(script);
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

			return CompileScripts(scriptsInFolder);
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
				return CompileScripts(scripts.ToArray());
			else
				return null;
		}

		/// <summary>
		/// Compiles the scripts and compiles them into an assembly.
		/// </summary>
		/// <param name="scripts">A string array containing full paths to scripts to be compiled.</param>
		/// <returns></returns>
		public static CryScript[] CompileScripts(string[] scripts)
		{
			CodeDomProvider provider = new CSharpCodeProvider();
			CompilerParameters compilerParameters = new CompilerParameters();

			compilerParameters.GenerateExecutable = false;

			compilerParameters.GenerateInMemory = false; 

			//Add additional assemblies as needed by gamecode
			//foreach(var script in scripts)
			//	GetScriptReferences(script);

			compilerParameters.ReferencedAssemblies.AddRange(_requestedAssemblies.ToArray());

#if RELEASE
			compilerParameters.IncludeDebugInformation = false;
#else
			// Necessary for stack trace line numbers etc
           compilerParameters.IncludeDebugInformation = true;
#endif

			//Automatically add needed assemblies
			var assemblies = AppDomain.CurrentDomain.GetAssemblies().Select(a => a.Location);
			foreach(var assemblyPath in assemblies)
				compilerParameters.ReferencedAssemblies.Add(assemblyPath);

			foreach(var assemblyReference in m_referencedAssemblies)
				compilerParameters.ReferencedAssemblies.Add(assemblyReference);

			try
			{
				CompilerResults results = provider.CompileAssemblyFromFile(compilerParameters, scripts);

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

		/// <summary>
		/// We keep an internal list of requested assemblies from CryGameCode to avoid duplication when loading them.
		/// </summary>
		static List<string> _requestedAssemblies = new List<string>();

		//Fugly hack for referenced assemblies
		static void GetScriptReferences(string script)
		{
			if(string.IsNullOrEmpty(script))
				return;

			using(var stream = new FileStream(script, FileMode.Open))
			{
				using(var reader = new StreamReader(stream))
				{
					string line;
					while((line = reader.ReadLine()) != null)
					{
						//Quit once we get past using statements
						//FIXME: there's gotta be a better way.
						if(line.Contains("namespace") || line.Contains("public class"))
							return;

						//Filter for using statements
						if(line.StartsWith("using") && line.EndsWith(";"))
						{
							ProcessNamespace(line.Replace("using", "").Replace(";", ""));
						}
					}
				}
			}
		}

		static void ProcessNamespace(string name)
		{
			foreach(var assemblyEntry in assemblyNamespaceMapping)
			{
				if(assemblyEntry.Value.Contains(name))
				{
					var fullname = assemblyEntry.Key + ".dll";
					if(!_requestedAssemblies.Contains(fullname))
					{
						Console.LogAlways("Adding an additional assembly, {0}", fullname);
						_requestedAssemblies.Add(fullname);
					}
					else
					{
						Console.LogAlways("Skipping additional assembly, {0}, already queued", fullname);
					}
				}
			}
		}

		/// <summary>
		/// Loads an C# assembly and return encapulsated script Type.
		/// </summary>
		public static CryScript[] LoadAssembly(Assembly assembly)
		{
			var assemblyTypes = assembly.GetTypes().Where(type => type.Implements(typeof(CryScriptInstance))).ToArray();

			var scripts = new CryScript[assemblyTypes.Length];

			//Extract the types and load everything that implements IEntity, IGameRules and IFlowNode

			System.Diagnostics.Stopwatch stopWatch = new System.Diagnostics.Stopwatch();
			stopWatch.Start();

			//for(var i = 0; i < scripts.Length; i++)
			Parallel.For(0, scripts.Length, i =>
			{
				var type = assemblyTypes[i];

				if(!type.ContainsAttribute<ExcludeFromCompilationAttribute>())
				{
					var scriptType = MonoScriptType.Null;

					if(type.Implements(typeof(BaseGameRules)))
						scriptType = MonoScriptType.GameRules;
					else if(type.Implements(typeof(BasePlayer)))
						scriptType = MonoScriptType.Actor;
					else if(type.Implements(typeof(Entity)))
						scriptType = MonoScriptType.Entity;
					else if(type.Implements(typeof(StaticEntity)))
						scriptType = MonoScriptType.StaticEntity;
					else if(type.Implements(typeof(FlowNode)))
						scriptType = MonoScriptType.FlowNode;

					scripts[i] = new CryScript(type, scriptType);

					// This is done after CryScript construction to avoid calling Type.name several times
					if(scriptType == MonoScriptType.GameRules)
					{
						GameRulesSystem._RegisterGameMode(scripts[i].className);

						if(type.ContainsAttribute<DefaultGamemodeAttribute>())
							GameRulesSystem._SetDefaultGameMode(scripts[i].className);
					}
					else if(scriptType == MonoScriptType.Actor)
						ActorSystem._RegisterActorClass(scripts[i].className, false);
					else if(scriptType == MonoScriptType.Entity || scriptType == MonoScriptType.StaticEntity)
						LoadEntity(type, scripts[i], scriptType == MonoScriptType.StaticEntity);
					else if(scriptType == MonoScriptType.FlowNode)
						LoadFlowNode(type, scripts[i].className);
				}
			});

			return scripts;
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

				var nodeInfo = type.GetAttribute<FlowNodeAttribute>();
				if(nodeInfo.UICategory != null)
					category = nodeInfo.UICategory;

				if(nodeInfo.Name != null)
					nodeName = nodeInfo.Name;
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
		public static List<string> m_referencedAssemblies;
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
		public List<CryScriptInstance> ScriptInstances { get; set; }
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