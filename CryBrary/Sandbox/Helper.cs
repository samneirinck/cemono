using System;
using System.Collections.Generic;
using System.Windows.Forms;
using CryEngine.Extensions;

namespace CryEngine.Sandbox
{
	internal static class FormHelper
	{
		private static FormLoader instance;

		internal static void Init()
		{
			FormHelper.AvailableForms = new List<FormInfo>();
			CCommand.Register("mono_extensions", (args, cmd) =>
			{
				if(instance == null)
					instance = new FormLoader();

				instance.Show();
			});
		}

		public static void RegisterInternal<T>() where T : Form
		{
			var type = typeof(T);
			SandboxExtensionAttribute attr;

			if(type.TryGetAttribute(out attr))
				AvailableForms.Add(new FormInfo { Type = type, Data = attr });
			else
				throw new Exception("The internal Sandbox extension of type {0} has no SandboxExtensionAttribute.");
		}

		public static List<FormInfo> AvailableForms { get; set; }
	}

	/// <summary>
	/// Describes an available Sandbox extension.
	/// </summary>
	internal class FormInfo
	{
		/// <summary>
		/// The type of the form, guaranteed to inherit from System.Windows.Forms.Form.
		/// </summary>
		public Type Type { get; set; }

		/// <summary>
		/// The name of the extension.
		/// </summary>
		public string Name { get { return Data.Name; } }

		/// <summary>
		/// Extension metadata such as author info.
		/// </summary>
		public SandboxExtensionAttribute Data { get; set; }
	}
}
