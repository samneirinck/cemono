using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace CryEngine.Sandbox
{
	internal static class FormHelper
	{
		static FormHelper()
		{
			FormHelper.AvailableForms = new List<FormInfo>();
			CCommand.Register("mono_extensions", (args, cmd) => new FormLoader().Show());
		}

		internal static List<FormInfo> AvailableForms { get; set; }
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
