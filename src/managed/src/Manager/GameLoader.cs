using System;
using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using CryEngine;
using CryEngine.FlowSystem;
using Microsoft.CSharp;
using Cemono.Extensions;
using System.Diagnostics;

namespace Cemono
{
    public class GameLoader : MarshalByRefObject
    {
        public ConsoleRedirector ConsoleRedirector { get; set; }

        private void Init()
        {
            Console.SetOut(ConsoleRedirector);
            Console.SetError(ConsoleRedirector);
        }

        private void LoadGameAssembly(Assembly assembly)
        {
            if (assembly == null)
            {
                throw new ArgumentNullException("Tried loading a NULL game assembly");
            }

            Type baseGameType = typeof(BaseGame);
            Type baseEntityType = typeof(Entity);
            Type baseFlowNodeType = typeof(FlowNode);

            Type gameType = null;
            List<Type> entityTypes = new List<Type>();
            List<Type> flowNodeTypes = new List<Type>();

            foreach (Type type in assembly.GetTypes())
            {
                if (gameType == null && type.Implements(baseGameType))
                {
                    gameType = type;
                }
                if (type.Implements(baseEntityType))
                {
                    entityTypes.Add(type);
                }
                if (type.Implements(baseFlowNodeType))
                {
                    flowNodeTypes.Add(type);
                }
            }

            if (entityTypes.Any())
            {
                LoadEntities(entityTypes);
            }

            if (flowNodeTypes.Any())
            {
                LoadFlowNodes(flowNodeTypes);
            }

            if (gameType != null)
            {
                LoadGame(gameType);
            }

        }

        private void LoadGame(Type gameType)
        {
        }

        private void LoadFlowNodes(List<Type> flowNodeTypes)
        {
            foreach (Type type in flowNodeTypes)
            {
                LoadFlowNode(type);
            }
        }

        private void LoadFlowNode(Type type)
        {
            Trace.TraceInformation("Loading flow node " + type.Name);

            string nodeCategory = "Cemono";
            string nodeName = type.Name;
            var categoryAttribute = type.GetAttribute<NodeCategoryAttribute>();
            if (categoryAttribute != null)
            {
                nodeCategory = categoryAttribute.Category;
            }

            FlowManager.RegisterNode(nodeCategory, nodeName);

        }

        private void LoadEntities(List<Type> entityTypes)
        {
            foreach (Type type in entityTypes)
            {
                LoadEntity(type);
            }
        }

        private void LoadEntity(Type type)
        {
            var entityAttribute = type.GetAttribute<EntityAttribute>();
            IList<EntityProperty> entityProperties = new List<EntityProperty>();
            foreach (PropertyInfo property in type.GetProperties())
            {
                var propertyAttribute = property.GetAttribute<PropertyAttribute>();
                var entityProperty = new EntityProperty();
                if (propertyAttribute != null)
                {
                    if (!String.IsNullOrEmpty(propertyAttribute.Description))
                    {
                        entityProperty.Description = propertyAttribute.Description;
                    }

                }

                entityProperties.Add(entityProperty);
            }
        }

        public void CompileAndLoadScripts(Folders folders)
        {
            Init();
            string languageExtension = "*.cs";

            List<string> filesToCompile = new List<string>();
            if (Directory.Exists(folders.EntitiesFolder))
                filesToCompile.AddRange(Directory.GetFiles(folders.EntitiesFolder, languageExtension, SearchOption.AllDirectories));
            if (Directory.Exists(folders.NodeFolder))
                filesToCompile.AddRange(Directory.GetFiles(folders.NodeFolder, languageExtension, SearchOption.AllDirectories));
            if (Directory.Exists(folders.LogicFolder))
                filesToCompile.AddRange(Directory.GetFiles(folders.LogicFolder, languageExtension, SearchOption.AllDirectories));

            CodeDomProvider provider = new CSharpCodeProvider();
            CompilerParameters compilerParameters = new CompilerParameters();

            compilerParameters.GenerateExecutable = false;
            compilerParameters.GenerateInMemory = true;
#if DEBUG
            compilerParameters.IncludeDebugInformation = true;
#else
            parameters.IncludeDebugInformation = false;
#endif
            // TODO: Add more references
            compilerParameters.ReferencedAssemblies.Add("System.dll");

            var assemblies = AppDomain.CurrentDomain.GetAssemblies().Select(a => a.Location);
            foreach (var assemblyPath in assemblies)
            {
                compilerParameters.ReferencedAssemblies.Add(assemblyPath);
            }

            CompilerResults results = provider.CompileAssemblyFromFile(compilerParameters, filesToCompile.ToArray());

            //// Log compilation result
            //foreach (var item in results.Output)
            //{
            //    Console.WriteLine(item);
            //}

            if (results.CompiledAssembly != null)
            {
                LoadGameAssembly(results.CompiledAssembly);
            }
        }
    }
}
