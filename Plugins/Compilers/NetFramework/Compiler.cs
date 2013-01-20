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

using CryEngine.FlowSystem;

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
                    else if (script.ScriptType.ContainsFlag(ScriptType.EntityFlowNode))
                    {
                        if (!TryGetEntityFlowNodeParams(ref registrationParams, script.Type))
                            continue;
                    }
                    else if (script.ScriptType.ContainsFlag(ScriptType.FlowNode))
                    {
                        if (!TryGetFlowNodeParams(ref registrationParams, script.Type))
                            continue;
                    }

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

                if (type.ContainsAttribute<TestCollectionAttribute>())
                {
                    var ctor = type.GetConstructor(Type.EmptyTypes);
                    if (ctor != null)
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

            if (!compilerParameters.GenerateInMemory)
            {
                var assemblyPath = Path.Combine(PathUtils.TempFolder, string.Format("CompiledScripts_{0}.dll", searchPattern.Replace("*.", "")));

                if (File.Exists(assemblyPath))
                {
                    try
                    {
                        File.Delete(assemblyPath);
                    }
                    catch (Exception ex)
                    {
                        if (ex is UnauthorizedAccessException || ex is IOException)
                            assemblyPath = Path.ChangeExtension(assemblyPath, "_" + Path.GetExtension(assemblyPath));
                        else
                            throw;
                    }
                }

                compilerParameters.OutputAssembly = assemblyPath;
            }

            var scripts = new List<string>();
            var scriptsDirectory = PathUtils.ScriptsFolder;

            if (Directory.Exists(scriptsDirectory))
            {
                foreach (var script in Directory.GetFiles(scriptsDirectory, searchPattern, SearchOption.AllDirectories))
                    scripts.Add(script);
            }
            else
                Debug.LogAlways("Scripts directory could not be located");

            CompilerResults results;
            using (provider)
            {
                var referenceHandler = new AssemblyReferenceHandler();
                compilerParameters.ReferencedAssemblies.AddRange(referenceHandler.GetRequiredAssembliesFromFiles(scripts));

                results = provider.CompileAssemblyFromFile(compilerParameters, scripts.ToArray());
            }

            return ScriptCompiler.ValidateCompilation(results);
        }

        #region Actor
        bool TryGetActorParams(ref IScriptRegistrationParams registrationParams, Type type)
		{
			registrationParams = new ActorRegistrationParams();

			return true;
		}
        #endregion

        #region Entity
        bool TryGetEntityParams(ref IScriptRegistrationParams registrationParams, Type type)
		{
			var entityRegistrationParams = new EntityRegistrationParams();

			BindingFlags flags = BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance;
			var members = type.GetMembers(flags);
            var entityProperties = new Dictionary<string, List<EntityProperty>>();

            members.ForEach(member => TryGetEntityProperty(member, ref entityProperties));

            int numProperties = entityProperties.Count;
            if (numProperties > 0)
            {
                var folders = new EntityPropertyFolder[numProperties];

                for (int i = 0; i < numProperties; i++)
                {
                    var folderPair = entityProperties.ElementAt(i);
                    var folder = new EntityPropertyFolder();

                    folder.name = folderPair.Key;
                    folder.properties = folderPair.Value.Cast<object>().ToArray();

                    folders[i] = folder;
                }

                entityRegistrationParams.propertyFolders = folders.Cast<object>().ToArray();
            }

            var curType = type;

            bool changedFlags = false;

            var entType = typeof(Entity);
            // This should not be specific to entities, all scripts should be able to utilize this parent class attribute functionality.
            while (curType != entType)
            {
                EntityAttribute entAttribute;
                if (curType.TryGetAttribute(out entAttribute))
                {
                    // don't override if the type before this (or earlier) changed it.
                    if (entityRegistrationParams.name == null)
                        entityRegistrationParams.name = entAttribute.Name;
                    if (entityRegistrationParams.category == null)
                        entityRegistrationParams.category = entAttribute.Category;
                    if (entityRegistrationParams.editorHelper == null)
                        entityRegistrationParams.editorHelper = entAttribute.EditorHelper;
                    if (entityRegistrationParams.editorIcon == null)
                        entityRegistrationParams.editorIcon = entAttribute.Icon;
                    if (!changedFlags)
                    {
                        entityRegistrationParams.flags = entAttribute.Flags;
                        changedFlags = true;
                    }
                }

                curType = curType.BaseType;
            }

			registrationParams = entityRegistrationParams;

			return true;
		}

        bool TryGetEntityProperty(MemberInfo memberInfo, ref Dictionary<string, List<EntityProperty>> folders)
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

                var property = new EntityProperty(propertyAttribute.Name ?? memberInfo.Name, propertyAttribute.Description, Entity.GetEditorType(memberType, propertyAttribute.Type), limits, propertyAttribute.Flags);

                if (propertyAttribute.Folder == null)
                    propertyAttribute.Folder = "Default";

                if (!folders.ContainsKey(propertyAttribute.Folder))
                    folders.Add(propertyAttribute.Folder, new List<EntityProperty>());

                folders[propertyAttribute.Folder].Add(property);

				return true;
			}

			return false;
		}
        #endregion

        #region FlowNode
        bool TryGetFlowNodeParams(ref IScriptRegistrationParams registrationParams, Type type)
		{
			var nodeRegistrationParams = new FlowNodeRegistrationParams();

            var inputs = new Dictionary<InputPortConfig, MethodInfo>();
            var outputs = new Dictionary<OutputPortConfig, MemberInfo>();

            var setFilter = false;
            var setTargetEntity = false;
            var setType = false;

            var nodeType = type;
            while (nodeType != typeof(FlowNode))
            {
                FlowNodeAttribute nodeAttribute;
                if (nodeType.TryGetAttribute(out nodeAttribute))
                {
                    if(nodeRegistrationParams.category == null)
                        nodeRegistrationParams.category = nodeAttribute.Category;
                    if (nodeRegistrationParams.name == null)
                        nodeRegistrationParams.name = nodeAttribute.Name;
                    if (nodeRegistrationParams.description == null)
                        nodeRegistrationParams.description = nodeAttribute.Description;

                    if (!setFilter)
                    {
                        nodeRegistrationParams.filter = nodeAttribute.Filter;
                        setFilter = true;
                    }
                    if (!setTargetEntity)
                    {
                        nodeRegistrationParams.hasTargetEntity = nodeAttribute.TargetsEntity;
                        setTargetEntity = true;
                    }
                    if (!setType)
                    {
                        nodeRegistrationParams.type = nodeAttribute.Type;
                        setType = true;
                    }
                }

                TryGetFlowNodePorts(nodeType, ref inputs, ref outputs);

                nodeType = nodeType.BaseType;
            }

            if (inputs.Count == 0 && outputs.Count == 0)
                return false;

            nodeRegistrationParams.InputPorts = inputs.Keys.ToArray();
            nodeRegistrationParams.OutputPorts = outputs.Keys.ToArray();

            nodeRegistrationParams.InputMethods = inputs.Values.ToArray();
            nodeRegistrationParams.OutputMembers = outputs.Values.ToArray();

            registrationParams = nodeRegistrationParams;

			return true;
		}

        bool TryGetEntityFlowNodeParams(ref IScriptRegistrationParams registrationParams, Type type)
        {
            var nodeRegistrationParams = new EntityFlowNodeRegistrationParams();

            var curEntityType = type.GetGenericArguments(typeof(EntityFlowNode<>)).ElementAt(0);
            nodeRegistrationParams.entityName = curEntityType.Name;

            var entType = typeof(EntityBase);

            // This should not be specific to entities, all scripts should be able to utilize this parent class attribute functionality.
            while (curEntityType != entType)
            {
                EntityAttribute entAttribute;
                if (curEntityType.TryGetAttribute(out entAttribute))
                {
                    // don't override if the type before this (or earlier) changed it.
                    if (nodeRegistrationParams.entityName == null)
                        nodeRegistrationParams.entityName = entAttribute.Name;
                }

                curEntityType = curEntityType.BaseType;
            }

            var inputs = new Dictionary<InputPortConfig, MethodInfo>();
            var outputs = new Dictionary<OutputPortConfig, MemberInfo>();

            var nodeType = type;
            while (nodeType != typeof(FlowNode))
            {
                TryGetFlowNodePorts(nodeType, ref inputs, ref outputs);

                nodeType = nodeType.BaseType;
            }

            if (inputs.Count == 0 && outputs.Count == 0)
                return false;

            nodeRegistrationParams.InputPorts = inputs.Keys.ToArray();
            nodeRegistrationParams.OutputPorts = outputs.Keys.ToArray();

            nodeRegistrationParams.InputMethods = inputs.Values.ToArray();
            nodeRegistrationParams.OutputMembers = outputs.Values.ToArray();

            registrationParams = nodeRegistrationParams;

            return true;
        }

        void TryGetFlowNodePorts(Type type, ref Dictionary<InputPortConfig, MethodInfo> inputs, ref Dictionary<OutputPortConfig, MemberInfo> outputs)
        {
            foreach (var member in type.GetMembers(BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.DeclaredOnly))
                {
                    PortAttribute portAttribute;
                    if (member.TryGetAttribute(out portAttribute))
                    {
                        switch (member.MemberType)
                        {
                            case MemberTypes.Method:
                                {
                                    var method = member as MethodInfo;

                                    InputPortConfig inputPortConfig;
                                    if (TryGetFlowNodeInput(portAttribute, method, out inputPortConfig))
                                        inputs.Add(inputPortConfig, method);
                                }
                                break;
                            case MemberTypes.Field:
                            case MemberTypes.Property:
                                {
                                    OutputPortConfig outputPortConfig;
                                    if (TryGetFlowNodeOutput(portAttribute, member, out outputPortConfig))
                                        outputs.Add(outputPortConfig, member);
                                }
                                break;
                        }
                    }
                }
        }

        bool TryGetFlowNodeInput(PortAttribute portAttribute, MethodInfo method, out InputPortConfig inputPortConfig)
        {
            string portPrefix = null;

            object defaultVal = null;

            inputPortConfig = new InputPortConfig();

            if (method.GetParameters().Length > 0)
            {
                ParameterInfo parameter = method.GetParameters()[0];
                if (parameter.ParameterType.IsEnum)
                {
                    inputPortConfig.type = NodePortType.Int;

                    var values = Enum.GetValues(parameter.ParameterType);
                    if (values.Length <= 0)
                        return false;

                    defaultVal = values.GetValue(0);

                    inputPortConfig.uiConfig = "enum_int:";

                    for (int i = 0; i < values.Length; i++)
                    {
                        var value = values.GetValue(i);

                        if (i > 0 && i != inputPortConfig.uiConfig.Length)
                            inputPortConfig.uiConfig += ",";

                        inputPortConfig.uiConfig += Enum.GetName(parameter.ParameterType, value) + "=" + (int)value;
                    }
                }
                else
                    inputPortConfig.type = GetFlowNodePortType(parameter.ParameterType, out portPrefix, portAttribute);

                if (parameter.IsOptional && defaultVal == null)
                    defaultVal = parameter.DefaultValue;
                else if (defaultVal == null)
                {
                    switch (inputPortConfig.type)
                    {
                        case NodePortType.Bool:
                            defaultVal = false;
                            break;
                        case NodePortType.EntityId:
                            defaultVal = 0;
                            break;
                        case NodePortType.Float:
                            defaultVal = 0.0f;
                            break;
                        case NodePortType.Int:
                            defaultVal = 0;
                            break;
                        case NodePortType.String:
                            defaultVal = "";
                            break;
                        case NodePortType.Vec3:
                            defaultVal = Vec3.Zero;
                            break;
                    }
                }
            }
            else
                inputPortConfig.type = NodePortType.Void;

            string portName = (portAttribute.Name ?? method.Name);

            if (portPrefix != null)
                inputPortConfig.name = portPrefix + portName;
            else
                inputPortConfig.name = portName;

            inputPortConfig.defaultValue = defaultVal;
            inputPortConfig.description = portAttribute.Description;
            inputPortConfig.humanName = portAttribute.Name ?? method.Name;
            return true;
        }

        bool TryGetFlowNodeOutput(PortAttribute portAttribute, MemberInfo memberInfo, out OutputPortConfig outputPortConfig)
        {
            outputPortConfig = new OutputPortConfig();

            outputPortConfig.name = portAttribute.Name ?? memberInfo.Name;
            outputPortConfig.humanName = outputPortConfig.name;
            outputPortConfig.description = portAttribute.Description;

            Type type;
            if (memberInfo.MemberType == MemberTypes.Field)
                type = (memberInfo as FieldInfo).FieldType;
            else
                type = (memberInfo as PropertyInfo).PropertyType;

            if (type.Name.StartsWith("OutputPort"))
            {
                bool isGenericType = type.IsGenericType;
                Type genericType = isGenericType ? type.GetGenericArguments()[0] : typeof(void);

                string prefix;
                outputPortConfig.type = GetFlowNodePortType(genericType, out prefix, portAttribute);

                return true;
            }

            return false;
        }

        NodePortType GetFlowNodePortType(Type type, out string prefix, PortAttribute portAttribute)
        {
            prefix = null;

            if (type == typeof(void))
                return NodePortType.Void;
            if (type == typeof(int))
                return NodePortType.Int;
            if (type == typeof(float))
                return NodePortType.Float;
            if (type == typeof(string))
            {
                switch (portAttribute.StringPortType)
                {
                    case StringPortType.Sound:
                        prefix = "sound_";
                        break;
                    case StringPortType.DialogLine:
                        prefix = "dialogline_";
                        break;
                    case StringPortType.Texture:
                        prefix = "texture_";
                        break;
                    case StringPortType.Object:
                        prefix = "object_";
                        break;
                    case StringPortType.File:
                        prefix = "file_";
                        break;
                    case StringPortType.EquipmentPack:
                        prefix = "equip_";
                        break;
                    case StringPortType.ReverbPreset:
                        prefix = "reverbpreset_";
                        break;
                    case StringPortType.GameToken:
                        prefix = "gametoken_";
                        break;
                    case StringPortType.Material:
                        prefix = "mat_";
                        break;
                    case StringPortType.Sequence:
                        prefix = "seq_";
                        break;
                    case StringPortType.Mission:
                        prefix = "mission_";
                        break;
                    case StringPortType.Animation:
                        prefix = "anim_";
                        break;
                    case StringPortType.AnimationState:
                        prefix = "animstate_";
                        break;
                    case StringPortType.AnimationStateEx:
                        prefix = "animstateEx_";
                        break;
                    case StringPortType.Bone:
                        prefix = "bone_";
                        break;
                    case StringPortType.Attachment:
                        prefix = "attachment_";
                        break;
                    case StringPortType.Dialog:
                        prefix = "dialog_";
                        break;
                    case StringPortType.MaterialParamSlot:
                        prefix = "matparamslot_";
                        break;
                    case StringPortType.MaterialParamName:
                        prefix = "matparamname_";
                        break;
                    case StringPortType.MaterialParamCharacterAttachment:
                        prefix = "matparamcharatt_";
                        break;
                }

                return NodePortType.String;
            }
            if (type == typeof(Vec3))
                return NodePortType.Vec3;
            if (type == typeof(Color))
            {
                prefix = "color_";
                return NodePortType.Vec3; // implicit operator takes care of conversion
            }
            if (type == typeof(bool))
                return NodePortType.Bool;
            if (type == typeof(EntityId))
                return NodePortType.EntityId;

            throw new ArgumentException("Invalid flownode port type specified!");
        }
        #endregion

        #region Gamemode
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
        #endregion
	}
}
