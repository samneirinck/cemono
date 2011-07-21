using System;
using System.CodeDom.Compiler;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Linq;
using Microsoft.CSharp;

namespace Cemono
{
    public class GameLoader : MarshalByRefObject
    {
        private void Init()
        {
        }
        public void CompileAndLoad(string pathToMono, string pathToSourceFiles)
        {
            Init();
            string[] filesToCompile = Directory.GetFiles(pathToSourceFiles, "*.cs", SearchOption.AllDirectories);

            CSharpCodeProvider provider = new CSharpCodeProvider();
            CompilerParameters parameters = new CompilerParameters();

            parameters.GenerateExecutable = false;
            parameters.GenerateInMemory = true;
#if DEBUG
            parameters.IncludeDebugInformation = true;
#else
            parameters.IncludeDebugInformation = false;
#endif
            // TODO: Add more references
            parameters.ReferencedAssemblies.Add("System.dll");

            var assemblies = AppDomain.CurrentDomain.GetAssemblies().Select(a => a.Location);
            foreach (var assemblyPath in assemblies)
            {
                parameters.ReferencedAssemblies.Add(assemblyPath);
            }

            Stopwatch compileStopwatch = new Stopwatch();
            compileStopwatch.Start();
            CompilerResults results = provider.CompileAssemblyFromFile(parameters, filesToCompile);
            compileStopwatch.Stop();

            Console.WriteLine("Compilation finished in {0}ms", compileStopwatch.ElapsedMilliseconds);

            // Log compilation result
            foreach (var item in results.Output)
            {
                Console.WriteLine(item);
            }

            if (results.CompiledAssembly != null)
            {
                LoadGameAssembly(results.CompiledAssembly);
            }


        }

        private void LoadGameAssembly(Assembly assembly)
        {
            Console.WriteLine("Loading game assembly " + assembly.ToString());
            if (assembly == null)
            {
                throw new ArgumentNullException("Tried loading a NULL game assembly");
            }

            Type baseGameType = typeof(BaseGame);
            Type entityType = typeof(Entity);

            foreach (Type type in assembly.GetTypes())
            {
                if (baseGameType.IsAssignableFrom(type) && !type.Equals(baseGameType))
                {
                    Console.WriteLine("Loading game assembly");
                    Activator.CreateInstance(type);
                }
                else if (entityType.IsAssignableFrom(type) && !type.Equals(entityType))
                {

                }
            }


        }
    }
}
