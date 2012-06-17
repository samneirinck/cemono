using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Windows.Forms;
using System.Xml;
using CryEngine.Extensions;
using CryEngine.Sandbox;
using CryEngine.Testing;
using CryEngine.Testing.Internals;

namespace CryEngine.Initialization
{
	class ScriptManager
	{
		static ScriptManager()
		{
			if(FlowNodes == null)
				FlowNodes = new List<string>();
			if(Scripts == null)
				Scripts = new List<CryScript>();
		}

		public ScriptManager()
		{
			if(!Directory.Exists(PathUtils.TempFolder))
				Directory.CreateDirectory(PathUtils.TempFolder);
			else
			{
				try
				{
					foreach(var file in Directory.GetFiles(PathUtils.TempFolder))
						File.Delete(file);
				}
				catch(UnauthorizedAccessException) { }
			}

			RegisterServices();
		}

		void RegisterServices()
		{
			FormHelper.Init();
			TestManager.Init();
		}

		void PopulateAssemblyLookup()
		{
#if !RELEASE
			var monoDir = Path.Combine(PathUtils.EngineFolder, "Mono");
			// Doesn't exist when unit testing
			if(Directory.Exists(monoDir))
			{
				using(XmlWriter writer = XmlWriter.Create(Path.Combine(monoDir, "assemblylookup.xml")))
				{
					writer.WriteStartDocument();
					writer.WriteStartElement("AssemblyLookupTable");

					var gacFolder = Path.Combine(monoDir, "lib", "mono", "gac");
					foreach(var assemblyLocation in Directory.GetFiles(gacFolder, "*.dll", SearchOption.AllDirectories))
					{
						var separator = new string[] { "__" };
						var splitParentDir = Directory.GetParent(assemblyLocation).Name.Split(separator, StringSplitOptions.RemoveEmptyEntries);

						var assembly = Assembly.Load(Path.GetFileName(assemblyLocation) + string.Format(", Version={0}, Culture=neutral, PublicKeyToken={1}", splitParentDir.ElementAt(0), splitParentDir.ElementAt(1)));

						writer.WriteStartElement("Assembly");
						writer.WriteAttributeString("name", assembly.FullName);

						foreach(var nameSpace in assembly.GetTypes().Select(t => t.Namespace).Distinct())
						{
							writer.WriteStartElement("Namespace");
							writer.WriteAttributeString("name", nameSpace);
							writer.WriteEndElement();
						}

						writer.WriteEndElement();
					}

					writer.WriteEndElement();
					writer.WriteEndDocument();
				}
			}
#endif
		}

		bool LoadPlugins()
		{
			LoadLibrariesInFolder(Path.Combine(PathUtils.ScriptsFolder, "Plugins"));

			return true;
		}

		public void PostInit()
		{
			// These have to be registered later on due to the flow system being initialized late.
			foreach(var node in FlowNodes)
				FlowNode.Register(node);
		}

		public void OnPostScriptReload()
		{
			ForEach(ScriptType.Any, x => x.OnScriptReload());
		}

		/// <summary>
		/// This function will automatically scan for C# dll (*.dll) files and load the types contained within them.
		/// </summary>
		public void LoadLibrariesInFolder(string directory)
		{
			if(directory == null)
				throw new ArgumentNullException("directory");
			else if(directory.Length < 1)
				throw new ArgumentException("string cannot be empty!", "directory");

			if(Directory.Exists(directory))
			{
				var plugins = Directory.GetFiles(directory, "*.dll", SearchOption.AllDirectories);

				if(plugins != null && plugins.Length != 0)
				{
					var typeCollection = new List<Type>();

					foreach(var plugin in plugins)
					{
						try
						{
							LoadAssembly(plugin);
						}
						//This exception tells us that the assembly isn't a valid .NET assembly for whatever reason
						catch(BadImageFormatException)
						{
							Debug.LogAlways("Plugin loading failed for {0}; dll is not valid.", plugin);
						}
					}
				}
			}
			else
				Debug.LogAlways("Skipping load of plugins in {0}, directory does not exist.", directory);
		}

		/// <summary>
		/// Processes a C# assembly and adds all found types to ScriptCompiler.CompiledScripts
		/// </summary>
		public static void ProcessAssembly(Assembly assembly)
		{
			if(assembly == null)
				throw new ArgumentNullException("assembly");

			foreach(var type in assembly.GetTypes())
				ProcessType(type);
		}

		/// <summary>
		/// Loads a C# assembly by location, creates a shadow-copy and generates debug database (mdb).
		/// </summary>
		/// <param name="assemblyPath"></param>
		public void LoadAssembly(string assemblyPath)
		{
			if(assemblyPath == null)
				throw new ArgumentNullException("assemblyPath");
			else if(assemblyPath.Length < 1)
				throw new ArgumentException("string cannot be empty!", "assemblyPath");

			var newPath = Path.Combine(PathUtils.TempFolder, Path.GetFileName(assemblyPath));

			TryCopyFile(assemblyPath, ref newPath, true);

#if !RELEASE
			GenerateDebugDatabaseForAssembly(assemblyPath);

			var mdbFile = assemblyPath + ".mdb";
			if(File.Exists(mdbFile)) // success
			{
				var newMdbPath = Path.Combine(PathUtils.TempFolder, Path.GetFileName(mdbFile));
				TryCopyFile(mdbFile, ref newMdbPath);
			}
#endif

			ProcessAssembly(Assembly.LoadFrom(newPath));
		}

		void TryCopyFile(string currentPath, ref string newPath, bool overwrite = true)
		{
			if(!File.Exists(newPath))
				File.Copy(currentPath, newPath, overwrite);
			else
			{
				try
				{
					File.Copy(currentPath, newPath, overwrite);
				}
				catch(Exception ex)
				{
					if(ex is UnauthorizedAccessException || ex is IOException)
					{
						newPath = Path.ChangeExtension(newPath, "_" + Path.GetExtension(newPath));
						TryCopyFile(currentPath, ref newPath);
					}
					else
						throw;
				}
			}
		}

		/// <summary>
		/// Adds the type to CompiledScripts
		/// </summary>
		/// <param name="type"></param>
		public static CryScript ProcessType(Type type)
		{
			if(type == null)
				throw new ArgumentNullException("type");

			if(Scripts.Any(x => x.Type == type))
				return Scripts.FirstOrDefault(x => x.Type == type);

			var script = new CryScript(type);
			if(!type.IsAbstract && !type.ContainsAttribute<ExcludeFromCompilationAttribute>() && !IgnoreExternalCalls)
			{
				if(script.ScriptType.ContainsFlag(ScriptType.Actor))
					Actor.Load(script);
				else if(script.ScriptType.ContainsFlag(ScriptType.Entity))
					Entity.Load(script);
				else if(script.ScriptType.ContainsFlag(ScriptType.FlowNode))
					FlowNode.Load(ref script);
				else if(script.ScriptType.ContainsFlag(ScriptType.GameRules))
					GameRules.Load(script);
				else if(script.ScriptType.ContainsFlag(ScriptType.UIEventSystem))
					UIEventSystem.Load(script);
				else if(script.ScriptType.ContainsFlag(ScriptType.ScriptCompiler))
				{
					Debug.LogAlways("		Compiling scripts using {0}...", type.Name);
					var compiler = Activator.CreateInstance(type) as ScriptCompiler;
					ProcessAssembly(compiler.Compile());
				}

				if(script.ScriptType.ContainsFlag(ScriptType.CryScriptInstance))
					ProcessMembers(type);
			}

			if(script.ScriptType.ContainsFlag(ScriptType.CryScriptInstance))
				Scripts.Add(script);

			return script;
		}

		/// <summary>
		/// Processes all members of a type for CryMono features such as CCommands.
		/// </summary>
		/// <param name="type"></param>
		public static void ProcessMembers(Type type)
		{
			if(type == null)
				throw new ArgumentNullException("type");

#if !RELEASE
			if(type.ContainsAttribute<TestCollectionAttribute>())
			{
				var ctor = type.GetConstructor(Type.EmptyTypes);
				if(ctor != null)
				{
					var collection = new TestCollection
					{
						Instance = ctor.Invoke(Type.EmptyTypes),
						Tests = from method in type.GetMethods()
								where method.ContainsAttribute<TestAttribute>()
									&& method.GetParameters().Length == 0
								select method
					};

					TestManager.TestCollections.Add(collection);
				}
			}
#endif

			SandboxExtensionAttribute attr;
			if(type.TryGetAttribute(out attr) && type.Implements<Form>())
			{
				Debug.LogAlways("Registering Sandbox extension: {0}", attr.Name);
				FormHelper.AvailableForms.Add(new FormInfo { Type = type, Data = attr });
			}

			foreach(var member in type.GetMembers(BindingFlags.Static | BindingFlags.DeclaredOnly | BindingFlags.Public))
			{
				switch(member.MemberType)
				{
					case MemberTypes.Method:
						{
							CCommandAttribute attribute;
							if(member.TryGetAttribute<CCommandAttribute>(out attribute))
								CCommand.Register(attribute.Name ?? member.Name, Delegate.CreateDelegate(typeof(CCommandDelegate), member as MethodInfo) as CCommandDelegate, attribute.Comment, attribute.Flags);
						}
						break;
					case MemberTypes.Field:
					case MemberTypes.Property:
						{
							CVarAttribute attribute;
							if(member.TryGetAttribute<CVarAttribute>(out attribute))
							{
								// There's no way to pass the variable itself by reference to properly register the CVar.
								//CVar.Register(attribute, member, 
							}
						}
						break;
				}
			}
		}

		public void GenerateDebugDatabaseForAssembly(string assemblyPath)
		{
			if(assemblyPath == null)
				throw new ArgumentNullException("assemblyPath");
			else if(assemblyPath.Length < 1)
				throw new ArgumentException("string cannot be empty!", "assemblyPath");

			if(File.Exists(Path.ChangeExtension(assemblyPath, "pdb")))
			{
				var assembly = Assembly.LoadFrom(Path.Combine(PathUtils.EngineFolder, "Mono", "bin", "pdb2mdb.dll"));
				var driver = assembly.GetType("Driver");
				var convertMethod = driver.GetMethod("Convert", BindingFlags.Static | BindingFlags.Public);

				object[] args = { assemblyPath };
				convertMethod.Invoke(null, args);
			}
		}

		internal static List<string> FlowNodes { get; set; }

		#region Statics
		/// <summary>
		/// Called once per frame.
		/// </summary>
		public static void OnUpdate(float frameTime)
		{
			Time.DeltaTime = frameTime;

			Scripts.ForEach(x =>
				{
					if(x.ScriptType.ContainsFlag(ScriptType.CryScriptInstance) && x.ScriptInstances != null)
					{
						x.ScriptInstances.ForEach(instance =>
						{
							if(instance.ReceiveUpdates)
								instance.OnUpdate();
						});
					}
				});
		}

		/// <summary>
		/// Instantiates a script using its name and interface.
		/// </summary>
		/// <param name="scriptName"></param>
		/// <param name="constructorParams"></param>
		/// <returns>New instance scriptId or -1 if instantiation failed.</returns>
		public static CryScriptInstance CreateScriptInstance(string scriptName, ScriptType scriptType, object[] constructorParams = null)
		{
			if(scriptName == null)
				throw new ArgumentNullException("scriptName");
			else if(scriptName.Length < 1)
				throw new ArgumentException("string cannot be empty!", "scriptName");
			else if(!Enum.IsDefined(typeof(ScriptType), scriptType))
				throw new ArgumentException(string.Format("scriptType: value {0} was not defined in the enum", scriptType));

			var script = Scripts.FirstOrDefault(x => x.ScriptType.ContainsFlag(scriptType) && x.ScriptName.Equals(scriptName));
			if(script == default(CryScript))
				throw new ScriptNotFoundException(string.Format("Script {0} of ScriptType {1} could not be found.", scriptName, scriptType));

			var scriptInstance = Activator.CreateInstance(script.Type, constructorParams) as CryScriptInstance;
			if(scriptInstance == null)
				throw new ArgumentException("Failed to create instance, make sure type derives from CryScriptInstance", "scriptName");

			if(scriptType == ScriptType.GameRules)
				GameRules.Current = scriptInstance as GameRules;

			AddScriptInstance(script, scriptInstance);

			return scriptInstance;
		}

		public static void AddScriptInstance(CryScriptInstance instance, ScriptType scriptType)
		{
			if(instance == null)
				throw new ArgumentNullException("instance");
			if(!Enum.IsDefined(typeof(ScriptType), scriptType))
				throw new ArgumentException(string.Format("scriptType: value {0} was not defined in the enum", scriptType));

			var script = FindScript(scriptType, x => x.Type == instance.GetType());
			if(script == default(CryScript))
				script = ProcessType(instance.GetType());

			AddScriptInstance(script, instance);
		}

		static void AddScriptInstance(CryScript script, CryScriptInstance instance)
		{
			var index = Scripts.IndexOf(script);

			instance.ScriptId = LastScriptId++;

			if(script.ScriptInstances == null)
				script.ScriptInstances = new List<CryScriptInstance>();

			script.ScriptInstances.Add(instance);

			Scripts[index] = script;
		}

		public static void RemoveInstance(int instanceId, ScriptType scriptType)
		{
			RemoveInstances<CryScriptInstance>(scriptType, x => x.ScriptId == instanceId);
		}

		/// <summary>
		/// Locates and destructs the script with the assigned scriptId.
		/// </summary>
		public static int RemoveInstances<T>(ScriptType scriptType, Predicate<T> match) where T : CryScriptInstance
		{
			if(!Enum.IsDefined(typeof(ScriptType), scriptType))
				throw new ArgumentException(string.Format("scriptType: value {0} was not defined in the enum", scriptType));

			int numRemoved = 0;
			for(int i = 0; i < Scripts.Count; i++)
			{
				var script = Scripts[i];

				if(script.ScriptType.ContainsFlag(scriptType))
				{
					if(script.ScriptInstances != null)
						numRemoved += script.ScriptInstances.RemoveAll(x => match(x as T));
				}

				Scripts[i] = script;
			}

			return numRemoved;
		}

		public static int RemoveInstances(ScriptType scriptType, Predicate<CryScriptInstance> match)
		{
			return RemoveInstances<CryScriptInstance>(scriptType, match);
		}

		public static CryScriptInstance GetScriptInstanceById(int id, ScriptType scriptType)
		{
			if(id == 0)
				throw new ArgumentException("instance id cannot be 0!");

			return Find<CryScriptInstance>(scriptType, x => x.ScriptId == id);
		}

		#region Linq statements
		public static CryScript FindScript(ScriptType scriptType, Func<CryScript, bool> predicate)
		{
			if(!Enum.IsDefined(typeof(ScriptType), scriptType))
				throw new ArgumentException(string.Format("scriptType: value {0} was not defined in the enum", scriptType));

			return Scripts.FirstOrDefault(x => x.ScriptType.ContainsFlag(scriptType) && predicate(x));
		}

		public static void ForEachScript(ScriptType scriptType, Action<CryScript> action)
		{
			if(!Enum.IsDefined(typeof(ScriptType), scriptType))
				throw new ArgumentException(string.Format("scriptType: value {0} was not defined in the enum", scriptType));

			Scripts.ForEach(x =>
				{
					if(x.ScriptType.ContainsFlag(scriptType))
						action(x);
				});
		}

		public static void ForEach(ScriptType scriptType, Action<CryScriptInstance> action)
		{
			if(!Enum.IsDefined(typeof(ScriptType), scriptType))
				throw new ArgumentException(string.Format("scriptType: value {0} was not defined in the enum", scriptType));

			ForEachScript(scriptType, script =>
			{
				if(script.ScriptInstances != null)
					script.ScriptInstances.ForEach(action);
			});
		}

		public static T Find<T>(ScriptType scriptType, Func<T, bool> predicate) where T : CryScriptInstance
		{
			if(!Enum.IsDefined(typeof(ScriptType), scriptType))
				throw new ArgumentException(string.Format("scriptType: value {0} was not defined in the enum", scriptType));

			T scriptInstance = null;

			ForEachScript(scriptType, script =>
				{
					if(script.ScriptInstances != null && script.Type.ImplementsOrEquals<T>())
					{
						var instance = script.ScriptInstances.Find(x => predicate(x as T)) as T;
						if(instance != null)
						{
							scriptInstance = instance;
							return;
						}
					}
				});

			return scriptInstance;
		}
		#endregion

		/// <summary>
		/// Last assigned ScriptId, next = + 1
		/// </summary>
		public static int LastScriptId = 1;

		public static bool IgnoreExternalCalls { get; set; }

		internal static List<CryScript> Scripts { get; set; }
		#endregion
	}

	[Serializable]
	public class ScriptNotFoundException : Exception
	{
		public ScriptNotFoundException(string error)
		{
			message = error;
		}

		private string message;
		public override string Message
		{
			get { return message; }
		}
	}
}