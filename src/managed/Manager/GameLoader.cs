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
        public ConsoleRedirector ConsoleRedirector { get; set; }

        private void Init()
        {
            Console.SetOut(ConsoleRedirector);
            Console.SetError(ConsoleRedirector);
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
            //parameters.ReferencedAssemblies.Add("Cemono.bcl.dll");

            Stopwatch compileStopwatch = new Stopwatch();
            compileStopwatch.Start();
            CompilerResults results = provider.CompileAssemblyFromFile(parameters, filesToCompile);
            compileStopwatch.Stop();

            Console.WriteLine("Compilation finished in {0}ms", compileStopwatch.ElapsedMilliseconds);

            if (results.CompiledAssembly != null)
            {
                LoadGameAssembly(results.CompiledAssembly);
            }


        }

        private void LoadGameAssembly(Assembly assembly)
        {
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

                }
                else if (entityType.IsAssignableFrom(type) && !type.Equals(entityType))
                {

                }
            }


        }
    }
}
