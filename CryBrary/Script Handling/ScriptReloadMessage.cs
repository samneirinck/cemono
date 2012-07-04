using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using System.Diagnostics;
using System.CodeDom.Compiler;

namespace CryEngine.Script_Handling
{
	public partial class ScriptReloadMessage : Form
	{
		public ScriptReloadMessage(CompilerResults compilationResults, bool canRevert)
		{
			InitializeComponent();

			//tryAgainButton.Click += (s, a) => ;
			//revertButton.Click += (s, a) => ;
			exitButton.Click += (s, a) => Process.GetCurrentProcess().Kill();

			if (!canRevert)
				revertButton.Enabled = false;

			string compilationError = string.Format("Compilation failed; {0} errors: ", compilationResults.Errors.Count);

			foreach (CompilerError error in compilationResults.Errors)
			{
				compilationError += Environment.NewLine;

				if (!error.ErrorText.Contains("(Location of the symbol related to previous error)"))
					compilationError += string.Format("{0}({1},{2}): {3} {4}: {5}", error.FileName, error.Line, error.Column, error.IsWarning ? "warning" : "error", error.ErrorNumber, error.ErrorText);
				else
					compilationError += "	" + error.ErrorText;
			}

			errorBox.Text = compilationError;
		}
	}
}
