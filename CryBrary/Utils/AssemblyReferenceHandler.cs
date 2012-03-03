using System.Collections.Generic;
using System.Xml.Linq;

using System.IO;

namespace CryEngine.Utils
{
	/// <summary>
	/// Handles retrieval of required assemblies for compiled scripts etc.
	/// </summary>
	public class AssemblyReferenceHandler
	{
        public AssemblyReferenceHandler()
        {
            ReferencedAssemblies = new List<string>();
        }

		/// <summary>
		/// Gets the required assemblies for the scripts passed to the method.
		/// Note: Does NOT exclude assemblies already loaded by CryMono.
		/// </summary>
		/// <param name="scripts"></param>
		/// <returns></returns>
		public IEnumerable<string> GetRequiredAssembliesForScripts(IEnumerable<string> scripts)
		{
			var namespaces = new List<string>();
			var assemblyPaths = new List<string>();

			foreach (var script in scripts)
			{
				foreach (var assembly in GetRequiredAssembliesForScript(script))
				{
					if (!namespaces.Contains(assembly))
						namespaces.Add(assembly);
				}
			}

			foreach (var nameSpace in namespaces)
				assemblyPaths.Add(ProcessNamespace(nameSpace));

			return assemblyPaths;
		}

		/// <summary>
		/// Gets the required assemblies for the script passed to the method.
		/// Note: Does NOT exclude assemblies already loaded by CryMono.
		/// </summary>
		/// <param name="script"></param>
		/// <returns></returns>
		protected IEnumerable<string> GetRequiredAssembliesForScript(string script)
		{
			if (string.IsNullOrEmpty(script))
				return null;

			var namespaces = new List<string>();

			using (var stream = new FileStream(script, FileMode.Open))
			{
				using (var reader = new StreamReader(stream))
				{
					string line;

					while ((line = reader.ReadLine()) != null)
					{
						//Filter for using statements
						if (line.StartsWith("using") && line.EndsWith(";"))
						{
							string Namespace = line.Replace("using ", "").Replace(";", "");
							if (!namespaces.Contains(Namespace))
							{
								namespaces.Add(Namespace);
								Namespace = null;
							}
						}
					}
				}
			}

			return namespaces.ToArray();
		}

		protected string ProcessNamespace(string name)
		{
			XDocument assemblyLookup = XDocument.Load(Path.Combine(PathUtils.GetEngineFolder(), "Mono", "assemblylookup.xml"));
			foreach (var node in assemblyLookup.Descendants())
			{
				if (node.Name.LocalName == "Namespace" && node.Attribute("name").Value == name)
				{
					string assemblyName = node.Parent.Attribute("name").Value;

					string[] assemblies = Directory.GetFiles(Path.Combine(PathUtils.GetEngineFolder(), "Mono", "lib", "mono", "gac"), "*.dll", SearchOption.AllDirectories);
					foreach (var assembly in assemblies)
					{
						if (assembly.Contains(assemblyName))
						{
							assemblyName = assembly;
							break;
						}
					}

					if (!ReferencedAssemblies.Contains(assemblyName))
						return assemblyName;
				}
			}

			return null;
		}

		/// <summary>
		/// All libraries passed through ScriptCompiler.LoadLibrariesInFolder will be automatically added to this list.
		/// </summary>
		public List<string> ReferencedAssemblies {get; protected set;}
	}
}
