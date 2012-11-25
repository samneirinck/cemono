using System;
using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using CryEngine.Extensions;
using CryEngine.Initialization;
using CryEngine.Testing;
using CryEngine.Utilities;

namespace CryEngine.Compilers.NET
{
	public class NETCompiler : ScriptCompiler
	{
		public override IEnumerable<CryScript> Process(IEnumerable<Assembly> assemblies)
		{
            var scripts = new List<CryScript>();

            foreach (var assembly in assemblies)
                scripts.AddRange(ProcessAssembly(assembly));

            scripts.AddRange(ProcessAssembly(CompileCSharpFromSource()));
          //  scripts.AddRange(ProcessAssembly(CompileVisualBasicFromSource()));

            return scripts;
		}

		bool TryGetActorParams(ref IScriptRegistrationParams registrationParams, Type type)
		{
			var actorRegistrationParams = new ActorRegistrationParams();

			registrationParams = actorRegistrationParams;

			return true;
		}

		bool TryGetEntityParams(ref IScriptRegistrationParams registrationParams, Type type)
		{
			var entityRegistrationParams = new EntityRegistrationParams();

			//LoadFlowNode(ref script, true);

			BindingFlags flags = BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance;
			var folders = type.GetNestedTypes(flags).Where(x => x.ContainsAttribute<EditorPropertyFolderAttribute>());
			var members = type.GetMembers(flags);
			var entityProperties = new List<object>();

			EntityProperty property;
			members.ForEach(member =>
				{
					if (TryGetProperty(member, out property))
						entityProperties.Add(property);
				});

			folders.ForEach(folder =>
				{
					folder.GetMembers().ForEach(member =>
						{
							if (TryGetProperty(member, out property))
							{
								property.folder = folder.Name;
								entityProperties.Add(property);
							}
						});
				});

			entityRegistrationParams.properties = entityProperties.ToArray();

			EntityAttribute entAttribute;
			if (type.TryGetAttribute(out entAttribute))
			{
				entityRegistrationParams.name = entAttribute.Name;

                var curType = type;

                bool changedFlags = false;

                var entType = typeof(Entity);
                while (curType != entType)
                {
                    if (type.TryGetAttribute(out entAttribute))
                    {
                        // don't override if the type before this (or earlier) changed it.
                        if(entityRegistrationParams.category == null)
                            entityRegistrationParams.category = entAttribute.Category;
                        if(entityRegistrationParams.editorHelper == null)
                            entityRegistrationParams.editorHelper = entAttribute.EditorHelper;
                        if(entityRegistrationParams.editorIcon  == null)
                            entityRegistrationParams.editorIcon = entAttribute.Icon;
                        if (!changedFlags)
                        {
                            entityRegistrationParams.flags = entAttribute.Flags;
                            changedFlags = true;
                        }
                    }

                    curType = curType.BaseType;

                }
			}

			registrationParams = entityRegistrationParams;

			return true;
		}

		bool TryGetProperty(MemberInfo memberInfo, out EntityProperty property)
		{
			EditorPropertyAttribute propertyAttribute;
			if (memberInfo.TryGetAttribute(out propertyAttribute))
			{
				Type memberType = null;
				switch (memberInfo.MemberType)
				{
					case MemberTypes.Field:
						memberType = (memberInfo as FieldInfo).FieldType;
						break;
					case MemberTypes.Property:
						memberType = (memberInfo as PropertyInfo).PropertyType;
						break;
				}

				var limits = new EntityPropertyLimits(propertyAttribute.Min, propertyAttribute.Max);

				property = new EntityProperty(memberInfo.Name, propertyAttribute.Description, Entity.GetEditorType(memberType, propertyAttribute.Type), limits, propertyAttribute.Flags);
				return true;
			}

			property = new EntityProperty();
			return false;
		}

		bool TryGetFlowNodeParams(ref IScriptRegistrationParams registrationParams, Type type)
		{
			if (!type.GetMembers().Any(member => member.ContainsAttribute<PortAttribute>()))
				return false;

			var nodeRegistrationParams = new FlowNodeRegistrationParams();

			FlowNodeAttribute nodeInfo;
			if (type.TryGetAttribute(out nodeInfo))
			{
				if (!string.IsNullOrEmpty(nodeInfo.UICategory))
					nodeRegistrationParams.category = nodeInfo.UICategory;

				if (!string.IsNullOrEmpty(nodeInfo.Name))
					nodeRegistrationParams.name = nodeInfo.Name;
			}

			registrationParams = nodeRegistrationParams;

			return true;
		}

		bool TryGetGamemodeParams(ref IScriptRegistrationParams registrationParams, Type type)
		{
			var gamemodeRegistrationParams = new GameRulesRegistrationParams();

			GameRulesAttribute gamemodeAttribute;
			if (type.TryGetAttribute(out gamemodeAttribute))
			{
				if (!string.IsNullOrEmpty(gamemodeAttribute.Name))
					gamemodeRegistrationParams.name = gamemodeAttribute.Name;

				gamemodeRegistrationParams.defaultGamemode = gamemodeAttribute.Default;
			}

			registrationParams = gamemodeRegistrationParams;

			return true;
		}

        IEnumerable<CryScript> ProcessAssembly(Assembly assembly)
        {
            var scripts = new List<CryScript>();

            foreach (var type in assembly.GetTypes())
            {
				IScriptRegistrationParams registrationParams = null;

                CryScript script;
                if (!type.ContainsAttribute<ExcludeFromCompilationAttribute>() && CryScript.TryCreate(type, out script))
                {
					if (script.ScriptType.ContainsFlag(ScriptType.Actor))
						TryGetActorParams(ref registrationParams, script.Type);
					else if (script.ScriptType.ContainsFlag(ScriptType.GameRules))
						TryGetGamemodeParams(ref registrationParams, script.Type);
					else if (script.ScriptType.ContainsFlag(ScriptType.Entity))
						TryGetEntityParams(ref registrationParams, script.Type);
					else if (script.ScriptType.ContainsFlag(ScriptType.FlowNode))
					{
						if (!TryGetFlowNodeParams(ref registrationParams, script.Type))
							continue;
					}
                   // else if (script.ScriptType.ContainsFlag(ScriptType.UIEventSystem))
                       // UIEventSystem.Load(script);

                    if (script.ScriptType.ContainsFlag(ScriptType.CryScriptInstance))
                    {
                        foreach (var member in type.GetMethods(BindingFlags.Static | BindingFlags.DeclaredOnly | BindingFlags.Public))
                        {
							ConsoleCommandAttribute attribute;
							if (member.TryGetAttribute(out attribute))
								ConsoleCommand.Register(attribute.Name ?? member.Name, Delegate.CreateDelegate(typeof(ConsoleCommandDelegate), member as MethodInfo) as ConsoleCommandDelegate, attribute.Comment, attribute.Flags);
                        }
                    }

					script.RegistrationParams = registrationParams;

                    scripts.Add(script);
                }

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
            }

            return scripts;
        }

        Assembly CompileVisualBasicFromSource()
        {
            return CompileFromSource(CodeDomProvider.CreateProvider("VisualBasic"), "*.vb");
        }

		Assembly CompileCSharpFromSource()
        {
            return CompileFromSource(CodeDomProvider.CreateProvider("CSharp"), "*.cs");
        }

        Assembly CompileFromSource(CodeDomProvider provider, string searchPattern)
		{
			var compilerParameters = new CompilerParameters();

			compilerParameters.GenerateExecutable = false;

			// Necessary for stack trace line numbers etc
			compilerParameters.IncludeDebugInformation = true;
			compilerParameters.GenerateInMemory = false;

#if RELEASE
			if(!compilationParameters.ForceDebugInformation)
			{
				compilerParameters.GenerateInMemory = true;
				compilerParameters.IncludeDebugInformation = false;
			}
#endif

			if(!compilerParameters.GenerateInMemory)
			{
                var assemblyPath = Path.Combine(PathUtils.TempFolder, string.Format("CompiledScripts_{0}.dll", searchPattern.Replace("*.", "")));

				if(File.Exists(assemblyPath))
				{
					try
					{
						File.Delete(assemblyPath);
					}
					catch(Exception ex)
					{
						if(ex is UnauthorizedAccessException || ex is IOException)
							assemblyPath = Path.ChangeExtension(assemblyPath, "_" + Path.GetExtension(assemblyPath));
						else
							throw;
					}
				}

				compilerParameters.OutputAssembly = assemblyPath;
			}

			var scripts = new List<string>();
			var scriptsDirectory = PathUtils.ScriptsFolder;

			if(Directory.Exists(scriptsDirectory))
			{
                foreach (var script in Directory.GetFiles(scriptsDirectory, searchPattern, SearchOption.AllDirectories))
					scripts.Add(script);
			}
			else
				Debug.LogAlways("Scripts directory could not be located");

			CompilerResults results;
            using(provider)
			{
				var referenceHandler = new AssemblyReferenceHandler();
				compilerParameters.ReferencedAssemblies.AddRange(referenceHandler.GetRequiredAssembliesFromFiles(scripts));

				results = provider.CompileAssemblyFromFile(compilerParameters, scripts.ToArray());
			}

			return ScriptCompiler.ValidateCompilation(results);
		}
	}
}
